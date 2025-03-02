#pragma once
#include "ObjectSerialization/Streams/StreamTypes.h"
#include "ObjectSerialization/ByteSwapper.h"
#include <cstdint>
#include <bit>
#include <cassert>


namespace Serialization { namespace Stream {

	class OutputMemoryBitStream
	{
	public:
		OutputMemoryBitStream();
		~OutputMemoryBitStream();

		const char* GetBufferPtr() const { return mBuffer; }
		uint32_t GetBitLength() const noexcept { return mBitHead; }
		uint32_t GetByteLength() const noexcept { return (mBitHead + 7) >> 3; }


		template <is_primitive_type T>
		void Write(T inData, size_t inBitCount = sizeof(T) << 3);

		template <is_primitive_type T>
		void WriteAlt(T inData, size_t inBitCount = sizeof(T) << 3);

		template <is_primitive_type T, size_t N>
		void Write(T(&inData)[N]);

	private:
		void WriteBitsInternal(uint8_t inData, size_t inBitCount);
		void WriteBits(const void* inData, size_t inBitCount);
		void ReallocBuffer(uint32_t inNewBitLength);


		void WriteBitsAlt(const void* inData, size_t inBitCount);
		void WriteBytesInternal(const void* inData, size_t inBitCount);

		char* mBuffer;
		uint32_t mBitHead;
		uint32_t mBitCapacity;
		static constexpr std::endian mEndian{std::endian::little};
	};

	template<is_primitive_type T>
	inline void OutputMemoryBitStream::Write(T inData, size_t inBitCount)
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
	inline void OutputMemoryBitStream::Write(T(&inData)[N])
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

	template<is_primitive_type T>
	inline void OutputMemoryBitStream::WriteAlt(T inData, size_t inBitCount)
	{
		assert(inBitCount <= (sizeof(inData) << 3));

		uint8_t bitOffset = mBitHead & 0x7;
		uint8_t freeBits{0};
		if (bitOffset > 0)
		{
			// Align to byte
			freeBits = 8 - bitOffset;
			WriteBitsInternal(static_cast<uint8_t>(inData), freeBits);
			inBitCount -= freeBits;
		}

		inData = inData >> freeBits;

		// Load aligned
		WriteBytesInternal(&inData, inBitCount);
	}

}}


