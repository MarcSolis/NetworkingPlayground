#pragma once
#include "ObjectSerialization/Streams/StreamTypes.h"
#include "ObjectSerialization/ByteSwapper.h"
#include "ObjectSerialization/Streams/Int2Type.h"

#include "Tools/ConstMath/ConstMath.h"

#include <cstdint>
#include <bit>
#include <cassert>
#include <algorithm>
#include <cstring>
#include <memory>


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

		static constexpr uint8_t InDataMaxByteSize = 4;
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

#pragma region V2.1
	/// <summary>
	/// Load the InData at the end of the buffer, then shifted if needed.
	/// PROS:
	///		This way we use a probably already cached line, and if not, we will load a line that we will need anyways.
	///		If buffer is byte-aligned, no shifting needed.
	/// CONS: 
	///		Platform dependent since this could try to load a value as uint64_t between to cache lines (unaligned memory support required).
	/// </summary>
	class OutputMemoryBitStream21
	{
	public:
		OutputMemoryBitStream21();
		~OutputMemoryBitStream21();

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

		static constexpr uint8_t InDataMaxByteSize = 4;
		static constexpr std::endian mEndian{std::endian::little};
	};

	//At least one full byte. Extra memory needed for sure
	inline void OutputMemoryBitStream21::WriteInternal(const uint32_t inBitCount, Int2Type<false>)	//Less than a byte = false
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
		mInData = reinterpret_cast<uint64_t*>(mBuffer + ((mBitHead + 7) >> 3));

		if (mBitHead > mBitCapacity)
		{
			ReallocBuffer(mBitCapacity * 2);
		}
	}

	inline void OutputMemoryBitStream21::WriteInternal(const uint32_t inBitCount, Int2Type<true>) //Less than a byte = true
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
				mInData = reinterpret_cast<uint64_t*>(mBuffer + ((mBitHead + 7) >> 3));
				return;	// No extra bytes needed
			}

			WriteFreeBits(bitOffset, freeBits);
		}

		*mInData >>= freeBits;	// Fix alignment
		mBitHead += inBitCount;
		mInData = reinterpret_cast<uint64_t*>(mBuffer + ((mBitHead + 7) >> 3));

		if (mBitHead > mBitCapacity)
		{
			ReallocBuffer(mBitCapacity * 2);
		}
	}

	template<is_primitive_type T, uint32_t InBitCount>
	inline void OutputMemoryBitStream21::Write(const T& inData)
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
#pragma endregion //v2.1


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

		static constexpr uint8_t InDataMaxByteSize = 4;
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

		static constexpr uint8_t InDataMaxByteSize = 8*2-1; // Max type size(8) + max alignment offset (7)
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

		size_t byteSpace = (static_cast<size_t>(mBitCapacity - mBitHead) >> 3) + InDataMaxByteSize;
		bool succeed = std::align(alignof(T), sizeof(T), targetPtr, byteSpace);

		assert(succeed);	// Allocator must take into account the extra space needed

		return targetPtr;
	}
#pragma endregion //v4

