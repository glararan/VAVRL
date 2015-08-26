#include "Settings.h"

typedef unsigned int    uint16_t;
typedef unsigned char   uint8_t;
typedef signed char     int8_t;
typedef signed int      int16_t;
typedef signed long int int32_t;

#define RESOLUTION 20

#define USECPERTICK 50

#define TOLERANCE 25
#define LTOL      (1.0 - (TOLERANCE / 100.))
#define UTOL      (1.0 + (TOLERANCE / 100.))

#define TICKS_LOW(us)   ((int)(((us) * LTOL / USECPERTICK)))
#define TICKS_HIGH(us)  ((int)(((us) * UTOL / USECPERTICK + 1)))

#define IR_REPEAT 0xFFFFFFFF

#define RAWBUF 100

#define MARK_EXCESS 100

#define TIMER_RESET
#define TIMER_ENABLE_PWM    (TCCR2A |= _BV(COM2B1))
#define TIMER_DISABLE_PWM   (TCCR2A &= ~(_BV(COM2B1)))
#define TIMER_ENABLE_INTR   (TIMSK2 = _BV(OCIE2A))
#define TIMER_DISABLE_INTR  (TIMSK2 = 0)
#define TIMER_INTR_NAME     TIMER2_COMPA_vect

#define TIMER_COUNT_TOP  (F_CPU * USECPERTICK / 1000000)

//-----------------
#if (TIMER_COUNT_TOP < 256)
	#define TIMER_CONFIG_NORMAL() ({ \
	TCCR2A = _BV(WGM21); \
	TCCR2B = _BV(CS20); \
	OCR2A  = TIMER_COUNT_TOP; \
	TCNT2  = 0; \
})
#else
	#define TIMER_CONFIG_NORMAL() ({ \
	TCCR2A = _BV(WGM21); \
	TCCR2B = _BV(CS21); \
	OCR2A  = TIMER_COUNT_TOP / 8; \
	TCNT2  = 0; \
})
#endif

#define NEC_BITS          32
#define NEC_HDR_MARK    9000
#define NEC_HDR_SPACE   4500
#define NEC_BIT_MARK     560
#define NEC_ONE_SPACE   1690
#define NEC_ZERO_SPACE   560
#define NEC_RPT_SPACE   2250

#define IR_MARK  0
#define IR_SPACE 1

namespace VAVRL
{
	enum ReceiveState
	{
		STATE_UNKNOWN,
		STATE_IDLE,
		STATE_MARK,
		STATE_SPACE,
		STATE_STOP,
		STATE_RUNNING
	};
	
	struct IRparams_t
	{
		ReceiveState rcvstate;	 // state machine
		
		uint16_t timer;	         // state timer, counts 50uS ticks.(and other uses)
		uint16_t rawbuf[RAWBUF]; // raw data
		uint8_t rawlen;          // counter of entries in rawbuf
	};
	
	extern volatile IRparams_t IRparams;
	
	int MATCH       (int measured,       int desired);
	int MATCH_MARK  (int measured_ticks, int desired_us);
	int MATCH_SPACE (int measured_ticks, int desired_us);
	
	class IRRemoteDecoder
	{
	public:	
		IRRemoteDecoder();
		
		bool Decode();
		
		void Reset();
		
		void ShowResults();
		
		unsigned long getValue()     { return value; }
		unsigned long getLastValue() { return lastValue; }
		
		void setRawlen(uint8_t data)               { rawlen = data; }
		void setRawbuff(volatile uint16_t* buffer) { rawbuf = buffer; }
		void setExtendedBuff(void* buff)           { rawbuf = (volatile uint16_t*)buff; } //Normally uses same rawbuf as IRrecv. Use this to define your own buffer.
		
	private:
		unsigned long lastValue;
		unsigned long value;       // Decoded value
		
		volatile uint16_t* rawbuf; // Raw intervals in microseconds
		
		uint8_t rawlen;            // Number of records in rawbuf.
		uint8_t bits;              // Number of bits in decoded value
	};
	
	class IRRemote
	{
	public:
		IRRemote();
	
		void EnableIR();
	
		bool GetCommand(IRRemoteDecoder* decoder);
		
		void Resume();
	};
}