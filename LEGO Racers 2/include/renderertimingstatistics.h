#ifndef LEGORACERS2_RENDERERTIMINGSTATISTICS_H
#define LEGORACERS2_RENDERERTIMINGSTATISTICS_H

#include "decomp.h"
#include "types.h"

#include <stddef.h>

// Counter storage observed by the renderer's once-per-second statistics
// sampler. Only the fields read by that sampler have semantic names.
struct RendererTimingStatisticsState {
	void GetMetrics(LegoFloat p_metrics[6]);

	LegoS32 m_counter0;                      // 0x000
	LegoS32 m_counter1;                      // 0x004
	LegoS32 m_counterHistory[0x158];         // 0x008
	LegoS32 m_pairCounters[8][2];            // 0x568
	LegoS32 m_tailCounters[2];               // 0x5a8
	LegoS32 m_previousCounter0;              // 0x5b0
	LegoS32 m_previousCounter1;              // 0x5b4
	LegoS32 m_counterHistorySnapshot[0x158]; // 0x5b8
	LegoS32 m_pairCounterSnapshot[8][2];     // 0xb18
	LegoS32 m_tailCounterSnapshot[2];        // 0xb58
};

DECOMP_SIZE_ASSERT(RendererTimingStatisticsState, 0xb60);
DECOMP_STATIC_ASSERT(offsetof(RendererTimingStatisticsState, m_pairCounters) == 0x568);
DECOMP_STATIC_ASSERT(offsetof(RendererTimingStatisticsState, m_previousCounter0) == 0x5b0);
DECOMP_STATIC_ASSERT(offsetof(RendererTimingStatisticsState, m_counterHistorySnapshot) == 0x5b8);
DECOMP_STATIC_ASSERT(offsetof(RendererTimingStatisticsState, m_pairCounterSnapshot) == 0xb18);
DECOMP_STATIC_ASSERT(offsetof(RendererTimingStatisticsState, m_tailCounterSnapshot) == 0xb58);

#endif // LEGORACERS2_RENDERERTIMINGSTATISTICS_H
