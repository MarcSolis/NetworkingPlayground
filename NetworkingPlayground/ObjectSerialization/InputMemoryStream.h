#pragma once
#include "StreamTypes.h"
#include <cstdint>

namespace Stream {

	class InputMemoryStream
	{
	public:
		InputMemoryStream(const char* inBuffer, uint32_t inByteCount);

		uint32_t GetRemainingDataSize() const noexcept;

		template <is_primitive_type T>
		void Read(T& outData) noexcept;

		template<is_primitive_type T, size_t N>
		void Read(T (&outData)[N]) noexcept;

	private:
		void ReadInternal(void* outData, size_t inByteCount);

		const char* mbuffer;
		uint32_t mHead;
		uint32_t mCapacity;
	};

	template<is_primitive_type T>
	inline void InputMemoryStream::Read(T& outData) noexcept
	{
		ReadInternal(&outData, sizeof(outData));
	}

	template<is_primitive_type T, size_t N>
	inline void InputMemoryStream::Read(T (&outData)[N]) noexcept
	{
		ReadInternal(&outData, sizeof(outData));
	}
}


