#include "OutputMemoryBitStream2.h"
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <cassert>

namespace Serialization { namespace Stream {

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
		if (auto reallocated = std::realloc(mBuffer, newByteSize + InDataMaxSize))
		{
			mBuffer = static_cast<char*>(reallocated);
			mBitCapacity = newByteSize << 3;

			mInData = reinterpret_cast<uint64_t*>(mBuffer + (mBitHead >> 3));
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
}}

