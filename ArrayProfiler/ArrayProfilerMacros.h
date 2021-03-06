#pragma once

#include "ArrayProfiler.h"

#if defined(__GNUC__)  || defined( __clang__)

#  define ARRAY_PROFILER_FORCE_INLINE inline __attribute__ ((always_inline))
#  define ARRAY_PROFILER_NEVER_INLINE __attribute__ ((noinline))
#  define ARRAY_PROFILER_CURRENT_CPP_VERSION __cplusplus
#  define ARRAY_PROFILER_RETURN_ADDRESS __builtin_return_address(0)

#elif defined(_MSC_VER)

#  define ARRAY_PROFILER_FORCE_INLINE __forceinline
#  define ARRAY_PROFILER_NEVER_INLINE __declspec(noinline)
#  define ARRAY_PROFILER_CURRENT_CPP_VERSION _MSVC_LANG

#include <intrin.h>
#pragma intrinsic(_ReturnAddress)
#  define ARRAY_PROFILER_RETURN_ADDRESS _ReturnAddress()

#endif

#define ARRAY_PROFILER

#ifdef ARRAY_PROFILER

#ifndef ARRAY_PROFILER_ARRAY_CONSTRUCTOR
#define ARRAY_PROFILER_ARRAY_CONSTRUCTOR																\
{																										\
	arrayProfiler::RegisterArray_Construct(this, ARRAY_PROFILER_RETURN_ADDRESS);						\
}

#endif

#ifndef ARRAY_PROFILER_ARRAY_DESTRUCTOR
#define ARRAY_PROFILER_ARRAY_DESTRUCTOR																	\
{																										\
	arrayProfiler::RegisterArray_Destruct(this);														\
}

#endif

#ifndef ARRAY_PROFILER_ARRAY_EXPLICIT_RESERVE
#define ARRAY_PROFILER_ARRAY_EXPLICIT_RESERVE(RESERVE_ELEMENT_COUNT)
#endif

#ifndef ARRAY_PROFILER_ARRAY_IMPLICIT_REALLOCATION
#define ARRAY_PROFILER_ARRAY_IMPLICIT_REALLOCATION(REALLOCATED_ELEMENT_COUNT)
#endif


#else

#ifndef ARRAY_PROFILER_ARRAY_CONSTRUCTOR
#define ARRAY_PROFILER_ARRAY_CONSTRUCTOR
#endif

#ifndef ARRAY_PROFILER_ARRAY_DESTRUCTOR
#define ARRAY_PROFILER_ARRAY_DESTRUCTOR
#endif

#ifndef ARRAY_PROFILER_ARRAY_RESERVE
#define ARRAY_PROFILER_ARRAY_RESERVE(RESERVE_ELEMENT_COUNT)
#endif

#ifndef ARRAY_PROFILER_ARRAY_IMPLICIT_REALLOCATION
#define ARRAY_PROFILER_ARRAY_IMPLICIT_REALLOCATION(REALLOCATED_ELEMENT_COUNT)
#endif

#endif