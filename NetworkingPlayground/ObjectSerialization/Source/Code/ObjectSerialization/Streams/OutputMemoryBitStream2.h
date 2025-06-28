#pragma once
#include "ObjectSerialization/Streams/StreamTypes.h"
#include "ObjectSerialization/ByteSwapper.h"
#include "ObjectSerialization/Streams/Int2Type.h"
#include <cstdint>
#include <bit>
#include <cassert>
#include <algorithm>


namespace Serialization { namespace Stream {

#pragma region V2
	/// <summary>
	/// Load the InData at the end of the buffer, then shifted if needed.
	/// PROS:
	///		This way we use a probably already cached line, and if not, we will load a line that we will need anyways.
	///		If buffer is byte-aligned, no shifting needed.
	/// CONS: 
	///		Platform dependent since this could try to load a value as uint64_t between to cache lines (unaligned memory support required).
	/// </summary>
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
#pragma endregion //v2


#pragma region V3
	/// <summary>
	/// Loads the InData on a private variable first, then splits the bits as needed for storing it into the buffer.
	/// PROS:
	///		Not platform dependent.
	///		Loading always in the same memory addresses prevent cache misses.
	/// CONS:
	///		It requires copying again into the buffer even if buffer is byte-aligned.
	/// </summary>
	class OutputMemoryBitStream3
	{
	public:
		OutputMemoryBitStream3();
		~OutputMemoryBitStream3();

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
		uint64_t mInData;
		uint32_t mBitHead;
		uint32_t mBitCapacity;

		static constexpr uint8_t InDataMaxSize = 4;
		static constexpr std::endian mEndian{std::endian::little};
	};

	//At least one full byte. Extra memory needed for sure
	inline void OutputMemoryBitStream3::WriteInternal(const uint32_t inBitCount, Int2Type<false>)	//Less than a byte = false
	{
		const uint8_t bitOffset = mBitHead & 0x7;
		uint8_t freeBits{0};
		if (bitOffset > 0)
		{
			// Align to byte
			freeBits = 8 - bitOffset;
			WriteFreeBits(bitOffset, freeBits);
		}

		mInData >>= freeBits;	// Fix alignment
		auto extraBytes = (inBitCount - freeBits + 7) >> 3;
		std::memcpy(mBuffer + (mBitHead >> 3) + 1, &mInData, extraBytes);
		mBitHead += inBitCount;

		if (mBitHead > mBitCapacity - 8*8)
		{
			ReallocBuffer(mBitCapacity * 2);
		}
	}

	inline void OutputMemoryBitStream3::WriteInternal(const uint32_t inBitCount, Int2Type<true>) //Less than a byte = true
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

		mInData >>= freeBits;	// Fix alignment
		auto extraBytes = (inBitCount - freeBits + 7) >> 3;
		std::memcpy(mBuffer + (mBitHead >> 3) + 1, &mInData, extraBytes);
		mBitHead += inBitCount;

		if (mBitHead > mBitCapacity - 8*8)
		{
			ReallocBuffer(mBitCapacity * 2);
		}
	}

	template<is_primitive_type T, uint32_t InBitCount>
	inline void OutputMemoryBitStream3::Write(const T& inData)
	{
		static_assert(InBitCount <= (sizeof(inData) << 3));

		constexpr bool isLessThanByte{InBitCount < 8};

		if constexpr (std::endian::native == mEndian)
		{
			std::memcpy(&mInData, &inData, (InBitCount + 7) >> 3);
			WriteInternal(InBitCount, Int2Type<isLessThanByte>());
		}
		else
		{
			std::memcpy(&mInData, Serialization::ByteSwap(inData), (InBitCount + 7) >> 3);
			WriteInternal(InBitCount, Int2Type<isLessThanByte>());
		}
	}
#pragma endregion //v3

#pragma region V4
	/// <summary>
	/// Load the InData at the end of the buffer (aligned), then shifted if needed.
	/// PROS:
	///		This way we use a probably already cached line, and if not, we will load a line that we will need anyways.
	///		If buffer is byte-aligned, no shifting needed.
	///		Compatibility and performance boosts due to memory alignment.
	/// CONS: 
	///		Extra calculus for ensuring memory accesses are aligned.
	/// </summary>
	class OutputMemoryBitStream4
	{
	public:
		OutputMemoryBitStream4();
		~OutputMemoryBitStream4();

