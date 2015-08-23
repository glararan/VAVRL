#ifndef F_CPU
	#define F_CPU 16000000UL
#endif

/// DHT11 & DHT22 sensor settings
#define DHT_DDR  DDRB
#define DHT_PORT PORTB
#define DHT_PINP PINB
#define DHT_PIN  PORTB1