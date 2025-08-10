#include "ObjectSerialization/Streams/DeprecatedInputMemoryBitStream.h"


namespace Serialization { namespace Stream {

	DeprecatedInputMemoryBitStream::DeprecatedInputMemoryBitStream(const char* inBuffer, uint32_t inBitCount) : mBuffer(inBuffer), mBitHead(0), mBitCapacity(inBitCount)
	{
		assert(mBuffer != nullptr);
	}

	void DeprecatedInputMemoryBitStream::ReadBitsInternal(uint8_t& outData, size_t outBitCount)
	{
		assert(outBitCount <= 8);

		uint32_t byteOffset = mBitHead >> 3;
		uint32_t bitOffset = mBitHead & 0x7;

		outData = static_cast<uint8_t>(mBuffer[byteOffset]) >> bitOffset;

		uint32_t bitsLoaded = 8 - bitOffset;
		if (bitsLoaded < outBitCount)
		{
			//we need another byte
			outData |= static_cast<uint8_t>(mBuffer[byteOffset + 1]) << bitsLoaded;
		}

		//don't forget a mask so that we only read the bit we wanted...
		outData &= ~(0x00ff << outBitCount);

		mBitHead += static_cast<uint32_t>(outBitCount);
	}

	void DeprecatedInputMemoryBitStream::ReadBits(void* outData, size_t outBitCount)
	{
		uint8_t* destByte = reinterpret_cast<uint8_t*>(outData);

		// Read bytes
		while (outBitCount > 8)
		{
			ReadBitsInternal(*destByte, 8);
			++destByte;
			outBitCount -= 8;
		}
		// Read bits left
		if (outBitCount > 0)
		{
			ReadBitsInternal(*destByte, outBitCount);
		}
	}
}}

