#ifndef LR2_DIAGNOSTICS_H
#define LR2_DIAGNOSTICS_H

#include "types.h"

void ReportFatalError(const LegoChar* p_format, ...);
void PrintDiagnostic(const LegoChar* p_format, ...);

#endif // LR2_DIAGNOSTICS_H
