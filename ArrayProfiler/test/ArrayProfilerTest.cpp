#include "../ArrayProfiler.h"

#include <cstring>
#include <cstdlib>
#include <new>
#include <cassert>
#include <utility>

namespace arrayProfiler
{
	template <typename ELEMENT_TYPE>
	class TestArray
	{
		ELEMENT_TYPE* mBufferBegin;
		ELEMENT_TYPE* mBufferEnd;
		ELEMENT_TYPE* mBufferCapacityEnd;

	private:

		inline void Destroy();
		inline void NullifyBufferPtr();
		inline void Reallocate(const size_t size);
		inline void Expand();

	public:

		inline TestArray();
		inline ~TestArray();
		inline TestArray(const TestArray& testArray);
		inline TestArray(TestArray&& testArray) noexcept;
		TestArray& operator=(const TestArray& testArray);
		inline TestArray& operator=(TestArray&& testArray) noexcept;
		inline void Reserve(const size_t size);
		inline void Push_Back(const ELEMENT_TYPE& element);
		inline void Push_Back(ELEMENT_TYPE&& element);
		template<typename... ARGS>
		inline void Emplace_Back(ARGS&&... args);
		inline size_t Count() const;
		inline size_t Capacity() const;
		inline ELEMENT_TYPE& operator[](const size_t index);
		inline const ELEMENT_TYPE& operator[](const size_t index) const;

		
	};

	template <typename ELEMENT_TYPE>
	inline void TestArray<ELEMENT_TYPE>::Destroy()
	{
		if (mBufferBegin != nullptr)
		{
			const size_t elementCount = mBufferEnd - mBufferBegin;
			for (size_t elementIndex = 0; elementIndex < elementCount; elementIndex++)
			{
				mBufferBegin[elementIndex].~ELEMENT_TYPE();
			}

			std::free(mBufferBegin);
		}
	}

	template <typename ELEMENT_TYPE>
	inline void TestArray<ELEMENT_TYPE>::NullifyBufferPtr()
	{
		mBufferBegin = nullptr;
		mBufferEnd = nullptr;
		mBufferCapacityEnd = nullptr;
	}

	template <typename ELEMENT_TYPE>
	inline void TestArray<ELEMENT_TYPE>::Reallocate(const size_t reallocElementCount)
	{
		assert(reallocElementCount > Count());

		ELEMENT_TYPE* const tempBufferBegin = reinterpret_cast<ELEMENT_TYPE*>(std::malloc(reallocElementCount * sizeof(ELEMENT_TYPE)));

		const size_t currentElementCount = Count();

		for (size_t elementIndex = 0; elementIndex < currentElementCount; elementIndex++)
		{
			new (tempBufferBegin + elementIndex) ELEMENT_TYPE(std::move(mBufferBegin[elementIndex]));
		}

		mBufferBegin = tempBufferBegin;
		mBufferEnd = tempBufferBegin + currentElementCount;
		mBufferCapacityEnd = tempBufferBegin + reallocElementCount;
	}

	template <typename ELEMENT_TYPE>
	inline void TestArray<ELEMENT_TYPE>::Expand()
	{
		const size_t currentCapacity = Capacity();
		Reallocate(currentCapacity == 0 ? (1) : (currentCapacity * 2));
	}

	template <typename ELEMENT_TYPE>
	inline TestArray<ELEMENT_TYPE>::TestArray()
		:
		mBufferBegin{ nullptr },
		mBufferEnd{ nullptr },
		mBufferCapacityEnd{ nullptr }
	{

	}

	template <typename ELEMENT_TYPE>
	inline TestArray<ELEMENT_TYPE>::~TestArray()
	{
		Destroy();
		NullifyBufferPtr();
	}

	template <typename ELEMENT_TYPE>
	inline TestArray<ELEMENT_TYPE>::TestArray(const TestArray& testArray)
	{
		const size_t passedArrayElementSize = testArray.Count();
		if (passedArrayElementSize > 0)
		{
			mBufferBegin = reinterpret_cast<ELEMENT_TYPE*>(std::malloc(passedArrayElementSize * sizeof(ELEMENT_TYPE)));
			mBufferEnd = mBufferBegin + passedArrayElementSize;
			mBufferCapacityEnd = mBufferEnd;

			for (size_t elementIndex = 0; elementIndex < passedArrayElementSize; elementIndex++)
			{
				new (mBufferBegin + elementIndex) ELEMENT_TYPE(testArray.mBufferBegin[elementIndex]);
			}
		}
		else
		{
			NullifyBufferPtr();
		}
	}

