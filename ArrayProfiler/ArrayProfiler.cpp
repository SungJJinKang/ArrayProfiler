#include "ArrayProfiler.h"

#include <unordered_map>

namespace arrayProfiler
{
	struct ArrayProfilingData;

	std::unordered_map<void*, ArrayProfilingData> _ArrayProfilingData;

	struct ArrayProfilingData
	{
		void* mVariableDeclarationAddresss;
		size_t mInitialReservationElementCount;
		size_t mImplicitReallocationCount;

		ArrayProfilingData()
		:
		mInitialReservationElementCount {0},
		mImplicitReallocationCount{ 0 }
		{
			
		}
	};
}
