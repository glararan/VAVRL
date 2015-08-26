#include "IRRemote.h"

#include <avr/interrupt.h>

#ifndef IR_DDR
	#define IR_DDR  DDRD
	#define IR_PORT PORTD
	#define IR_PINP PIND
	#define IR_PIN  PORTD2
#endif

namespace VAVRL
{
	volatile IRparams_t IRparams;
	
	int MATCH(int measured, int desired)
	{
		return measured >= TICKS_LOW(desired) && measured <= TICKS_HIGH(desired);
	}
	
	int MATCH_MARK(int measured_ticks, int desired_us)
	{
		return measured_ticks >= TICKS_LOW(desired_us + MARK_EXCESS) && measured_ticks <= TICKS_HIGH(desired_us + MARK_EXCESS);
	}
	
	int MATCH_SPACE(int measured_ticks, int desired_us)
	{
		return measured_ticks >= TICKS_LOW(desired_us - MARK_EXCESS) && measured_ticks <= TICKS_HIGH(desired_us - MARK_EXCESS);
	}
	
	IRRemoteDecoder::IRRemoteDecoder()
	{		
		rawbuf = (volatile uint16_t*)IRparams.rawbuf;
		
		Reset();
	}
	
	void IRRemoteDecoder::Reset()
	{
		value = bits = rawlen = 0;
	}
	
	bool IRRemoteDecoder::Decode()
	{
		long data = 0;
		
		uint8_t offset = 1;
		
		if(!MATCH_MARK(rawbuf[offset], NEC_HDR_MARK))
			return false;
		
		++offset;
		
		if(IRparams.rawlen == 4 && MATCH_SPACE(rawbuf[offset], NEC_RPT_SPACE) && MATCH_MARK(rawbuf[offset + 1], NEC_BIT_MARK))
		{
			bits      = 0;
			lastValue = value == IR_REPEAT ? lastValue : value;
			value     = IR_REPEAT;
			
			return true;
		}
		
		if(IRparams.rawlen < 2 * NEC_BITS + 4)
			return false;
		
		if(!MATCH_SPACE(rawbuf[offset], NEC_HDR_SPACE))
			return false;
			
		++offset;
		
		for(uint8_t i = 0; i < NEC_BITS; ++i)
		{
			if(!MATCH_MARK(rawbuf[offset], NEC_BIT_MARK))
				return false;
			
			++offset;
			
			if(MATCH_SPACE(rawbuf[offset], NEC_ONE_SPACE))
				data = (data << 1) | 1;
			else if(MATCH_SPACE(rawbuf[offset], NEC_ZERO_SPACE))
				data <<= 1;
			else
				return false;
				
			++offset;
		}
		
		bits  = NEC_BITS;
		value = data;
		
		return true;
	}
	
	IRRemote::IRRemote()
	{		
		EnableIR();
	}
	
	void IRRemote::EnableIR()
	{
		IR_DDR &= ~_BV(IR_PIN);
		
		Resume();
		
		cli();
		
		TIMER_CONFIG_NORMAL();
		
		TIMER_ENABLE_INTR;
		TIMER_RESET;
		
		sei();
	}
	
	bool IRRemote::GetCommand(IRRemoteDecoder* decoder)
	{
		decoder->setRawbuff(IRparams.rawbuf);
		decoder->setRawlen(IRparams.rawlen);
				
		if(IRparams.rcvstate != STATE_STOP)
			return false;
		
		if(decoder->Decode())
			return true;
			
		Resume(); // reset
		
		return false;
	}
	
	void IRRemote::Resume()
	{
		IRparams.rcvstate = STATE_IDLE;
		IRparams.rawlen   = 0;
	}
	
	ISR(TIMER_INTR_NAME)
	{
		TIMER_RESET;
		
		uint8_t IRdata = (IR_PINP & _BV(IR_PIN)) ? IR_SPACE : IR_MARK;
		
		IRparams.timer++; // One more 50us tick
		
		if (IRparams.rawlen >= RAWBUF) // Buffer overflow
			IRparams.rcvstate = STATE_STOP;
		
		switch(IRparams.rcvstate)
		{
			case STATE_IDLE: // In the middle of a gap
				{
					if(IRdata == IR_MARK)
					{
						if (IRparams.timer < 5000 / USECPERTICK) // Not big enough to be a gap.
							IRparams.timer = 0;
						else
						{
							// gap just ended, record duration and start recording transmission
							IRparams.rawlen                    = 0;
							IRparams.rawbuf[IRparams.rawlen++] = IRparams.timer;
							IRparams.timer                     = 0;
							IRparams.rcvstate                  = STATE_MARK;
						}
					}
				}
				break;
			
			case STATE_MARK: // timing MARK
				{
					if(IRdata == IR_SPACE) // MARK ended, record time
					{
						IRparams.rawbuf[IRparams.rawlen++] = IRparams.timer;
						IRparams.timer                     = 0;
						IRparams.rcvstate                  = STATE_SPACE;
					}
				}
				break;
				
			case STATE_SPACE: // timing SPACE
				{
					if(IRdata == IR_MARK)  // SPACE just ended, record it
					{
						IRparams.rawbuf[IRparams.rawlen++] = IRparams.timer;
						IRparams.timer                     = 0;
						IRparams.rcvstate                  = STATE_MARK;
					}
					else if(IRparams.timer > 5000 / USECPERTICK) // SPACE
					{
						// big SPACE, indicates gap between codes
						// Mark current code as ready for processing
						// Switch to STOP
						// Don't reset timer; keep counting space width
						IRparams.rcvstate = STATE_STOP;
					}
				}
				break;
				
			case STATE_STOP: // waiting, measuring gap
				{
					if(IRdata == IR_MARK) // reset gap timer
						IRparams.timer = 0;
				}
				break;
		}
	}
}