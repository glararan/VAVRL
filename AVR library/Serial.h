#include "Settings.h"

typedef unsigned char uint8_t;
typedef unsigned int  uint16_t;

namespace VAVRL
{
	class Serial
	{
	public:
		static void Initialize(uint16_t baudrate_index);

		static void WriteChar(uint8_t data);
		static void WriteByte(uint8_t data);
		static void WriteString(const char* data);
		static void WriteString(const char* format, ...);
		static void WriteString(unsigned char* data);
		static void WriteInt(int data);
		static void WriteUInt16(uint16_t data);
	};
}