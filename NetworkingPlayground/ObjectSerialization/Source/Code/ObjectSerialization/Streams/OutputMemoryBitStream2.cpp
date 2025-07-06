#include "ObjectSerialization/Streams/OutputMemoryBitStream2.h"

#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <cassert>


namespace Serialization { namespace Stream {

#pragma region V2
	OutputMemoryBitStream2::OutputMemoryBitStream2() : mBuffer(nullptr), mBitHead(0), mBitCapacity(0)
	{
		ReallocBuffer(256);
	}

	OutputMemoryBitStream2::~OutputMemoryBitStream2()
	{
		std::free(mBuffer);
	}

	void OutputMemoryBitStream2::ReallocBuffer(uint32_t inNewBitLength)
	{
		const uint32_t newByteSize = (inNewBitLength + 7) >> 3;
		if (auto reallocated = std::realloc(mBuffer, static_cast<size_t>(newByteSize) + InDataMaxByteSize))
		{
			mBuffer = static_cast<char*>(reallocated);
			mBitCapacity = newByteSize << 3;

			mInData = reinterpret_cast<uint64_t*>(mBuffer + ((mBitHead + 7 )>> 3));
		}

		//handle realloc failure 
	}


	void OutputMemoryBitStream2::WriteFreeBits(const uint8_t bitOffset, const uint32_t inBitCount)
	{
		const uint32_t byteOffset = mBitHead >> 3;

		assert(bitOffset + inBitCount <= 8);

		// calculate which bits of the current byte to preserve
		const uint8_t currentMask = ~(0xff << bitOffset);
		mBuffer[byteOffset] = (mBuffer[byteOffset] & currentMask) | (mBuffer[byteOffset+1] << bitOffset);
	}
#pragma endregion //v2

#pragma region V3
	OutputMemoryBitStream3::OutputMemoryBitStream3() : mBuffer(nullptr), mBitHead(0), mBitCapacity(0), mInData(0)
	{
		ReallocBuffer(256);
	}

	OutputMemoryBitStream3::~OutputMemoryBitStream3()
	{
		std::free(mBuffer);
	}

	void OutputMemoryBitStream3::ReallocBuffer(uint32_t inNewBitLength)
	{
		const uint32_t newByteSize = (inNewBitLength + 7) >> 3;
		if (auto reallocated = std::realloc(mBuffer, newByteSize))
		{
			mBuffer = static_cast<char*>(reallocated);
			mBitCapacity = newByteSize << 3;
		}

		//handle realloc failure 
	}


	void OutputMemoryBitStream3::WriteFreeBits(const uint8_t bitOffset, const uint32_t inBitCount)
	{
		const uint32_t byteOffset = mBitHead >> 3;

		assert(bitOffset + inBitCount <= 8);

		// calculate which bits of the current byte to preserve
		const uint8_t currentMask = ~(0xff << bitOffset);
		mBuffer[byteOffset] = (mBuffer[byteOffset] & currentMask) | (mInData & 0x7) << bitOffset;
	}
#pragma endregion//v3

#pragma region V4
	OutputMemoryBitStream4::OutputMemoryBitStream4() : mBuffer(nullptr), mBitHead(0), mBitCapacity(0)
	{
		ReallocBuffer(256);
	}

	OutputMemoryBitStream4::~OutputMemoryBitStream4()
	{
		std::free(mBuffer);
	}

	void OutputMemoryBitStream4::ReallocBuffer(uint32_t inNewBitLength)
	{
		const uint32_t newByteSize = (inNewBitLength + 7) >> 3;
		if (auto reallocated = std::realloc(mBuffer, static_cast<size_t>(newByteSize) + InDataMaxByteSize))
		{
			mBuffer = static_cast<char*>(reallocated);
			mBitCapacity = newByteSize << 3;
		}

		//handle realloc failure 
	}


	void OutputMemoryBitStream4::WriteFreeBits(const uint8_t bitOffset, const uint32_t inBitCount)
	{
		const uint32_t byteOffset = mBitHead >> 3;

		assert(bitOffset + inBitCount <= 8);

		// calculate which bits of the current byte to preserve
		const uint8_t currentMask = ~(0xff << bitOffset);
		mBuffer[byteOffset] = (mBuffer[byteOffset] & currentMask) | (mBuffer[byteOffset + 1] << bitOffset);
	}
#pragma endregion //v4

#pragma region V5
	OutputMemoryBitStream5::OutputMemoryBitStream5() : mBuffer(nullptr), mBitHead(0), mBitCapacity(0)
	{
		ReallocBuffer(256);
	}

