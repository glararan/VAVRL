#ifndef MSGEQ_H
#define MSGEQ_H

#include "Utility.h"

namespace VAVRL
{
	// FPS makro
	#define ReadsPerSecond(f) (1000000UL / f)

	// Basic frequencys definitions (0-6 valid, 7 channels)
	#define MSGEQ7_0 0
	#define MSGEQ7_1 1
	#define MSGEQ7_2 2
	#define MSGEQ7_3 3
	#define MSGEQ7_4 4
	#define MSGEQ7_5 5
	#define MSGEQ7_6 6
	#define MSGEQ7_BASS 0
	#define MSGEQ7_LOW 0
	#define MSGEQ7_MID 3
	#define MSGEQ7_HIGH 5

	// Resolution dependant settings
	#ifdef MSGEQ7_10BIT
		typedef uint16_t MSGEQ7_data_t;

		#define MSGEQ7_IN_MIN 80
		#define MSGEQ7_IN_MAX 1023
		#define MSGEQ7_OUT_MIN 0
		#define MSGEQ7_OUT_MAX 1023
	#else
		typedef uint8_t MSGEQ7_data_t;

		#define MSGEQ7_IN_MIN 20
		#define MSGEQ7_IN_MAX 255
		#define MSGEQ7_OUT_MIN 0
		#define MSGEQ7_OUT_MAX 255
	#endif

	struct frequency
	{
		MSGEQ7_data_t pinValue;
	};

	frequency frequencies[7];

	MSGEQ7_data_t mapNoise(MSGEQ7_data_t x, MSGEQ7_data_t minIn = MSGEQ7_IN_MIN, MSGEQ7_data_t maxIn = MSGEQ7_IN_MAX, MSGEQ7_data_t minOut = MSGEQ7_OUT_MIN, MSGEQ7_data_t maxOut = MSGEQ7_OUT_MAX)
	{
		// if input is smaller/bigger than expected return the min/max out ranges value
		if(x < minIn)
			return minOut;
		else if(x > maxIn)
			return maxOut;
		else if(maxIn - minIn > maxOut - minOut) // map the input to the output range, round up if mapping bigger ranges to smaller ranges
			return (uint16_t)(x - minIn) * (maxOut - minOut + 1) / (maxIn - minIn + 1) + minOut;
	
		// round down if mapping smaller ranges to bigger ranges
		return (uint16_t)(x - minIn) * (maxOut - minOut) / (maxIn - minIn) + minOut;
	}

	bool MSGEQ_read()
	{
		// reset the IC frequently, otherwise it will get out of sync after a while
		// only this setting seems to works properly
		MSGEQ7_PORT |= _BV(MSGEQ7_RESET_PIN);  // reset high

		_delay_us(0.1);

		MSGEQ7_PORT &= ~_BV(MSGEQ7_RESET_PIN); // reset low

		for (uint8_t i = 0; i < 7; i++) // read all 7 channels => 63Hz, 160Hz, 400Hz, 1kHz, 2.5kHz, 6.25KHz, 16kHz
		{
			// trigger next value
			MSGEQ7_PORT &= ~_BV(MSGEQ7_STROBE_PIN);  // strobe low

			_delay_us(30); // allow the output to settle

			// read pins with 8 bit resolution
			frequency f = (frequency)
			{
				#ifdef MSGEQ7_10BIT
					analogRead(MSGEQ7_ANALOG_PIN)
				#else
					MSGEQ7_data_t(analogRead(MSGEQ7_ANALOG_PIN) >> 2)
				#endif
			};

			if (MSGEQ7_SMOOTH) // Save smooth values
			{
				if(frequencies[i].pinValue < f.pinValue)  // Round up if value increases
					frequencies[i].pinValue++;

				frequencies[i].pinValue = uint16_t(frequencies[i].pinValue * MSGEQ7_SMOOTH + f.pinValue * (255 - MSGEQ7_SMOOTH)) / 255; // Smooth value
			}
			else // Save peek values
				frequencies[i] = f;

			MSGEQ7_PORT |= _BV(MSGEQ7_STROBE_PIN); // strobe high
		}

		return true;
	}

	bool MSGEQ_read(const uint32_t interval)
	{
		// Read without delay
		// TODO use static variable??
		static uint32_t prevMicros = 0;
		uint32_t currentMicros = micros();

		if((currentMicros - prevMicros) > interval)
		{
			prevMicros = currentMicros;

			MSGEQ_read(); // Analyze

			return true;
		}

		return false;
	}

	MSGEQ7_data_t MSGEQ_get(const uint8_t frequency)
	{
		if (frequency >= 7) // dont read out of bounds
			return 0;

		return frequencies[frequency].pinValue; // return the average of all channels
	}

	MSGEQ7_data_t MSGEQ_getVolume(void)
	{
		uint16_t vol = 0;

		for (uint8_t i = 0; i < 7; ++i) // add all frequencies of all channels to the overall volume
			vol += MSGEQ_get(i);

		return vol / 7; // return the average of all channels
	}
}

#endif // MSGEQ_H