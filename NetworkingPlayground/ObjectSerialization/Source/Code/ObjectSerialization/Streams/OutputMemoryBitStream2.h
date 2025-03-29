#pragma once
#include "ObjectSerialization/Streams/StreamTypes.h"
#include "ObjectSerialization/ByteSwapper.h"
#include "ObjectSerialization/Streams/Int2Type.h"
#include <cstdint>
#include <bit>
#include <cassert>
#include <algorithm>


namespace Serialization { namespace Stream {

	class OutputMemoryBitStream2
	{
	public:
		OutputMemoryBitStream2();
		~OutputMemoryBitStream2();

		inline const char* GetBufferPtr() const noexcept { return mBuffer; }
		inline uint32_t GetBitLength() const noexcept { return mBitHead; }
		inline uint32_t GetByteLength() const noexcept { return (mBitHead + 7) >> 3; }


		template<is_primitive_type T, uint32_t InBitCount = sizeof(T) << 3>
		void Write(const T& inData);

	private:
		void ReallocBuffer(uint32_t inNewBitLength);
		
		void WriteInternal(const uint32_t inBitCount, Int2Type<false>);	// Byte or more
		void WriteInternal(const uint32_t inBitCount, Int2Type<true>);	// Less than a byte

		void WriteFreeBits(const uint8_t bitOffset, const uint32_t inBitCount);

		char* mBuffer;
		uint64_t* mInData;
		uint32_t mBitHead;
		uint32_t mBitCapacity;

		static constexpr uint8_t InDataMaxSize = 4;
		static constexpr std::endian mEndian{std::endian::little};
	};

	//At least one full byte. Extra memory needed for sure
	inline void OutputMemoryBitStream2::WriteInternal(const uint32_t inBitCount, Int2Type<false>)	//Less than a byte = false
	{
		const uint8_t bitOffset = mBitHead & 0x7;
		uint8_t freeBits{0};
		if (bitOffset > 0)
		{
			// Align to byte
			freeBits = 8 - bitOffset;
			WriteFreeBits(bitOffset, freeBits);
		}

		*mInData >>= freeBits;	// Fix alignment
		mBitHead += inBitCount;

		if (mBitHead > mBitCapacity)
		{
			ReallocBuffer(mBitCapacity * 2);
		}
	}

	inline void OutputMemoryBitStream2::WriteInternal(const uint32_t inBitCount, Int2Type<true>) //Less than a byte = true
	{
		const uint8_t bitOffset = mBitHead & 0x7;
		uint8_t freeBits{0};
		if (bitOffset > 0)
		{
			// Align to byte
			freeBits = 8 - bitOffset;
			if (inBitCount <= freeBits)
			{
				WriteFreeBits(bitOffset, inBitCount);
				mBitHead += inBitCount;
				return;	// No extra bytes needed
			}

			WriteFreeBits(bitOffset, freeBits);
		}

		*mInData >>= freeBits;	// Fix alignment
		mBitHead += inBitCount;

		if (mBitHead > mBitCapacity)
		{
			ReallocBuffer(mBitCapacity * 2);
		}
	}

	template<is_primitive_type T, uint32_t InBitCount>
	inline void OutputMemoryBitStream2::Write(const T& inData)
	{
		static_assert(InBitCount <= (sizeof(inData) << 3));

		constexpr bool isLessThanByte{InBitCount < 8};

		if constexpr (std::endian::native == mEndian)
		{
			std::memcpy(mInData, &inData, (InBitCount + 7) >> 3);
			WriteInternal(InBitCount, Int2Type<isLessThanByte>());
		}
		else
		{
			std::memcpy(mInData, Serialization::ByteSwap(inData), (InBitCount + 7) >> 3);
			WriteInternal(InBitCount, Int2Type<isLessThanByte>());
		}
	}

}}


