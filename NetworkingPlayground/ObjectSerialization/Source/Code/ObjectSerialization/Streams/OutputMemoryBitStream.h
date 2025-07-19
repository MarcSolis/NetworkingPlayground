#pragma once
#include "ObjectSerialization/Streams/StreamTypes.h"
#include "ObjectSerialization/ByteSwapper.h"

#include <cstring>
#include <memory>



namespace Serialization { namespace Stream {

	/// <summary>
	/// Fill current byte free bits, then create a tmp with the shifted InData and load it to the buffer.
	/// 
	/// PROS:
	///		If buffer is byte-aligned, no shifting needed.
	///		Compatibility and performance due to memory alignment.
	/// CONS: 
	///		Extra temp for ensuring memory accesses are aligned.
	/// </summary>
	class OutputMemoryBitStream
	{
		typedef unsigned char byte;

	public:
		OutputMemoryBitStream();
		~OutputMemoryBitStream();

		inline const byte* GetBufferPtr() const noexcept { return mBuffer; }
		inline uint32_t GetBitLength() const noexcept { return mBitHead; }


		template<is_primitive_type T, uint32_t InBitCount = sizeof(T) << 3>
		void Write(const T& inData);

	private:
		inline uint32_t GetNextFreeByte() const noexcept { return (mBitHead + 7) >> 3; }
		void ReallocBuffer(uint32_t inNewBitLength);

		byte WriteFreeBits(const byte* const inData, const byte inBitCount);
		byte FillFreeBitsLeft(const byte* const inData);	// WriteFreeBits specialization

		byte* mBuffer;
		uint32_t mBitHead;
		uint32_t mBitCapacity;

		static constexpr uint32_t InitialBufferBitSize{256 * 8};
		static constexpr uint8_t InDataMaxByteSize{8}; // Max type byte size supported
		static constexpr std::endian Endian{std::endian::little};
	};

	template<is_primitive_type T, uint32_t InBitCount>
	inline void OutputMemoryBitStream::Write(const T& inData)
	{
		static_assert(InBitCount <= (sizeof(inData) << 3), "More bits requested than type provides!");
		static_assert(sizeof(inData) <= InDataMaxByteSize, "Unsupported type, maximum type size exceeded");

		constexpr uint32_t InByteCount = (InBitCount + 7) >> 3;

		if constexpr (Endian == std::endian::native)
		{
			if (mBitHead & 0x7)	// byte-unaligned
			{
				if constexpr (InBitCount < 8) // Is less than a byte
				{
					const byte addedBits = WriteFreeBits(reinterpret_cast<const byte*>(&inData), InBitCount);
					auto shiftedData = inData >> addedBits;
					std::memcpy(mBuffer + GetNextFreeByte(), &shiftedData, InByteCount);
				}
				else
				{
					const byte addedBits = FillFreeBitsLeft(reinterpret_cast<const byte*>(&inData));
					auto shiftedData = inData >> addedBits;
					std::memcpy(mBuffer + GetNextFreeByte(), &shiftedData, InByteCount);
				}
			}
			else // byte-aligned
			{
				std::memcpy(mBuffer + GetNextFreeByte(), &inData, InByteCount);
			}
		}
		else
		{
			auto swappedData = Serialization::ByteSwap(inData);

			if constexpr (InBitCount < 8) // Is less than a byte
			{
				const byte addedBits = WriteFreeBits(reinterpret_cast<const byte*>(&swappedData), InBitCount);
				std::memcpy(mBuffer + GetNextFreeByte(), &(swappedData >>= addedBits), InByteCount);
			}
			else
			{
				const byte addedBits = FillFreeBitsLeft(reinterpret_cast<const byte*>(&swappedData));
				std::memcpy(mBuffer + GetNextFreeByte(), &(swappedData >>= addedBits), InByteCount);
			}
		}

		mBitHead += InBitCount;

		if (mBitHead > mBitCapacity) [[unlikely]]
		{
			ReallocBuffer(mBitCapacity * 2);
		}
	}
}}


