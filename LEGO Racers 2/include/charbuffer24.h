#ifndef LR2_CHARBUFFER24_H
#define LR2_CHARBUFFER24_H

#include "types.h"

// The constructor writes 24 bytes. The original type name is unknown.
struct CharBuffer24 {
	CharBuffer24();
	CharBuffer24(const LegoChar* p_text);

	LegoChar m_data[24]; // 0x00
};

#endif // LR2_CHARBUFFER24_H