	template <typename ELEMENT_TYPE>
	inline TestArray<ELEMENT_TYPE>::TestArray(TestArray&& testArray) noexcept
	{
		mBufferBegin = testArray.mBufferBegin;
		mBufferEnd = testArray.mBufferEnd;
		mBufferCapacityEnd = testArray.mBufferCapacityEnd;

		testArray.NullifyBufferPtr();
	}

	template <typename ELEMENT_TYPE>
	inline TestArray<ELEMENT_TYPE>& TestArray<ELEMENT_TYPE>::operator=(const TestArray<ELEMENT_TYPE>& testArray)
	{
		const size_t currentArrayElementCount = Count();
		const size_t passedArrayElementCount = testArray.Count();
		if (currentArrayElementCount >= passedArrayElementCount)
		{
			for (size_t elementIndex = 0; elementIndex < passedArrayElementCount; elementIndex++)
			{
				mBufferBegin[elementIndex] = testArray.mBufferBegin[elementIndex];
			}

			for (size_t elementIndex = passedArrayElementCount; elementIndex < currentArrayElementCount; elementIndex++)
			{
				mBufferBegin[elementIndex].~ELEMENT_TYPE();
			}

			mBufferEnd = mBufferBegin + passedArrayElementCount;
		}
		else
		{
			Destroy();

			mBufferBegin = reinterpret_cast<ELEMENT_TYPE*>(std::malloc(passedArrayElementCount * sizeof(ELEMENT_TYPE)));
			mBufferEnd = mBufferBegin + passedArrayElementCount;
			mBufferCapacityEnd = mBufferEnd;

			for (size_t elementIndex = 0; elementIndex < passedArrayElementCount; elementIndex++)
			{
				new (mBufferBegin + elementIndex) ELEMENT_TYPE(testArray.mBufferBegin[elementIndex]);
			}
		}
	}

	template <typename ELEMENT_TYPE>
	inline TestArray<ELEMENT_TYPE>& TestArray<ELEMENT_TYPE>::operator=(TestArray<ELEMENT_TYPE>&& testArray) noexcept
	{
		Destroy();

		mBufferBegin = testArray.mBufferBegin;
		mBufferEnd = testArray.mBufferEnd;
		mBufferCapacityEnd = testArray.mBufferCapacityEnd;

		testArray.NullifyBufferPtr();
	}

	template <typename ELEMENT_TYPE>
	inline void TestArray<ELEMENT_TYPE>::Reserve(const size_t reservationCount)
	{
		if(reservationCount > Count())
		{
			Reallocate(reservationCount);
		}
	}

	

	template <typename ELEMENT_TYPE>
	inline void TestArray<ELEMENT_TYPE>::Push_Back(const ELEMENT_TYPE& element)
	{
		if(Count() == Capacity())
		{
			Expand();
		}

		*mBufferEnd = element;
		mBufferEnd++;
	}

	template <typename ELEMENT_TYPE>
	inline void TestArray<ELEMENT_TYPE>::Push_Back(ELEMENT_TYPE&& element)
	{
		if (Count() == Capacity())
		{
			Expand();
		}

		*mBufferEnd = std::move(element);
		mBufferEnd++;
	}

	template <typename ELEMENT_TYPE>
	template <typename... ARGS>
	inline void TestArray<ELEMENT_TYPE>::Emplace_Back(ARGS&&... args)
	{
		if (Count() == Capacity())
		{
			Expand();
		}

		new (mBufferEnd) ELEMENT_TYPE(std::move(args)...);
		mBufferEnd++;
	}

	template <typename ELEMENT_TYPE>
	inline size_t TestArray<ELEMENT_TYPE>::Count() const
	{
		return mBufferEnd - mBufferBegin;
	}

	template <typename ELEMENT_TYPE>
	inline size_t TestArray<ELEMENT_TYPE>::Capacity() const
	{
		return mBufferCapacityEnd - mBufferBegin;
	}

	template <typename ELEMENT_TYPE>
	ELEMENT_TYPE& TestArray<ELEMENT_TYPE>::operator[](const size_t index)
	{
		assert(Count() > index);
		return mBufferBegin[index];
	}

	template <typename ELEMENT_TYPE>
	const ELEMENT_TYPE& TestArray<ELEMENT_TYPE>::operator[](const size_t index) const
	{
		assert(Count() > index);
		return mBufferBegin[index];
	}
}


int main()
{
	arrayProfiler::TestArray<float> testArray{};
	testArray.Emplace_Back(10);
	testArray.Emplace_Back(15);
	testArray.Emplace_Back(20);
	testArray.Push_Back(30);
	testArray.Emplace_Back(30);

	assert(testArray[2] == 20);
	assert(testArray[4] == 30);

	return 0;
}