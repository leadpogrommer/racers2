#ifndef LR2_PROFILER_H
#define LR2_PROFILER_H

#include "charbuffer128.h"

void GetProfilerConfiguration(LegoFloat* p_scale, LegoU32* p_enabled, CharBuffer128* p_font);
void* BeginProfileRange(const CharBuffer24* p_name);
void EndProfileRange(void* p_range);

#endif // LR2_PROFILER_H