#pragma region V5
	/// <summary>
	/// Load the InData at the end of the buffer (aligned), then shifted if needed.
	/// PROS:
	///		This way we use a probably already cached line, and if not, we will load a line that we will need anyways.
	///		If buffer is byte-aligned, no shifting needed.
	///		Compatibility and performance boosts due to memory alignment.
	/// CONS: 
	///		Extra calculus for ensuring memory accesses are aligned.
	/// </summary>
	class OutputMemoryBitStream5
	{
		typedef unsigned char byte;
	public:
		OutputMemoryBitStream5();
		~OutputMemoryBitStream5();

		inline const char* GetBufferPtr() const noexcept { return mBuffer; }
		inline uint32_t GetBitLength() const noexcept { return mBitHead; }


		template<is_primitive_type T, uint32_t InBitCount = sizeof(T) << 3>
		void Write(T inData);

	private:
		inline uint32_t GetNextFreeByte() const noexcept { return (mBitHead + 7) >> 3; }
		void ReallocBuffer(uint32_t inNewBitLength);


		byte FillFreeBits(byte*const inData);	// Byte or more
		byte WriteFreeBits(byte*const inData, const byte inBitCount);	// Less than a byte

		inline void WriteBytes(byte&& inData, byte&& inBitCount);

		void WriteInternal(void* alignedDataIn, const uint32_t inBitCount, Int2Type<false>);	// Byte or more
		void WriteInternal(void* alignedDataIn, const uint32_t inBitCount, Int2Type<true>);	// Less than a byte

		//void WriteFreeBits(const uint8_t bitOffset, const uint32_t inBitCount);
		template <typename T>
		void* GetNextAlignedByte() const noexcept;

		char* mBuffer;
		uint32_t mBitHead;
		uint32_t mBitCapacity;

		static constexpr uint8_t InDataMaxByteSize = 8 * 2 - 1; // Max type size(8) + max alignment offset (7)
		static constexpr std::endian mEndian{std::endian::little};
	};

	//At least one full byte. Extra memory needed for sure
	inline void OutputMemoryBitStream5::WriteInternal(void* alignedDataIn, const uint32_t inBitCount, Int2Type<false>)	//Less than a byte = false
	{
		// Align to byte
		const uint8_t bitOffset = mBitHead & 0x7;
		const uint8_t freeBits{uint8_t(8 - bitOffset)};
		//WriteFreeBits(bitOffset, freeBits);

		uintptr_t dataIn = reinterpret_cast<uintptr_t>(alignedDataIn);
		dataIn >>= freeBits;	// remove duplicated data
		std::memmove(&mBuffer[mBitHead >> 3], alignedDataIn, static_cast<size_t>(inBitCount + 7) >> 3);
		mBitHead += inBitCount;

		if (mBitHead > mBitCapacity)
		{
			ReallocBuffer(mBitCapacity * 2);
		}
	}

	inline void OutputMemoryBitStream5::WriteInternal(void* alignedDataIn, const uint32_t inBitCount, Int2Type<true>) //Less than a byte = true
	{
		// Align to byte
		const uint8_t bitOffset = mBitHead & 0x7;
		const uint8_t freeBits{uint8_t(8 - bitOffset)};

		if (inBitCount <= freeBits)
		{
			//WriteFreeBits(bitOffset, inBitCount);
			mBitHead += inBitCount;
			return;	// No extra bytes needed
		}

		//WriteFreeBits(bitOffset, freeBits);

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
	inline void OutputMemoryBitStream5::Write(T inData)
	{
		static_assert(InBitCount <= (sizeof(inData) << 3));

		constexpr uint32_t InByteCount = (InBitCount + 7) >> 3;

		if constexpr (std::endian::native == mEndian)
		{
			if constexpr (InBitCount < 8) // Is less than a byte
			{
				const byte addedBits = WriteFreeBits(reinterpret_cast<byte*>(&inData), InBitCount);

				std::memcpy(mBuffer + GetNextFreeByte(), &(*reinterpret_cast<byte*>(&inData) >>= addedBits), InByteCount);
				mBitHead += InBitCount;
			}
			else
			{
				const byte addedBits = FillFreeBits(reinterpret_cast<byte*>(&inData));

				std::memcpy(mBuffer + GetNextFreeByte(), &(*reinterpret_cast<byte*>(&inData) >>= addedBits), InByteCount);
				mBitHead += InBitCount;
			}
		}
		else
		{
			inData = Serialization::ByteSwap(inData);

			if constexpr (InBitCount < 8) // Is less than a byte
			{
				const byte addedBits = WriteFreeBits(reinterpret_cast<byte*>(&inData), InBitCount);

				std::memcpy(mBuffer + GetNextFreeByte(), &(*reinterpret_cast<byte*>(&inData) >>= addedBits), InByteCount);
				mBitHead += InBitCount;
			}
			else
			{
				const byte addedBits = FillFreeBits(reinterpret_cast<byte*>(&inData));

				std::memcpy(mBuffer + GetNextFreeByte(), &(*reinterpret_cast<byte*>(&inData) >>= addedBits), InByteCount);
				mBitHead += InBitCount;
			}
		}

		if (mBitHead > mBitCapacity) [[unlikely]]
		{
			ReallocBuffer(mBitCapacity * 2);
		}
	}

	void OutputMemoryBitStream5::WriteBytes(byte&& inData, byte&& inBitCount)
	{
		std::memcpy(mBuffer + GetNextFreeByte(), &inData, inBitCount);
		mBitHead += inBitCount;
	}

	template<typename T>
	inline void* OutputMemoryBitStream5::GetNextAlignedByte() const noexcept
	{
		constexpr uint8_t bitsToShift = ConstMath::log2<alignof(T)>::value;	// narrowing cast valid, since max alignment is 8|4 on a x64|x32 platform
		const size_t nextAlignedByte{(GetNextFreeByte() + alignof(T) - 1) >> bitsToShift};

		return mBuffer + nextAlignedByte;
	}
#pragma endregion //v5

#pragma region V6
	/// <summary>
	/// Load the InData at the end of the buffer, then shifted if needed.
	/// PROS:
	///		This way we use a probably already cached line, and if not, we will load a line that we will need anyways.
	///		If buffer is byte-aligned, no shifting needed.
	/// CONS: 
	///		Platform dependent since this could try to load a value as uint64_t between to cache lines (unaligned memory support required).
	/// </summary>
	class OutputMemoryBitStream6
	{
		using byte = unsigned char;

	public:
		OutputMemoryBitStream6();
		~OutputMemoryBitStream6();

		[[nodiscard]] inline const byte* GetBufferPtr() const noexcept { return mBuffer; }
		[[nodiscard]] inline uint32_t GetBitLength() const noexcept { return mBitHead; }
		[[nodiscard]] inline uint32_t GetByteLength() const noexcept { return (mBitHead + 7) >> 3; }
		//inline uint8_t GetReallocsCount() const noexcept { return reallocs; }

		template<is_primitive_type T, uint32_t InBitCount = sizeof(T) << 3>
		void Write(const T& inData);

	private:
		void ReallocBuffer(const uint32_t inNewBitLength);

		void WriteInternal(const uint32_t inBitCount, Int2Type<false>);	// Byte or more
		void WriteInternal(const uint32_t inBitCount, Int2Type<true>);	// Less than a byte

		void WriteFreeBits(const uint8_t bitOffset, const uint32_t inBitCount);

		byte* mBuffer;
		uint64_t* mInData;
		uint32_t mBitHead;
		uint32_t mBitCapacity;
		uint8_t reallocs{0};

		static constexpr uint8_t InDataMaxByteSize = 8;
		static constexpr std::endian mEndian{std::endian::little};
	};

	//At least one full byte. Extra memory needed for sure
	inline void OutputMemoryBitStream6::WriteInternal(const uint32_t inBitCount, Int2Type<false>)	//Less than a byte = false
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
		mInData = reinterpret_cast<uint64_t*>(mBuffer + ((mBitHead + 7) >> 3));
	}

	inline void OutputMemoryBitStream6::WriteInternal(const uint32_t inBitCount, Int2Type<true>) //Less than a byte = true
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
				mInData = reinterpret_cast<uint64_t*>(mBuffer + ((mBitHead + 7) >> 3));
				return;	// No extra bytes needed
			}

			WriteFreeBits(bitOffset, freeBits);
		}

		*mInData >>= freeBits;	// Fix alignment
		mBitHead += inBitCount;
		mInData = reinterpret_cast<uint64_t*>(mBuffer + ((mBitHead + 7) >> 3));
	}

	template<is_primitive_type T, uint32_t InBitCount>
	inline void OutputMemoryBitStream6::Write(const T& inData)
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

		if (mBitHead > mBitCapacity) [[unlikely]]	// safe for primitive types, since mBuffer has an extra 8 bytes passed mBitCapacity
		{
			ReallocBuffer(mBitCapacity * 2);
		}
	}
