#pragma once
#include "ObjectSerialization/Streams/StreamTypes.h"
#include "ObjectSerialization/ByteSwapper.h"
#include <cstdint>
#include <bit>


namespace Serialization { namespace Stream {

	class InputMemoryStream
	{
	public:
		InputMemoryStream(const char* inBuffer, uint32_t inByteCount);

		uint32_t GetRemainingDataSize() const noexcept;

		template <is_primitive_type T>
		void Read(T& outData) noexcept;

		template<is_primitive_type T, size_t N>
		void Read(T(&outData)[N]) noexcept;

	private:
		void ReadInternal(void* outData, size_t inByteCount);

		const char* mbuffer;
		uint32_t mHead;
		uint32_t mCapacity;
		static constexpr std::endian mEndian{std::endian::little};
	};

	template<is_primitive_type T>
	inline void InputMemoryStream::Read(T& outData) noexcept
	{
		ReadInternal(&outData, sizeof(outData));
		
		if constexpr (std::endian::native != mEndian)
		{
			outData = Serialization::ByteSwap(outData);
		}		
	}

	template<is_primitive_type T, size_t N>
	inline void InputMemoryStream::Read(T(&outData)[N]) noexcept
	{
		ReadInternal(&outData, sizeof(outData));

		if constexpr (std::endian::native != mEndian)
		{
			for (auto& element : outData)
			{
				element = Serialization::ByteSwap(element);
			}
		}
	}
}}

