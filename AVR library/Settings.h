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

#endif // SETTINGS_H