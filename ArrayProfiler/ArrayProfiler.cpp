#include "ArrayProfiler.h"

#include "Containers/Array.h"
#include <unordered_map>

namespace arrayProfiler
{
	struct ArrayProfilingData;

	std::unordered_map<void* /* Array constructed location */, ArrayProfilingData> _ArrayProfilingData;

	struct ArrayProfilingData
	{
		//bool mIsInplaceArray;
		//size_t mInPlaceArrayInternalBufferElementCount;

		void* mArrayConstructedAddresss = 0;
		void* mArrayObjectAddress = 0;
		size_t mInitialReservationElementCount = 0;

		size_t mImplicitReallocationCount = 0;

		size_t mPeakElementCount = 0;
	};


	void RegisterArray_Construct(const void* const arrayLocation, const void* const constructedCodeLocation)
	{

	}

	void RegisterArray_Destruct(const void* const arrayLocation)
	{

	}

	void RegisterArray_ExplicitReserve(const void* const arrayLocation, const size_t reservationSize)
	{

	}

	void RegisterArray_BufferReallocation(const void* const arrayLocation, const size_t reservationSize)
	{
	}

	void RegisterArray_ElementCountUpdated(const void* const arrayLocation, const size_t currentElementCount)
	{
	}
}
