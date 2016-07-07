#ifndef COLOR_H
#define COLOR_H

#include "Settings.h"
#include "Utility.h"
#include "lib8/Scale8.h"

typedef unsigned char     uint8_t;
typedef unsigned int      uint16_t;
typedef unsigned long int uint32_t;

#define K255 255
#define K171 171
#define K170 170
#define K85  85

#define FORCE_REFERENCE(var) asm volatile("" : : "r" (var))

namespace VAVRL
{
	struct HSB;

	/// Red: 0 - 255
	/// Green: 0 - 255
	/// Blue: 0 - 255
	struct RGB
	{
		uint8_t red, green, blue;
		
		RGB(uint8_t r, uint8_t g, uint8_t b)
		{
			red   = r;
			green = g;
			blue  = b;
		}
		
		RGB()
		{
		}
		
		inline bool operator==(RGB second)
		{
			if(red == second.red && green == second.green && blue == second.blue)
				return true;
			
			return false;
		}
	};

	/// Hue: 0 - 360
	/// Saturation: 0 - 1
	/// Brightness: 0 - 1
	struct HSB
	{
		float hue, saturation, brightness;
		
		HSB(float h, float s, float b)
		{
			hue        = h;
			saturation = s;
			brightness = b;
		}
		
		HSB()
		{
		}
		
		inline bool operator==(HSB second)
		{
			if(hue == second.hue && saturation == second.saturation && brightness == second.brightness)
				return true;
			
			return false;
		}
	};

	inline RGB computeAdjustment(uint8_t brightness = 255, const RGB& colorCorrection = RGB(255, 255, 255), const RGB& colorTemp = RGB(255, 255, 255))
	{
		RGB adj = RGB(0, 0, 0);

		if(brightness > 0)
		{
			uint8_t cc = colorCorrection.red;
			uint8_t ct = colorTemp.red;

			if(cc > 0 && ct > 0)
			{
				uint32_t work = (((uint32_t)cc) + 1) * (((uint32_t)ct) + 1) * brightness;
				work /= 0x10000L;

				adj.red = work & 0xFF;
			}

			cc = colorCorrection.green;
			ct = colorTemp.green;

			if(cc > 0 && ct > 0)
			{
				uint32_t work = (((uint32_t)cc) + 1) * (((uint32_t)ct) + 1) * brightness;
				work /= 0x10000L;

				adj.green = work & 0xFF;
			}

			cc = colorCorrection.blue;
			ct = colorTemp.blue;

			if(cc > 0 && ct > 0)
			{
				uint32_t work = (((uint32_t)cc) + 1) * (((uint32_t)ct) + 1) * brightness;
				work /= 0x10000L;

				adj.blue = work & 0xFF;
			}
		}

		return adj;
	}
	
	static const RGB KnownColors[] =
	{
		RGB(255, 255, 255), // white
		RGB(255,   0,   0), // red
		RGB(  0, 255,   0), // green
		RGB(  0,   0, 255), // blue
		RGB(255, 128, 128), // pink
		RGB(255, 255,   0), // yellow
		RGB(  0, 128, 255), // light blue
		RGB(255, 128,   0), // orange
		RGB(128,   0, 255), // purple
		RGB(128, 255,   0), // limet
		RGB(255,   0, 128)  // pink-purple
	};

	namespace Convert
	{
		inline RGB toRGB(const HSB& hsb)
		{
			uint16_t slice = uint16_t(hsb.hue / 60.0f);

			float frac = hsb.hue / 60.0f - slice;
			float p = hsb.brightness * (1.0f - hsb.saturation);
			float q = hsb.brightness * (1.0f - frac * hsb.saturation);
			float t = hsb.brightness * (1.0f - (1.0f - frac) * hsb.saturation);

			switch(slice)
			{
				case 0:
				case 6:
					return RGB(uint8_t(hsb.brightness * 255.0f), uint8_t(t * 255.0f), uint8_t(p * 255.0f));

				case 1:
					return RGB(uint8_t(q * 255.0f), uint8_t(hsb.brightness * 255.0f), uint8_t(p * 255.0f));

				case 2:
					return RGB(uint8_t(p * 255.0f), uint8_t(hsb.brightness * 255.0f), uint8_t(t * 255.0f));

				case 3:
					return RGB(uint8_t(p * 255.0f), uint8_t(q * 255.0f), uint8_t(hsb.brightness * 255.0f));

				case 4:
					return RGB(uint8_t(t * 255.0f), uint8_t(p * 255.0f), uint8_t(hsb.brightness * 255.0f));

				case 5:
					return RGB(uint8_t(hsb.brightness * 255.0f), uint8_t(p * 255.0f), uint8_t(q * 255.0f));

				default:
					return RGB(0, 0, 0);
			}
		}

