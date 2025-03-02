#include "OutputMemoryBitStream.h"
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <cassert>

namespace Serialization { namespace Stream {

	OutputMemoryBitStream::OutputMemoryBitStream() : mBuffer(nullptr), mBitHead(0), mBitCapacity(0)
	{
		ReallocBuffer(256);
	}

	OutputMemoryBitStream::~OutputMemoryBitStream()
	{
		std::free(mBuffer);
	}

	void OutputMemoryBitStream::WriteBitsInternal(uint8_t inData, size_t inBitCount)
	{
		assert(inBitCount <= 8);

		uint32_t nextBitHead = mBitHead + static_cast<uint32_t>(inBitCount);
		if (nextBitHead > mBitCapacity)
		{
			ReallocBuffer(std::max(mBitCapacity * 2, nextBitHead));
		}

		uint32_t byteOffset = mBitHead >> 3;
		uint32_t bitOffset = mBitHead & 0x7;

		// calculate which bits of the current byte to preserve
		uint8_t currentMask = ~(0xff << bitOffset);
		mBuffer[byteOffset] = (mBuffer[byteOffset] & currentMask) | (inData << bitOffset);

		uint32_t currentByteFreeBits = 8 - bitOffset;
		if (currentByteFreeBits < inBitCount)
		{
			//we need another byte
			mBuffer[byteOffset + 1] = inData >> currentByteFreeBits;
		}

		mBitHead = nextBitHead;
	}

	void OutputMemoryBitStream::WriteBits(const void* inData, size_t inBitCount)
	{
		const char* srcByte = static_cast<const char*>(inData);
		// Write bytes
		while (inBitCount > 8)
		{
			WriteBitsInternal(*srcByte, 8);
			++srcByte;
			inBitCount -= 8;
		}
		// Write bits left
		if (inBitCount > 0)
		{
			WriteBitsInternal(*srcByte, inBitCount);
		}
	}

	void OutputMemoryBitStream::ReallocBuffer(uint32_t inNewBitLength)
	{
		uint32_t newByteSize = (inNewBitLength + 7) >> 3;
		if (auto reallocated = std::realloc(mBuffer, newByteSize))
		{
			mBuffer = static_cast<char*>(reallocated);
			mBitCapacity = newByteSize << 3;
		}

		//handle realloc failure 
	}


	void OutputMemoryBitStream::WriteBitsAlt(const void* inData, size_t inBitCount)
	{
		const char* srcByte = static_cast<const char*>(inData);

		uint8_t bitOffset = mBitHead & 0x7;
		if (bitOffset > 0)
		{
			WriteBitsInternal(*srcByte, 8-bitOffset);
		}


		// Write bytes
		while (inBitCount > 8)
		{
			WriteBitsInternal(*srcByte, 8);
			++srcByte;
			inBitCount -= 8;
		}
		// Write bits left
		if (inBitCount > 0)
		{
			WriteBitsInternal(*srcByte, inBitCount);
		}
	}

	void OutputMemoryBitStream::WriteBytesInternal(const void* inData, size_t inBitCount)
	{
		// ensure we have space 
		uint32_t nextBitHead = mBitHead + static_cast<uint32_t>(inBitCount);
		if (nextBitHead > mBitCapacity)
		{
			ReallocBuffer(std::max(mBitCapacity * 2, nextBitHead));
		}

		std::memcpy(mBuffer + (mBitHead>>3), inData, (inBitCount+7)>>3);
		mBitHead = nextBitHead;
	}
}}

