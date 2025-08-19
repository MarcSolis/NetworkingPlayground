#include "ObjectSerialization/Streams/InputMemoryBitStream.h"

#include <cassert>


namespace Serialization { namespace Stream {

	InputMemoryBitStream::InputMemoryBitStream(const byte* buffer, const uint32_t mByteLength) : 
		mBuffer(buffer), mByteLength(mByteLength), mBitHead(0)
	{
		assert(buffer != nullptr);
	}

	InputMemoryBitStream::InputMemoryBitStream(InputMemoryBitStream&& other) noexcept :
		mBuffer(nullptr), mByteLength(std::move(other.mByteLength)), mBitHead(std::move(other.mBitHead))
	{
		using std::swap;
		swap(mBuffer, other.mBuffer);
	}

	InputMemoryBitStream& InputMemoryBitStream::operator=(InputMemoryBitStream&& other) noexcept
	{
		using std::swap;
		swap(mBuffer, other.mBuffer);
		mByteLength = other.mByteLength;
		mBitHead = other.mBitHead;

		return *this;
	}

	void InputMemoryBitStream::ReadBits(byte& dest, const byte bitsOffset)
	{
		const byte bitsToAdd = *GetCurrentByte() << bitsOffset;
		const byte currentMask = ~(0xff << bitsOffset);
		dest = (dest & currentMask) | bitsToAdd;
	}




}}

