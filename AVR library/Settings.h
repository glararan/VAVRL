#ifndef SETTINGS_H
#define SETTINGS_H

#ifndef F_CPU
	#define F_CPU 16000000UL
#endif

/// DHT11 & DHT22 sensor settings
#define DHT_DDR  DDRB
#define DHT_PORT PORTB
#define DHT_PINP PINB
#define DHT_PIN  PORTB1

/// IR Remote settings
#define IR_DDR  DDRD
#define IR_PORT PORTD
#define IR_PINP PIND
#define IR_PIN  PORTD2

// LED strips settings
#define UCS1903_PORT PORTD
#define UCS1903_PIN  PORTD7

#define UCS1903_LAMPPORT PORTB
#define UCS1903_LAMPPIN  PORTB1

// MSGEQ7 settings
#define MSGEQ7_SMOOTH 0 // 0-255
#define MSGEQ7_ANALOG_PIN 1

#define MSGEQ7_DDR DDRD
#define MSGEQ7_PORT PORTD
#define MSGEQ7_STROBE_PIN PORTD2
#define MSGEQ7_RESET_PIN PORTD3

#endif // SETTINGS_H