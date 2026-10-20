#include "io/compression/gtccompression.h"

inline static LegoU32 ReadControlWord(const LegoU8*& p_source)
{
	LegoU32 word = p_source[3];
	word = (word << 8) + p_source[2];
	word = (word << 8) + p_source[1];
	word = (word << 8) + p_source[0];
	p_source += 4;
	return word;
}

inline static LegoU16 ReadMatchWord(const LegoU8*& p_source)
{
	LegoU16 word = p_source[0] + (p_source[1] << 8);
	p_source += 2;
	return word;
}

inline static void AdvanceControl(LegoU32& p_control, LegoU16& p_remaining, const LegoU8*& p_source)
{
	--p_remaining;
	p_control >>= 1;
	if (p_remaining == 0) {
		p_control = ReadControlWord(p_source);
		p_remaining = 32;
	}
}

// STUB: LEGORACERS2 0x004e7bb0
LegoU8* DecompressGtcBlock(const LegoU8* p_source, LegoU8* p_destination, LegoBool32 p_skipHeader)
{
	if (p_skipHeader) {
		const LegoU8* lengthField = p_source + 12;
		LegoU32 prefixLength = ReadControlWord(lengthField);
		p_source += prefixLength + 16;
	}

	LegoU32 control = ReadControlWord(p_source);
	LegoU16 remaining = 32;
	for (;;) {
		if ((control & 1) == 0) {
			*p_destination++ = *p_source++;
		}
		else {
			AdvanceControl(control, remaining, p_source);
			if ((control & 1) != 0) {
				LegoU16 descriptor = ReadMatchWord(p_source);
				LegoU8* match = p_destination + ((LegoS32) (descriptor >> 3) - 0x2000);
				LegoU16 count = descriptor & 7;
				if (count == 0) {
					count = *p_source++;
					if ((count & 0x80) != 0) {
						match -= 0x2000;
					}
					count &= 0x7f;
					if (count == 1) {
						return p_destination;
					}
					if (count == 0) {
						count = ReadMatchWord(p_source);
					}
					else {
						count += 2;
					}
				}
				else {
					count += 2;
				}
				while (count-- != 0) {
					*p_destination++ = *match++;
				}
			}
			else {
				LegoU8* match = p_destination + ((LegoS32) *p_source++ - 256);
				*p_destination++ = *match++;
				*p_destination++ = *match++;
				AdvanceControl(control, remaining, p_source);
				if ((control & 1) != 0) {
					*p_destination++ = *match++;
					*p_destination++ = *match++;
				}
				AdvanceControl(control, remaining, p_source);
				if ((control & 1) != 0) {
					*p_destination++ = *match;
				}
			}
		}
		AdvanceControl(control, remaining, p_source);
	}
}
