#include "Settings.h"

typedef unsigned char uint8_t;

#define MAX_TIMING 84 // how many timing transitions we need to keep track of. 2 * number bits + extra

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
	
		DHT(Type DHTtype);
		
		bool Process();
		
		uint8_t Temperature();
		uint8_t Humidity();
		
	private:
		Type type;
		
		uint8_t data[6];
		
		uint8_t humidity;
		uint8_t temperature;
		
		bool Read();
	};
}