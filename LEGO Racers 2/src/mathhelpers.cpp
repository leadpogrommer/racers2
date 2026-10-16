#include "mathhelpers.h"

#include "decomp.h"

// FUNCTION length: 0x24 bytes (0x004d15e0-0x004d1603).
// FUNCTION: LEGORACERS2 0x004d15e0
SplineBasis::SplineBasis(LegoU32 p_mode, LegoFloat p_parameter)
{
	m_unk0x80 = p_mode;
	m_unk0x84 = p_parameter;
	FUN_004d1610();
}

// FUNCTION length: 0x1b5 bytes (0x004d1610-0x004d17c4).
// STUB: LEGORACERS2 0x004d1610
void SplineBasis::FUN_004d1610()
{
	LegoFloat reciprocal;
	LegoFloat negativeReciprocal;
	LegoFloat twiceReciprocal;
	LegoFloat reciprocalMinusThree;

	switch (m_unk0x80) {
	case 1: {
		m_basis[0][0] = 0.166667f; // retail bits 0x3e2aaac1
		m_basis[0][1] = -0.5f;
		m_basis[0][2] = 0.5f;
		m_basis[0][3] = -0.166667f;
		m_basis[1][0] = 0.666667f; // retail bits 0x3f2aaab0
		m_basis[1][1] = 0.0f;
		m_basis[1][2] = -1.0f;
		m_basis[1][3] = 0.5f;
		m_basis[2][0] = 0.166667f;
		m_basis[2][1] = 0.5f;
		m_basis[2][2] = 0.5f;
		m_basis[2][3] = -0.5f;
		m_basis[3][0] = 0.0f;
		m_basis[3][1] = 0.0f;
		m_basis[3][2] = 0.5f;
		m_basis[3][3] = 0.166667f;
		FUN_004d17d0();
		return;
	}
	case 2: {
		m_basis[0][0] = 0.0f;
		m_basis[0][1] = -0.5f;
		m_basis[0][2] = 1.0f;
		m_basis[0][3] = -0.5f;
		m_basis[1][0] = 1.0f;
		m_basis[1][1] = 0.0f;
		m_basis[1][2] = -2.5f;
		m_basis[1][3] = 1.5f;
		m_basis[2][0] = 0.0f;
		m_basis[2][1] = 0.5f;
		m_basis[2][2] = 2.0f;
		m_basis[2][3] = -1.5f;
		m_basis[3][0] = 0.0f;
		m_basis[3][1] = 0.0f;
		m_basis[3][2] = -0.5f;
		m_basis[3][3] = 0.5f;
		FUN_004d17d0();
		return;
	}
	case 4: {
		m_basis[0][0] = 1.0f;
		m_basis[0][1] = -3.0f;
		m_basis[0][2] = 3.0f;
		m_basis[0][3] = -1.0f;
		m_basis[1][0] = 0.0f;
		m_basis[1][1] = 3.0f;
		m_basis[1][2] = -6.0f;
		m_basis[1][3] = 3.0f;
		m_basis[2][0] = 0.0f;
		m_basis[2][1] = 0.0f;
		m_basis[2][2] = 3.0f;
		m_basis[2][3] = -3.0f;
		m_basis[3][0] = 0.0f;
		m_basis[3][1] = 0.0f;
		m_basis[3][2] = 0.0f;
		m_basis[3][3] = 1.0f;
		FUN_004d17d0();
		return;
	}
	default: {

		negativeReciprocal = -1.0f / m_unk0x84;
		twiceReciprocal = 2.0f / m_unk0x84;
		reciprocal = 1.0f / m_unk0x84;
		reciprocalMinusThree = reciprocal - 3.0f;

		m_basis[1][0] = 1.0f;
		m_basis[0][0] = 0.0f;
		m_basis[1][1] = 0.0f;
		m_basis[2][0] = 0.0f;
		m_basis[3][0] = 0.0f;
		m_basis[3][1] = 0.0f;
		m_basis[0][1] = negativeReciprocal;
		m_basis[0][2] = twiceReciprocal;
		m_basis[0][3] = negativeReciprocal;
		m_basis[2][1] = reciprocal;
		m_basis[3][3] = reciprocal;
		m_basis[1][2] = reciprocalMinusThree;
		m_basis[1][3] = 2.0f - reciprocal;
		m_basis[2][2] = 3.0f - twiceReciprocal;
		m_basis[2][3] = reciprocal - 2.0f;
		m_basis[3][2] = negativeReciprocal;

		FUN_004d17d0();
		return;
	}
	}
}

// FUNCTION length: 0x2f bytes (0x004d17d0-0x004d17fe).
// FUNCTION: LEGORACERS2 0x004d17d0
void SplineBasis::FUN_004d17d0()
{
	LegoS32 row;

	for (row = 0; row < 4; ++row) {
		m_derivative[row][0] = 0.0f;
		m_derivative[row][1] = m_basis[row][1];
		m_derivative[row][2] = m_basis[row][2] * 2.0f;
		m_derivative[row][3] = m_basis[row][3] * 3.0f;
	}
}
