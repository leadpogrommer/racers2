#ifndef LR2_MENUTEXTPAIR_H
#define LR2_MENUTEXTPAIR_H

#include "globaloverlaymanager.h"

struct MenuTextPair {
	GlobalOverlayRecord* m_records[2];
	void SetClipRectangle(const LegoFloat*, const LegoFloat*);
	void GetRectangle(LegoFloat*, LegoFloat*);
};
DECOMP_SIZE_ASSERT(MenuTextPair, 8);

#endif
