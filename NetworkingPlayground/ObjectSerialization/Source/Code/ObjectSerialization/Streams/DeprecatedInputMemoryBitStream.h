#pragma once
#include "ObjectSerialization/Streams/StreamDefinitions.h"
#include "ObjectSerialization/ByteSwapper.h"

#include <cstdint>
#include <cassert>


namespace Serialization { namespace Stream {

	// For simplicity, this implementation assumes a non-owning reference to the buffer (No resource clean-up required)
	class DeprecatedInputMemoryBitStream
	{
	public:
		DeprecatedInputMemoryBitStream(const char* inBuffer, uint32_t inBitCount);

		inline const char* GetBufferPtr() const noexcept { return mBuffer; }
		inline uint32_t GetBitLength() const noexcept { return mBitHead; }
		inline uint32_t GetByteLength() const noexcept { return (mBitHead + 7) >> 3; }


		template <is_primitive_type T>
		void Read(T& outData, size_t outBitCount = sizeof(T) << 3);

		template <is_primitive_type T, size_t N>
		void Read(T(&inData)[N]);

	private:
		void ReadBitsInternal(uint8_t& inData, size_t inBitCount);
		void ReadBits(void* outData, size_t outBitCount);

		const char* mBuffer;
		uint32_t mBitHead;
		uint32_t mBitCapacity;
		static constexpr std::endian mEndian{std::endian::little};
	};

	template<is_primitive_type T>
	inline void DeprecatedInputMemoryBitStream::Read(T& outData, size_t outBitCount)
	{
		assert(outBitCount <= (sizeof(outData) << 3));

		ReadBits(&outData, outBitCount);

		if constexpr (std::endian::native != mEndian)
		{
			outData = Serialization::ByteSwap(outData);
		}
	}

	template<is_primitive_type T, size_t N>
	inline void DeprecatedInputMemoryBitStream::Read(T(&inData)[N])
	{
		if constexpr (std::endian::native == mEndian)
		{
			ReadBits(&inData, sizeof(inData) << 3);
		}
		else
		{
			T swappedData[N];
			for (size_t i = 0; i < N; ++i)
			{
				swappedData[i] = Serialization::ByteSwap(inData[i]);
			}

			ReadBits(&swappedData, sizeof(swappedData) << 3);
		}
	}
}}


