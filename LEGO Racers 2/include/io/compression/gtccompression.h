#ifndef LR2_GTCCOMPRESSION_H
#define LR2_GTCCOMPRESSION_H

#include "types.h"

LegoU8* DecompressGtcBlock(const LegoU8* p_source, LegoU8* p_destination, LegoBool32 p_skipHeader);

#endif // LR2_GTCCOMPRESSION_H