		inline const char* GetBufferPtr() const noexcept { return mBuffer; }
		inline uint32_t GetBitLength() const noexcept { return mBitHead; }
		inline uint32_t GetByteLength() const noexcept { return (mBitHead + 7) >> 3; }


		template<is_primitive_type T, uint32_t InBitCount = sizeof(T) << 3>
		void Write(const T& inData);

	private:
		void ReallocBuffer(uint32_t inNewBitLength);

		void WriteInternal(void* alignedDataIn, const uint32_t inBitCount, Int2Type<false>);	// Byte or more
		void WriteInternal(void* alignedDataIn, const uint32_t inBitCount, Int2Type<true>);	// Less than a byte

		void WriteFreeBits(const uint8_t bitOffset, const uint32_t inBitCount);
		template <typename T>
		void* GetNextAlignedByte() const noexcept;

		char* mBuffer;
		uint32_t mBitHead;
		uint32_t mBitCapacity;

		static constexpr uint8_t InDataMaxSize = 8*2-1; // Max type size(8) + max alignment offset (7)
		static constexpr std::endian mEndian{std::endian::little};
	};

	//At least one full byte. Extra memory needed for sure
	inline void OutputMemoryBitStream4::WriteInternal(void* alignedDataIn, const uint32_t inBitCount, Int2Type<false>)	//Less than a byte = false
	{
		const uint8_t bitOffset = mBitHead & 0x7;
		uint8_t freeBits{0};
		if (bitOffset > 0)
		{
			// Align to byte
			freeBits = 8 - bitOffset;
			WriteFreeBits(bitOffset, freeBits);
		}

		uintptr_t dataIn = reinterpret_cast<uintptr_t>(alignedDataIn);
		dataIn >>= freeBits;	// remove duplicated data
		std::memmove(&mBuffer[mBitHead >> 3], alignedDataIn, static_cast<size_t>(inBitCount + 7) >> 3);
		mBitHead += inBitCount;
		
		if (mBitHead > mBitCapacity)
		{
			ReallocBuffer(mBitCapacity * 2);
		}
	}

	inline void OutputMemoryBitStream4::WriteInternal(void* alignedDataIn, const uint32_t inBitCount, Int2Type<true>) //Less than a byte = true
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

		uintptr_t dataIn = reinterpret_cast<uintptr_t>(alignedDataIn);
		dataIn >>= freeBits;	// remove duplicated data
		std::memmove(&mBuffer[mBitHead >> 3], alignedDataIn, static_cast<size_t>(inBitCount + 7) >> 3);
		mBitHead += inBitCount;

		if (mBitHead > mBitCapacity)
		{
			ReallocBuffer(mBitCapacity * 2);
		}
	}

	template<is_primitive_type T, uint32_t InBitCount>
	inline void OutputMemoryBitStream4::Write(const T& inData)
	{
		static_assert(InBitCount <= (sizeof(inData) << 3));

		constexpr bool isLessThanByte{InBitCount < 8};

		if constexpr (std::endian::native == mEndian)
		{
			auto alignedData = std::memcpy(GetNextAlignedByte<T>(), &inData, (InBitCount + 7) >> 3);
			WriteInternal(alignedData, InBitCount, Int2Type<isLessThanByte>());
		}
		else
		{
			auto alignedData = std::memcpy(GetNextAlignedByte<T>(), Serialization::ByteSwap(inData), (InBitCount + 7) >> 3);
			WriteInternal(alignedData, InBitCount, Int2Type<isLessThanByte>());
		}
	}

	template<typename T>
	inline void* OutputMemoryBitStream4::GetNextAlignedByte() const noexcept
	{
		void* targetPtr = mBuffer + GetByteLength();

		size_t byteSpace = static_cast<size_t>(mBitCapacity - mBitHead) >> 3;
		bool succeed = std::align(alignof(T), sizeof(T), targetPtr, byteSpace);

		assert(succeed);	// Allocator must take into account the extra space needed

		return targetPtr;
	}
#pragma endregion //v4
}}