		inline HSB toHSB(const RGB& rgb)
		{
			uint8_t rgbMax = max(rgb.red, max(rgb.green, rgb.blue));
			uint8_t rgbMin = min(rgb.red, min(rgb.green, rgb.blue));

			if(rgbMax == 0)
				return HSB(0, 0, 0);

			float delta = (float)rgbMax - (float)rgbMin;
			float saturation = delta / (float)rgbMax;

			HSB hsb;

			if(rgb.red >= rgbMax)
				hsb = HSB((float)(rgb.green - rgb.blue) / delta, saturation, rgbMax / 255.0f);
			else if(rgb.green >= rgbMax)
				hsb = HSB(2.0 + (float)(rgb.blue - rgb.red) / delta, saturation, rgbMax / 255.0f);
			else
				hsb = HSB(4.0 + (float)(rgb.red - rgb.green) / delta, saturation, rgbMax / 255.0f);

			hsb.hue *= 60.0f;

			if(hsb.hue < 0.0)
				hsb.hue += 360.0f;

			return hsb;
		}

		inline const RGB hsv2rgb_rainbow(const HSB& hsb)
		{
			// Yellow has a higher inherent brightness than
			// any other color; 'pure' yellow is perceived to
			// be 93% as bright as white.  In order to make
			// yellow appear the correct relative brightness,
			// it has to be rendered brighter than all other
			// colors.
			// Level Y1 is a moderate boost, the default.
			// Level Y2 is a strong boost.
			const uint8_t Y1 = 1;
			const uint8_t Y2 = 0;
			
			// G2: Whether to divide all greens by two.
			// Depends GREATLY on your particular LEDs
			const uint8_t G2 = 0;
			
			// Gscale: what to scale green down by.
			// Depends GREATLY on your particular LEDs
			const uint8_t Gscale = 0;
			
			uint8_t hue = (uint8_t)(hsb.hue / 360.0f * 255.0f);
			uint8_t sat = (uint8_t)(hsb.saturation * 255.0f);
			uint8_t bri = (uint8_t)(hsb.brightness * 255.0f);
			
			uint8_t offset = hue & 0x1F; // 0..31
			
			uint8_t offset8 = offset; // offset8 = offset * 8
			{
				#if defined(__AVR__)
					// Left to its own devices, gcc turns "x <<= 3" into a loop
					// It's much faster and smaller to just do three single-bit shifts
					// So this business is to force that.
					offset8 <<= 1;
					asm volatile("");
					offset8 <<= 1;
					asm volatile("");
					offset8 <<= 1;
				#else // On ARM and other non-AVR platforms, we just shift 3.
					offset8 <<= 3;
				#endif
			}
			
			uint8_t third = scale8(offset8, (256 / 3)); // max = 85
			
			uint8_t r, g, b;
			
			if(!(hue & 0x80))
			{
				if(!(hue & 0x40)) // 0XX
				{
					// 00X
					//section 0-1
					if(!(hue & 0x20))
					{
						// 000
						//case 0: // R -> O
						r = K255 - third;
						g = third;
						b = 0;
						FORCE_REFERENCE(b);
					}
					else
					{
						// 001
						//case 1: // O -> Y
						if(Y1)
						{
							r = K171;
							g = K85 + third;
							b = 0;
							FORCE_REFERENCE(b);
						}

						if(Y2)
						{
							r = K170 + third;
							//uint8_t twothirds = (third << 1);
							uint8_t twothirds = scale8(offset8, ((256 * 2) / 3)); // max=170
							g = K85 + twothirds;
							b = 0;
							FORCE_REFERENCE(b);
						}
					}
				}
				else
				{
					//01X
					// section 2-3
					if(!(hue & 0x20))
					{
						// 010
						//case 2: // Y -> G
						if(Y1)
						{
							//uint8_t twothirds = (third << 1);
							uint8_t twothirds = scale8(offset8, ((256 * 2) / 3)); // max=170
							r = K171 - twothirds;
							g = K170 + third;
							b = 0;
							FORCE_REFERENCE(b);
						}
						if(Y2)
						{
							r = K255 - offset8;
							g = K255;
							b = 0;
							FORCE_REFERENCE(b);
						}
					}
					else
					{
						// 011
						// case 3: // G -> A
						r = 0;
						FORCE_REFERENCE(r);
						g = K255 - third;
						b = third;
					}
				}
			}
			else
			{
				// section 4-7
				// 1XX
				if(!(hue & 0x40))
				{
					// 10X
					if(!(hue & 0x20))
					{
						// 100
						//case 4: // A -> B
						r = 0;
						FORCE_REFERENCE(r);
						//uint8_t twothirds = (third << 1);
						uint8_t twothirds = scale8( offset8, ((256 * 2) / 3)); // max=170
						g = K171 - twothirds; //K170?
						b = K85  + twothirds;
					}
					else
					{
						// 101
						//case 5: // B -> P
						r = third;
						g = 0;
						FORCE_REFERENCE(g);
						b = K255 - third;
					}
				}
				else
				{
					if(!(hue & 0x20))
					{
						// 110
						//case 6: // P -- K
						r = K85 + third;
						g = 0;
						FORCE_REFERENCE(g);
						b = K171 - third;
					}
					else
					{
						// 111
						//case 7: // K -> R
						r = K170 + third;
						g = 0;
						FORCE_REFERENCE(g);
						b = K85 - third;
					}
				}
			}
			
			// This is one of the good places to scale the green down,
			// although the client can scale green down as well.
			if(G2)
				g = g >> 1;

			if(Gscale)
				g = scale8_video_LEAVING_R1_DIRTY(g, Gscale);
			
			// Scale down colors if we're desaturated at all
			// and add the brightness_floor to r, g, and b.
			if(sat != 255)
			{
				if(sat == 0)
					r = g = b = 255;
				else
				{
					//nscale8x3_video( r, g, b, sat);
					#if (SCALE8_FIXED==1)
						if(r)
							r = scale8_LEAVING_R1_DIRTY(r, sat);
						if(g)
							g = scale8_LEAVING_R1_DIRTY(g, sat);
						if(b)
							b = scale8_LEAVING_R1_DIRTY(b, sat);
					#else
						if(r)
							r = scale8_LEAVING_R1_DIRTY(r, sat) + 1;
						if(g)
							g = scale8_LEAVING_R1_DIRTY(g, sat) + 1;
						if(b)
							b = scale8_LEAVING_R1_DIRTY(b, sat) + 1;
					#endif

					cleanup_R1();
					
					uint8_t desat = 255 - sat;
					desat = scale8(desat, desat);
					
					uint8_t brightness_floor = desat;
					r += brightness_floor;
					g += brightness_floor;
					b += brightness_floor;
				}
			}
			
			// Now scale everything down if we're at value < 255.
			if(bri != 255)
			{
				bri = scale8_video_LEAVING_R1_DIRTY(bri, bri);

				if(bri == 0)
					r = g = b = 0;
				else
				{
					// nscale8x3_video( r, g, b, val);
					#if (SCALE8_FIXED==1)
						if(r)
							r = scale8_LEAVING_R1_DIRTY(r, bri);
						if(g)
							g = scale8_LEAVING_R1_DIRTY(g, bri);
						if(b)
							b = scale8_LEAVING_R1_DIRTY(b, bri);
					#else
						if(r)
							r = scale8_LEAVING_R1_DIRTY(r, bri) + 1;
						if(g)
							g = scale8_LEAVING_R1_DIRTY(g, bri) + 1;
						if(b)
							b = scale8_LEAVING_R1_DIRTY(b, bri) + 1;
					#endif

					cleanup_R1();
				}
			}
			
			// Here we have the old AVR "missing std X+n" problem again
			// It turns out that fixing it winds up costing more than
			// not fixing it.
			// To paraphrase Dr Bronner, profile! profile! profile!
			//asm volatile(  ""  :  :  : "r26", "r27" );
			//asm volatile (" movw r30, r26 \n" : : : "r30", "r31");
			return RGB(r, g, b);
		}
	}
}

#endif // COLOR_H