	OutputMemoryBitStream5::~OutputMemoryBitStream5()
	{
		std::free(mBuffer);
	}

	void OutputMemoryBitStream5::ReallocBuffer(uint32_t inNewBitLength)
	{
		const uint32_t newByteSize = (inNewBitLength + 7) >> 3;
		if (auto reallocated = std::realloc(mBuffer, static_cast<size_t>(newByteSize) + InDataMaxByteSize))
		{
			mBuffer = static_cast<char*>(reallocated);
			mBitCapacity = newByteSize << 3;
		}

		//handle realloc failure 
	}

	OutputMemoryBitStream5::byte OutputMemoryBitStream5::FillFreeBits(byte*const inData)	//More than a byte
	{
		const uint32_t byteOffset = mBitHead >> 3;
		const byte bitOffset = mBitHead & 0x7;

		// calculate which bits of the current byte to preserve
		const byte currentMask = ~(0xff << bitOffset);
		mBuffer[byteOffset] = (mBuffer[byteOffset] & currentMask) | (*inData << bitOffset);

		assert(((8 - bitOffset) & 0x7) < 8);

		return (8 - bitOffset) & 0x7;	// Added bits (all available bits)
	}

	OutputMemoryBitStream5::byte OutputMemoryBitStream5::WriteFreeBits(byte*const inData, const byte inBitCount)
	{
		const uint32_t byteOffset = mBitHead >> 3;
		const byte bitOffset = mBitHead & 0x7;

		// calculate which bits of the current byte to preserve
		const byte currentMask = ~(0xff << bitOffset);
		mBuffer[byteOffset] = (mBuffer[byteOffset] & currentMask) | (*inData << bitOffset);

		const byte availableBits = (8 - bitOffset) & 0x7;
		const bool byteOverflow {inBitCount > availableBits};
		return availableBits * (byteOverflow) + inBitCount * (!byteOverflow);	// Added bits (min between available bits and inBitCount)
	}


	/*void OutputMemoryBitStream5::WriteFreeBits(const uint8_t bitOffset, const uint32_t inBitCount)
	{
		const uint32_t byteOffset = mBitHead >> 3;

		assert(bitOffset + inBitCount <= 8);

		// calculate which bits of the current byte to preserve
		const uint8_t currentMask = ~(0xff << bitOffset);
		mBuffer[byteOffset] = (mBuffer[byteOffset] & currentMask) | (mBuffer[byteOffset + 1] << bitOffset);
	}*/
#pragma endregion //v5

#pragma region V6
	OutputMemoryBitStream6::OutputMemoryBitStream6() : mBuffer(nullptr), mBitHead(0), mBitCapacity(0)
	{
		ReallocBuffer(256);
	}

	OutputMemoryBitStream6::~OutputMemoryBitStream6()
	{
		std::free(mBuffer);
	}

	void OutputMemoryBitStream6::ReallocBuffer(const uint32_t inNewBitLength)
	{
		const uint32_t newByteSize = (inNewBitLength + 7) >> 3;
		if (auto reallocated = std::realloc(mBuffer, static_cast<size_t>(newByteSize) + InDataMaxByteSize))
		{
			mBuffer = static_cast<byte*>(reallocated);
			mBitCapacity = newByteSize << 3;	// Guaranteed fit since newByteSize comes from a bitSize value.

			mInData = reinterpret_cast<uint64_t*>(mBuffer + ((mBitHead + 7) >> 3));

			//reallocs++;
		}

		//handle realloc failure 
	}


	void OutputMemoryBitStream6::WriteFreeBits(const uint8_t bitOffset, const uint32_t inBitCount)
	{
		const uint32_t byteOffset = mBitHead >> 3;

		assert(bitOffset + inBitCount <= 8);

		// calculate which bits of the current byte to preserve
		const uint8_t currentMask = ~(0xff << bitOffset);
		mBuffer[byteOffset] = (mBuffer[byteOffset] & currentMask) | (mBuffer[byteOffset + 1] << bitOffset);
	}
#pragma endregion //v6
}}

