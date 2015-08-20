#include <stdint.h>

#define MAX_TIMING 85 // how many timing transitions we need to keep track of. 2 * number bits + extra

namespace VAVRL
{
	class DHT
	{
	public:
		enum Type
		{
			DHT11,
			DHT21,
			DHT22	
		};
	
		DHT(Type DHTtype, volatile uint8_t* DDR, volatile uint8_t* Port, volatile uint8_t* PortPin, uint8_t Pin);
		
		bool Process();
		
		uint8_t Temperature();
		uint8_t Humidity();
		
	private:
		volatile uint8_t* ddr;
		volatile uint8_t* port;
		volatile uint8_t* portPin;
		volatile uint8_t  pin;
		
		Type type;
		
		uint8_t data[6];
		
		unsigned long lastReadTime;
		
		uint8_t humidity;
		uint8_t temperature;
		
		bool Read();
	};
}