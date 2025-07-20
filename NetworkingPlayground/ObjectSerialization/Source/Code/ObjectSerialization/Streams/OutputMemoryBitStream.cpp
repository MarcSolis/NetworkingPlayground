#include "ObjectSerialization/Streams/OutputMemoryBitStream.h"

#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <cassert>


namespace Serialization { namespace Stream {

	OutputMemoryBitStream::OutputMemoryBitStream() : mBuffer(nullptr), mBitHead(0), mBitCapacity(0)
	{
		ReallocBuffer(InitialBufferBitSize);
	}

	OutputMemoryBitStream::~OutputMemoryBitStream()
	{
		std::free(mBuffer);
	}

	void OutputMemoryBitStream::ReallocBuffer(uint32_t inNewBitLength)
	{
		const uint32_t newByteSize = (inNewBitLength + 7) >> 3;
		if (auto reallocated = std::realloc(mBuffer, static_cast<size_t>(newByteSize) + InDataMaxByteSize))
		{
			mBuffer = static_cast<byte*>(reallocated);
			mBitCapacity = newByteSize << 3;
		}

		//handle realloc failure 
	}

	OutputMemoryBitStream::byte OutputMemoryBitStream::FillAllFreeBits(const byte* const inData)	//More than a byte
	{
		const uint32_t byteOffset = mBitHead >> 3;
		const byte bitOffset = mBitHead & 0x7;

		// calculate which bits of the current byte to preserve
		const byte currentMask = ~(0xff << bitOffset);
		mBuffer[byteOffset] = (mBuffer[byteOffset] & currentMask) | (*inData << bitOffset);

		assert(((8 - bitOffset) & 0x7) < 8);

		return (8 - bitOffset) & 0x7;	// Added bits (all available bits)
	}

	OutputMemoryBitStream::byte OutputMemoryBitStream::WriteFreeBits(const byte* const inData, const byte inBitCount)
	{
		const uint32_t byteOffset = mBitHead >> 3;
		const byte bitOffset = mBitHead & 0x7;

		// calculate which bits of the current byte to preserve
		const byte currentMask = ~(0xff << bitOffset);
		mBuffer[byteOffset] = (mBuffer[byteOffset] & currentMask) | (*inData << bitOffset);

		const byte availableBits = (8 - bitOffset) & 0x7;
		const bool byteOverflow{inBitCount > availableBits};
		return availableBits * byteOverflow + inBitCount * !byteOverflow;	// Added bits (min between available bits and inBitCount)
	}

}}

