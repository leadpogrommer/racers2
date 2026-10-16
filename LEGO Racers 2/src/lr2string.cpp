#include "charbuffer128.h"
#include "charbuffer24.h"

#include <string.h>

// FUNCTION: LEGORACERS2 0x004a2130
CharBuffer24::CharBuffer24(const LegoChar* p_text)
{
	memset(m_data, 0, sizeof(m_data));
	strcpy(m_data, p_text);
}

// FUNCTION: LEGORACERS2 0x004a2170
CharBuffer24::CharBuffer24()
{
	memset(m_data, 0, sizeof(m_data));
}

// FUNCTION: LEGORACERS2 0x004a2320
CharBuffer128::CharBuffer128(const LegoChar* p_text)
{
	memset(m_data, 0, sizeof(m_data));
	strcpy(m_data, p_text);
}

// FUNCTION: LEGORACERS2 0x004a2360
CharBuffer128::CharBuffer128()
{
	memset(m_data, 0, sizeof(m_data));
}

// FUNCTION: LEGORACERS2 0x004a2380
void CharBuffer128::AppendTilde()
{
	strcat(m_data, "~");
}

// FUNCTION: LEGORACERS2 0x004a23c0
LegoChar* CharBuffer128::FindAfterDot()
{
	LegoS32 index = 0;
	// The original scan is bounded by the buffer size and does not stop at NUL.
	while (index < 128 && m_data[index] != '.') {
		++index;
	}

	if (index == 128) {
		return NULL;
	}
	return m_data + index + 1;
}

// FUNCTION: LEGORACERS2 0x004a23e0
void CharBuffer128::SetText(const LegoChar* p_text)
{
	memset(m_data, 0, sizeof(m_data));
	strcpy(m_data, p_text);
}

// FUNCTION: LEGORACERS2 0x004a2420
void CharBuffer128::CopyBasenameTo(CharBuffer24* p_output)
{
	LegoChar* basename = strrchr(m_data, '\\');
	if (basename == NULL) {
		basename = m_data;
	}
	else {
		++basename;
	}

	strncpy(p_output->m_data, basename, 0x11);
	p_output->m_data[0x11] = '\0';

	LegoChar* extension = strrchr(p_output->m_data, '.');
	if (extension != NULL) {
		*extension = '\0';
	}
}

// FUNCTION: LEGORACERS2 0x004a2460
void CharBuffer128::Uppercase()
{
	LegoS32 index;
	for (index = 0; index < 128; ++index) {
		if (m_data[index] >= 'a' && m_data[index] <= 'z') {
			m_data[index] &= 0xdf;
		}
	}
}

// FUNCTION: LEGORACERS2 0x004a2480
void CharBuffer128::CopyPathWithSeparatorTo(LegoChar* p_output, const LegoChar* p_component)
{
	strcpy(p_output, m_data);
	strcat(p_output, "\\");
	strcat(p_output, p_component);
}
