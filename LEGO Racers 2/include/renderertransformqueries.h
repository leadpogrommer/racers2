#ifndef LEGORACERS2_RENDERERTRANSFORMQUERIES_H
#define LEGORACERS2_RENDERERTRANSFORMQUERIES_H

#include "types.h"

class Vector3;

// Callee-cleanup helper using the static DisplayRenderer's cached world-view matrix.
void __stdcall TransformStaticWorldViewPoint(const Vector3* p_source, Vector3* p_result);

#endif // LEGORACERS2_RENDERERTRANSFORMQUERIES_H
