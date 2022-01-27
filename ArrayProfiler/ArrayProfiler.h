#pragma once

#include "ArrayProfilerMacros.h"

namespace arrayProfiler
{
	extern void RegisterArray_Construct(const void* const arrayLocation, const void* const constructedCodeLocation);
	extern void RegisterArray_Destruct(const void* const arrayLocation);
	extern void RegisterArray_ExplicitReserve(const void* const arrayLocation, const size_t reservationSize);
	extern void RegisterArray_BufferGrowReallocation(const void* const arrayLocation, const size_t reservationSize);
	//extern void RegisterArray_ElementCountUpdated(const void* const arrayLocation, const size_t currentElementCount);
}