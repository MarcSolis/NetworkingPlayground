#pragma once
#include "StreamTypes.h"
#include <cstdint>

namespace Stream {

	class OutputMemoryStream
	{
	public:
		OutputMemoryStream();
		~OutputMemoryStream();

		const char* GetBufferPtr() const { return mBuffer; }
		uint32_t GetLength() const { return mHead; }

		template <is_primitive_type T>
		void Write(T inData);

		template <is_primitive_type T>
		void Write(T inData[], size_t size);

	private:
		void WriteInternal(const void* inData, size_t inByteCount);
		void ReallocBuffer(uint32_t inNewLength);

		char* mBuffer;
		uint32_t mHead;
		uint32_t mCapacity;
	};

	template<is_primitive_type T>
	inline void OutputMemoryStream::Write(T inData)
	{
		WriteInternal(&inData, sizeof(inData));
	}

	template<is_primitive_type T>
	inline void OutputMemoryStream::Write(T inData[], size_t size)
	{
		WriteInternal(inData, size);
	}

}


