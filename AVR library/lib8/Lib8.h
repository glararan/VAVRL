#ifndef LIB8_H
#define LIB8_H

/// Copyright lib8tion

namespace VAVRL
{
	#if defined(__AVR_ATmega32U2__) || defined(__AVR_ATmega16U2__) || defined(__AVR_ATmega8U2__) || defined(__AVR_AT90USB162__) || defined(__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__) || defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__) || defined(__AVR_ATtiny167__) || defined(__AVR_ATtiny87__)
		#define LIB8_ATTINY 1
	#endif

	#if defined(__AVR__)
		// AVR ATmega and friends Arduino
		#define QADD8_C 0
		#define QADD7_C 0
		#define QSUB8_C 0
		#define ABS8_C 0
		#define ADD8_C 0
		#define SUB8_C 0
		#define AVG8_C 0
		#define AVG7_C 0
		#define AVG16_C 0
		#define AVG15_C 0

		#define QADD8_AVRASM 1
		#define QADD7_AVRASM 1
		#define QSUB8_AVRASM 1
		#define ABS8_AVRASM 1
		#define ADD8_AVRASM 1
		#define SUB8_AVRASM 1
		#define AVG8_AVRASM 1
		#define AVG7_AVRASM 1
		#define AVG16_AVRASM 1
		#define AVG15_AVRASM 1

		// Note: these require hardware MUL instruction
		//       -- sorry, ATtiny!
		#if !defined(LIB8_ATTINY)
			#define SCALE8_C 0
			#define SCALE16BY8_C 0
			#define SCALE16_C 0
			#define MUL8_C 0
			#define QMUL8_C 0
			#define EASE8_C 0
			#define SCALE8_AVRASM 1
			#define SCALE16BY8_AVRASM 1
			#define SCALE16_AVRASM 1
			#define MUL8_AVRASM 1
			#define QMUL8_AVRASM 1
			#define EASE8_AVRASM 1
			#define CLEANUP_R1_AVRASM 1
		#else
			// On ATtiny, we just use C implementations
			#define SCALE8_C 1
			#define SCALE16BY8_C 1
			#define SCALE16_C 1
			#define MUL8_C 1
			#define QMUL8_C 1
			#define EASE8_C 1
			#define SCALE8_AVRASM 0
			#define SCALE16BY8_AVRASM 0
			#define SCALE16_AVRASM 0
			#define MUL8_AVRASM 0
			#define QMUL8_AVRASM 0
			#define EASE8_AVRASM 0
		#endif
	#else
		// unspecified architecture, so
		// no ASM, everything in C
		#define QADD8_C 1
		#define QADD7_C 1
		#define QSUB8_C 1
		#define SCALE8_C 1
		#define SCALE16BY8_C 1
		#define SCALE16_C 1
		#define ABS8_C 1
		#define MUL8_C 1
		#define QMUL8_C 1
		#define ADD8_C 1
		#define SUB8_C 1
		#define EASE8_C 1
		#define AVG8_C 1
		#define AVG7_C 1
		#define AVG16_C 1
		#define AVG15_C 1
	#endif

	#define SCALE8_FIXED 1
}

#endif // LIB8_H