#pragma endregion //v6

#pragma region V6.1
	//Defines a valid type to operate with raw data of size N
	template <uint8_t N>
	struct HeadAlignment;

	template <>
	struct HeadAlignment<1>
	{
		using type = uint8_t;
	};

	template <>
	struct HeadAlignment<2>
	{
		using type = uint16_t;
	};

	template <>
	struct HeadAlignment<4>
	{
		using type = uint32_t;
	};

	template <>
	struct HeadAlignment<8>
	{
		using type = uint64_t;
	};


	/// <summary>
	/// Load the InData at the end of the buffer, then shifted if needed.
	/// PROS:
	///		This way we use a probably already cached line, and if not, we will load a line that we will need anyways.
	///		If buffer is byte-aligned, no shifting needed.
	/// CONS: 
	///		Platform dependent since this could try to load a value as uint64_t between to cache lines (unaligned memory support required).
	/// </summary>
	class OutputMemoryBitStream61
	{
		using byte = unsigned char;

	public:
		OutputMemoryBitStream61();
		~OutputMemoryBitStream61();

		[[nodiscard]] inline const byte* GetBufferPtr() const noexcept { return mBuffer; }
		[[nodiscard]] inline uint32_t GetBitLength() const noexcept { return mBitHead; }

		template<is_primitive_type T, uint32_t InBitCount = sizeof(T) << 3>
		void Write(const T& inData);

	private:
		[[nodiscard]] inline void* GetNextFreeByte() const noexcept { return mBuffer + ((mBitHead + 7) >> 3); }
		// Returns the minimum type size for containing the inserted data
		static constexpr uint8_t GetMinTypeSize(const uint16_t inBytes) noexcept;

		void ReallocBuffer(const uint32_t inNewBitLength);

		template <is_unsigned_arithmetic_type T>
		void BitAlignment(const uint32_t inBitCount, Int2Type<false>);
		template <is_unsigned_arithmetic_type T>
		void BitAlignment(const uint32_t inBitCount, Int2Type<true>);


		void WriteInternal(const uint32_t inBitCount, Int2Type<false>);	// Byte or more
		void WriteInternal(const uint32_t inBitCount, Int2Type<true>);	// Less than a byte

		void WriteFreeBits(const uint8_t bitOffset, const uint32_t inBitCount);

		byte* mBuffer;
		uint64_t* mInData;
		uint32_t mBitHead;
		uint32_t mBitCapacity;
		uint8_t reallocs{0};

		static constexpr uint8_t InDataMaxByteSize = 8;
		static constexpr std::endian mEndian{std::endian::little};
	};

	//At least one full byte. Extra memory needed for sure
	inline void OutputMemoryBitStream61::WriteInternal(const uint32_t inBitCount, Int2Type<false>)	//Less than a byte = false
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
		mInData = reinterpret_cast<uint64_t*>(mBuffer + ((mBitHead + 7) >> 3));
	}

	inline void OutputMemoryBitStream61::WriteInternal(const uint32_t inBitCount, Int2Type<true>) //Less than a byte = true
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
				mInData = reinterpret_cast<uint64_t*>(mBuffer + ((mBitHead + 7) >> 3));
				return;	// No extra bytes needed
			}

			WriteFreeBits(bitOffset, freeBits);
		}

		*mInData >>= freeBits;	// Fix alignment
		mBitHead += inBitCount;
		mInData = reinterpret_cast<uint64_t*>(mBuffer + ((mBitHead + 7) >> 3));
	}

	template<is_primitive_type T, uint32_t InBitCount>
	inline void OutputMemoryBitStream61::Write(const T& inData)
	{
		static_assert(InBitCount <= (sizeof(inData) << 3));

		constexpr bool isLessThanByte{InBitCount < 8};
		constexpr uint16_t byteSize = (InBitCount + 7) >> 3;

		if constexpr (std::endian::native == mEndian)
		{
			std::memcpy(GetNextFreeByte(), &inData, byteSize);
			BitAlignment<typename HeadAlignment<GetMinTypeSize(byteSize)>::type>(InBitCount, Int2Type<isLessThanByte>());
		}
		else
		{
			std::memcpy(GetNextFreeByte(), Serialization::ByteSwap(inData), byteSize);
			BitAlignment<typename HeadAlignment<GetMinTypeSize(byteSize)>::type>(InBitCount, Int2Type<isLessThanByte>());
		}

		if (mBitHead > mBitCapacity) [[unlikely]]	// safe for primitive types, since mBuffer has an extra 8 bytes passed mBitCapacity
		{
			ReallocBuffer(mBitCapacity * 2);
		}
	}

	template<is_unsigned_arithmetic_type T> 
	inline void OutputMemoryBitStream61::BitAlignment(const uint32_t inBitCount, Int2Type<false>) //At least one full byte. Extra memory needed for sure
	{
		const uint8_t bitOffset = mBitHead & 0x7;
		uint8_t freeBits{0};
		if (bitOffset > 0)
		{
			// Align to byte
			freeBits = 8 - bitOffset;
			WriteFreeBits(bitOffset, freeBits);
		}

		*reinterpret_cast<T*>(GetNextFreeByte()) >>= freeBits;	// Fix alignment
		mBitHead += inBitCount;
	}

	template<is_unsigned_arithmetic_type T>
	inline void OutputMemoryBitStream61::BitAlignment(const uint32_t inBitCount, Int2Type<true>) //Less than a byte
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
				*reinterpret_cast<T*>(GetNextFreeByte()) >>= freeBits;	// Fix alignment
				return;	// No extra bytes needed
			}

			WriteFreeBits(bitOffset, freeBits);
		}

		*reinterpret_cast<T*>(GetNextFreeByte()) >>= freeBits;	// Fix alignment
		mBitHead += inBitCount;
	}

	constexpr uint8_t OutputMemoryBitStream61::GetMinTypeSize(const uint16_t inBytes) noexcept
	{
		if (inBytes == 1)
		{
			return 1;
		}
		else if (inBytes == 2)
		{
			return 2;
		}
		else if (inBytes <= 4)
		{
			return 4;
		}
		else
		{
			return 8;
		}
	}
