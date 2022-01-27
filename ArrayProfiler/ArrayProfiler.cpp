#include "ArrayProfiler.h"

#include "Containers/Array.h"
#include <unordered_map>

namespace arrayProfiler
{
	struct ArrayProfilingData;

	static std::unordered_map<const void* /* Array Declaration Code location */, ArrayProfilingData> DestroyedArrayProfilingData{};

	static std::unordered_map<const void* /* Array object location */, ArrayProfilingData> ALiveArrayProfilingData{};

	struct ArrayProfilingDataRecord
	{
		
	};
	
	struct ArrayProfilingData
	{
		const void* mArrayConstructCodeLocation;
		const void* mArrayObjectAddress;

		bool mWasReallocated;
		size_t mInitialReservationElementCount = 0;

		size_t mImplicitGrowReallocationCount = 0;

		size_t mPeakElementCount = 0;
	};

	static ArrayProfilingData* FindALiveArrayProfilingData(const void* const arrayLocation)
	{
		ArrayProfilingData* profilingData = nullptr;

		auto node = ALiveArrayProfilingData.find(arrayLocation);
		if(node != ALiveArrayProfilingData.end())
		{
			profilingData = &(node->second);
		}

		return profilingData;
	}

	void RegisterArray_Construct(const void* const arrayLocation, const void* const constructedCodeLocation)
	{
		ArrayProfilingData arrayProfilingData{};
		arrayProfilingData.mArrayConstructCodeLocation = constructedCodeLocation;
		arrayProfilingData.mArrayObjectAddress = arrayLocation;

		ALiveArrayProfilingData.emplace(arrayLocation, arrayProfilingData);
	}

	static void UpdateDestroyedArrayProfilingData
	(
		ArrayProfilingData& destroyedArrayProfilingData,
		ArrayProfilingData& updatedArrayProfilingData
	)
	{
		
	}

	void RegisterArray_Destruct(const void* const arrayLocation)
	{
		ArrayProfilingData* const profilingData = FindALiveArrayProfilingData(arrayLocation);

		auto node = DestroyedArrayProfilingData.find(profilingData->mArrayConstructCodeLocation);
		if(node == DestroyedArrayProfilingData.end())
		{
			DestroyedArrayProfilingData.emplace(profilingData->mArrayConstructCodeLocation, *profilingData);
		}
		else
		{
			UpdateDestroyedArrayProfilingData(node->second, *profilingData);
		}
	}

	static void RegisterArray_ElementCountUpdated(ArrayProfilingData* const profilingData, const size_t currentElementCount)
	{
		profilingData->mPeakElementCount = ((currentElementCount > profilingData->mPeakElementCount) ? currentElementCount : profilingData->mPeakElementCount);
	}

	void RegisterArray_ExplicitReserve(const void* const arrayLocation, const size_t reservationSize)
	{
		ArrayProfilingData* const profilingData = FindALiveArrayProfilingData(arrayLocation);

		if(profilingData->mWasReallocated == false)
		{
			profilingData->mInitialReservationElementCount = reservationSize;
		}

		profilingData->mWasReallocated = true;
		RegisterArray_ElementCountUpdated(profilingData, reservationSize);
	}

	void RegisterArray_BufferGrowReallocation(const void* const arrayLocation, const size_t reservationSize)
	{
		ArrayProfilingData* const profilingData = FindALiveArrayProfilingData(arrayLocation);

		profilingData->mWasReallocated = true;
		RegisterArray_ElementCountUpdated(profilingData, reservationSize);
		profilingData->mImplicitGrowReallocationCount++;
	}

	
}
