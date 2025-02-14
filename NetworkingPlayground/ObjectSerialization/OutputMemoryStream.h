#pragma once
#include "StreamTypes.h"
#include <cstdint>
#include <span>

namespace Stream {

	class OutputMemoryStream
	{
	public:
		OutputMemoryStream();
		~OutputMemoryStream();

		const char* GetBufferPtr() const { return mBuffer; }
		uint32_t GetLength() const noexcept { return mHead; }

		template <is_primitive_type T>
		void Write(const T& inData);

		template <is_primitive_type T, size_t N>
		void Write(T (& inData)[N]);

	private:
		void WriteInternal(const void* inData, size_t inByteCount);
		void ReallocBuffer(uint32_t inNewLength);

		char* mBuffer;
		uint32_t mHead;
		uint32_t mCapacity;
	};

	template<is_primitive_type T>
	inline void OutputMemoryStream::Write(const T& inData)
	{
		WriteInternal(&inData, sizeof(inData));
	}

	template<is_primitive_type T, size_t N>
	inline void OutputMemoryStream::Write(T(&inData)[N])
	{
		WriteInternal(&inData, sizeof(inData));
	}

}