#pragma endregion //v6.1

#pragma region V6.2
	/// <summary>
	/// Load the InData at the end of the buffer, then shifted if needed.
	/// PROS:
	///		This way we use a probably already cached line, and if not, we will load a line that we will need anyways.
	///		If buffer is byte-aligned, no shifting needed.
	/// CONS: 
	///		Platform dependent since this could try to load a value as uint64_t between to cache lines (unaligned memory support required).
	/// </summary>
	class OutputMemoryBitStream62
	{
		using byte = unsigned char;

	public:
		OutputMemoryBitStream62();
		~OutputMemoryBitStream62();

		[[nodiscard]] inline const byte* GetBufferPtr() const noexcept { return mBuffer; }
		[[nodiscard]] inline uint32_t GetBitLength() const noexcept { return mBitHead; }

		template<is_primitive_type T, uint32_t InBitCount = sizeof(T) << 3>
		void Write(const T& inData);

	private:
		[[nodiscard]] inline void* GetNextFreeByte() const noexcept { return mBuffer + ((mBitHead + 7) >> 3); }
		// Returns the minimum type size for containing the inserted data
		static constexpr uint8_t GetMinTypeSize(const uint16_t inBytes) noexcept;

		void ReallocBuffer(const uint32_t inNewBitLength);

		template <is_unsigned_arithmetic_type T>
		void BitAlignment();

		void WriteFreeBits(const uint8_t occupiedBits);

		byte* mBuffer;
		uint32_t mBitHead;
		uint32_t mBitCapacity;

		static constexpr uint8_t InDataMaxByteSize = 8;
		static constexpr std::endian mEndian{std::endian::little};
	};


	template<is_primitive_type T, uint32_t InBitCount>
	inline void OutputMemoryBitStream62::Write(const T& inData)
	{
		static_assert(InBitCount <= (sizeof(inData) << 3));

		constexpr uint16_t byteSize = (InBitCount + 7) >> 3;

		if constexpr (std::endian::native == mEndian)
		{
			std::memcpy(GetNextFreeByte(), &inData, byteSize);
		}
		else
		{
			auto invertedData = Serialization::ByteSwap(inData);
			std::memcpy(GetNextFreeByte(), &invertedData, byteSize);
		}

		BitAlignment<typename HeadAlignment<GetMinTypeSize(byteSize)>::type>();
		mBitHead += InBitCount;

		if (mBitHead > mBitCapacity) [[unlikely]]	// safe for primitive types, since mBuffer has an extra 8 bytes passed mBitCapacity
		{
			ReallocBuffer(mBitCapacity * 2);
		}
	}

	template<is_unsigned_arithmetic_type T>
	inline void OutputMemoryBitStream62::BitAlignment()
	{
		const uint8_t occupiedBits = mBitHead & 0x7;
		if (occupiedBits > 0)
		{
			// Align to byte
			WriteFreeBits(occupiedBits);
			const uint8_t freeBits = 8 - occupiedBits;
			*reinterpret_cast<T*>(GetNextFreeByte()) >>= freeBits;	// Fix alignment
		}
	}

	constexpr uint8_t OutputMemoryBitStream62::GetMinTypeSize(const uint16_t inBytes) noexcept
	{
		if (inBytes == 1)
		{
			return 1;
		}
		else if (inBytes == 2)
		{
			return 2;
		}
		else if (inBytes <= 4)
		{
			return 4;
		}
		else
		{
			return 8;
		}
	}
#pragma endregion //v6.2
}}


