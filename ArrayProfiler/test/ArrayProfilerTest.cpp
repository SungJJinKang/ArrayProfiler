#include "../ArrayProfiler.h"

#include <cstdlib>
#include <new>
#include <cassert>
#include <utility>

namespace arrayProfiler
{
	template <typename ELEMENT_TYPE>
	class Array
	{
		ELEMENT_TYPE* mBufferBegin;
		ELEMENT_TYPE* mBufferEnd;
		ELEMENT_TYPE* mBufferCapacityEnd;

	private:

		inline void Destroy();
		inline void NullifyBufferPtr();
		inline void ResizeGrow(const size_t size);
		inline void Expand();

	public:

		inline Array();
		inline ~Array();
		inline Array(const Array& testArray);
		inline Array(Array&& testArray) noexcept;
		Array& operator=(const Array& testArray);
		inline Array& operator=(Array&& testArray) noexcept;
		inline void Reserve(const size_t size);
		inline void Push_Back(const ELEMENT_TYPE& element);
		inline void Push_Back(ELEMENT_TYPE&& element);
		template<typename... ARGS>
		inline void Emplace_Back(ARGS&&... args);
		inline bool Empty() const;
		inline size_t Count() const;
		inline size_t Capacity() const;
		inline ELEMENT_TYPE& operator[](const size_t index);
		inline const ELEMENT_TYPE& operator[](const size_t index) const;

		
	};

	template <typename ELEMENT_TYPE>
	inline void Array<ELEMENT_TYPE>::Destroy()
	{
		const size_t elementCount = mBufferEnd - mBufferBegin;
		for (size_t elementIndex = 0; elementIndex < elementCount; elementIndex++)
		{
			mBufferBegin[elementIndex].~ELEMENT_TYPE();
		}

		std::free(mBufferBegin);
	}

	template <typename ELEMENT_TYPE>
	inline void Array<ELEMENT_TYPE>::NullifyBufferPtr()
	{
		mBufferBegin = nullptr;
		mBufferEnd = nullptr;
		mBufferCapacityEnd = nullptr;
	}

	template <typename ELEMENT_TYPE>
	inline void Array<ELEMENT_TYPE>::ResizeGrow(const size_t reAllocElementCount)
	{
		assert(reAllocElementCount > Count());

		ELEMENT_TYPE* const newlyAllocatedBufferBegin = reinterpret_cast<ELEMENT_TYPE*>(std::malloc(reAllocElementCount * sizeof(ELEMENT_TYPE)));

		const size_t currentElementCount = Count();

		for (size_t elementIndex = 0; elementIndex < currentElementCount; elementIndex++)
		{
			new (newlyAllocatedBufferBegin + elementIndex) ELEMENT_TYPE(std::move(mBufferBegin[elementIndex]));
		}

		std::free(mBufferBegin);
		
		mBufferBegin = newlyAllocatedBufferBegin;
		mBufferEnd = newlyAllocatedBufferBegin + currentElementCount;
		mBufferCapacityEnd = newlyAllocatedBufferBegin + reAllocElementCount;

		ARRAY_PROFILER_ARRAY_IMPLICIT_REALLOCATION(reAllocElementCount)
	}

	template <typename ELEMENT_TYPE>
	inline void Array<ELEMENT_TYPE>::Expand()
	{
		const size_t currentCapacity = Capacity();
		ResizeGrow(currentCapacity == 0 ? (1) : (currentCapacity * 2));
	}

	template <typename ELEMENT_TYPE>
	inline Array<ELEMENT_TYPE>::Array()
		:
		mBufferBegin{ nullptr },
		mBufferEnd{ nullptr },
		mBufferCapacityEnd{ nullptr }
	{
		ARRAY_PROFILER_ARRAY_CONSTRUCTOR
	}

	template <typename ELEMENT_TYPE>
	inline Array<ELEMENT_TYPE>::~Array()
	{
		Destroy();
		NullifyBufferPtr();

		ARRAY_PROFILER_ARRAY_DESTRUCTOR
	}

	template <typename ELEMENT_TYPE>
	inline Array<ELEMENT_TYPE>::Array(const Array& testArray)
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

