#ifndef LR2_RENDERERRECORDGROUP_H
#define LR2_RENDERERRECORDGROUP_H

#include "decomp.h"
#include "types.h"

// Group initialized by 0x004abeb0. The constructor's caller advances by
// 0xc4 bytes between groups; each group contains four 0x30-byte records.
class RendererSubrecordGroup {
public:
	struct Subrecord {
		LegoU32 m_unk0x00[8]; // 0x00..0x1c, initialized to 0xffffffff
		LegoU32 m_unk0x20;    // 0x20, initialized to 0
		LegoU32 m_unk0x24;    // 0x24, preserved by constructor
		LegoU32 m_unk0x28;    // 0x28, initialized to 0
		LegoU32 m_unk0x2c;    // 0x2c, initialized to 1
	};

	RendererSubrecordGroup();

	Subrecord m_records[4]; // 0x00..0xbf
	LegoU32 m_unk0xc0;      // 0xc0, initialized to 0
};

DECOMP_SIZE_ASSERT(RendererSubrecordGroup::Subrecord, 0x30);
DECOMP_SIZE_ASSERT(RendererSubrecordGroup, 0xc4);

#endif // LR2_RENDERERRECORDGROUP_H
