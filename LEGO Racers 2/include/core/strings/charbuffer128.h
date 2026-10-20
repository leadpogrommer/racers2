#ifndef LR2_CHARBUFFER128_H
#define LR2_CHARBUFFER128_H

#include "core/strings/charbuffer24.h"
#include "types.h"

// The constructor writes 128 bytes. The original type name is unknown.
struct CharBuffer128 {
	CharBuffer128();
	CharBuffer128(const LegoChar* p_text);
	void AppendTilde();
	LegoChar* FindAfterDot();
	void SetText(const LegoChar* p_text);
	void CopyBasenameTo(CharBuffer24* p_output);
	void Uppercase();
	void CopyPathWithSeparatorTo(LegoChar* p_output, const LegoChar* p_component);

	LegoChar m_data[128]; // 0x00
};

#endif // LR2_CHARBUFFER128_H