		ARRAY_PROFILER_ARRAY_CONSTRUCTOR
	}

	template <typename ELEMENT_TYPE>
	inline Array<ELEMENT_TYPE>::Array(Array&& testArray) noexcept
	{
		mBufferBegin = testArray.mBufferBegin;
		mBufferEnd = testArray.mBufferEnd;
		mBufferCapacityEnd = testArray.mBufferCapacityEnd;

		testArray.NullifyBufferPtr();

		ARRAY_PROFILER_ARRAY_CONSTRUCTOR
	}

	template <typename ELEMENT_TYPE>
	inline Array<ELEMENT_TYPE>& Array<ELEMENT_TYPE>::operator=(const Array<ELEMENT_TYPE>& testArray)
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

		ARRAY_PROFILER_ARRAY_CONSTRUCTOR
	}

	template <typename ELEMENT_TYPE>
	inline Array<ELEMENT_TYPE>& Array<ELEMENT_TYPE>::operator=(Array<ELEMENT_TYPE>&& testArray) noexcept
	{
		Destroy();

		mBufferBegin = testArray.mBufferBegin;
		mBufferEnd = testArray.mBufferEnd;
		mBufferCapacityEnd = testArray.mBufferCapacityEnd;

		testArray.NullifyBufferPtr();

		ARRAY_PROFILER_ARRAY_CONSTRUCTOR
	}

	template <typename ELEMENT_TYPE>
	inline void Array<ELEMENT_TYPE>::Reserve(const size_t reservationCount)
	{
		if(reservationCount > Count())
		{
			ResizeGrow(reservationCount);
			ARRAY_PROFILER_ARRAY_EXPLICIT_RESERVE(reservationCount);
		}

		
	}

	

	template <typename ELEMENT_TYPE>
	inline void Array<ELEMENT_TYPE>::Push_Back(const ELEMENT_TYPE& element)
	{
		if(Count() == Capacity())
		{
			Expand();
		}

		*mBufferEnd = element;
		mBufferEnd++;
	}

	template <typename ELEMENT_TYPE>
	inline void Array<ELEMENT_TYPE>::Push_Back(ELEMENT_TYPE&& element)
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
	inline void Array<ELEMENT_TYPE>::Emplace_Back(ARGS&&... args)
	{
		if (Count() == Capacity())
		{
			Expand();
		}

		new (mBufferEnd) ELEMENT_TYPE(std::move(args)...);
		mBufferEnd++;
	}

	template <typename ELEMENT_TYPE>
	inline bool Array<ELEMENT_TYPE>::Empty() const
	{
		return (mBufferBegin == mBufferEnd);
	}

	template <typename ELEMENT_TYPE>
	inline size_t Array<ELEMENT_TYPE>::Count() const
	{
		return mBufferEnd - mBufferBegin;
	}

	template <typename ELEMENT_TYPE>
	inline size_t Array<ELEMENT_TYPE>::Capacity() const
	{
		return mBufferCapacityEnd - mBufferBegin;
	}

	template <typename ELEMENT_TYPE>
	ELEMENT_TYPE& Array<ELEMENT_TYPE>::operator[](const size_t index)
	{
		assert(Count() > index);
		return mBufferBegin[index];
	}

	template <typename ELEMENT_TYPE>
	const ELEMENT_TYPE& Array<ELEMENT_TYPE>::operator[](const size_t index) const
	{
		assert(Count() > index);
		return mBufferBegin[index];
	}

	class TestClass
	{
		int data1;
		int data2;
		int data3;

		TestClass()
			: data1{ 0 }, data2{ 0 }, data3{ 0 }
		{
			
		}
		
		TestClass(int a, int b, int c)
			: data1{a}, data2{ b }, data3{ c }
		{

		}

		~TestClass()
		{
			
		}
	};
}



int main()
{
	
	arrayProfiler::Array<size_t> testArray{};
	for(size_t i = 0 ; i < 100 ; i++)
	{
		testArray.Push_Back(i);
	}

	assert(testArray[50] == 50);
	assert(testArray[21] == 21);

	testArray.~Array();
	

	_CrtDumpMemoryLeaks();

	return 0;
}