#pragma once
#include "ObjectSerialization/Streams/StreamTypes.h"
#include "ObjectSerialization/ByteSwapper.h"
#include "ObjectSerialization/Streams/Int2Type.h"

#include <cstdint>
#include <bit>
#include <cassert>
#include <algorithm>


namespace Serialization { namespace Stream {

	class DeprecatedOutputMemoryBitStream
	{
	public:
		DeprecatedOutputMemoryBitStream();
		~DeprecatedOutputMemoryBitStream();

		inline const char* GetBufferPtr() const noexcept { return mBuffer; }
		inline uint32_t GetBitLength() const noexcept { return mBitHead; }
		inline uint32_t GetByteLength() const noexcept { return (mBitHead + 7) >> 3; }


		template <is_primitive_type T>
		void Write(T inData, size_t inBitCount = sizeof(T) << 3);

		template <is_primitive_type T, size_t N>
		void Write(T(&inData)[N]);

	private:
		void WriteBitsInternal(uint8_t inData, size_t inBitCount);
		void WriteBits(const void* inData, size_t inBitCount);
		void ReallocBuffer(uint32_t inNewBitLength);

		void WriteFreeBits(uint8_t inData, size_t inBitCount);
		void WriteBytesInternal(const void* inData, size_t inBitCount);

		char* mBuffer;
		uint32_t mBitHead;
		uint32_t mBitCapacity;
		static constexpr std::endian mEndian{std::endian::little};
	};

	template<is_primitive_type T>
	inline void DeprecatedOutputMemoryBitStream::Write(T inData, size_t inBitCount)
	{
		assert(inBitCount <= (sizeof(inData) << 3));

		if constexpr (std::endian::native == mEndian)
		{
			WriteBits(&inData, inBitCount);
		}
		else
		{
			T swappedData{Serialization::ByteSwap(inData)};
			WriteBits(&swappedData, inBitCount);
		}
	}

	template<is_primitive_type T, size_t N>
	inline void DeprecatedOutputMemoryBitStream::Write(T(&inData)[N])
	{
		if constexpr (std::endian::native == mEndian)
		{
			WriteBits(&inData, sizeof(inData) << 3);
		}
		else
		{
			T swappedData[N];
			for (size_t i = 0; i < N; ++i)
			{
				swappedData[i] = Serialization::ByteSwap(inData[i]);
			}

			WriteBits(&swappedData, sizeof(swappedData) << 3);
		}
	}
}}


