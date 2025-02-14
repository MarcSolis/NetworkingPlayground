#include "InputMemoryStream.h"
#include <cstdlib>
#include <algorithm>

Stream::InputMemoryStream::InputMemoryStream(const char* inBuffer, uint32_t inByteCount) : 
	mbuffer(inBuffer), mHead(0), mCapacity(inByteCount)
{
}

uint32_t Stream::InputMemoryStream::GetRemainingDataSize() const noexcept
{
	return mCapacity - mHead;
}

void Stream::InputMemoryStream::ReadInternal(void* outData, size_t inByteCount)
{
	uint32_t resultHead = mHead + static_cast<uint32_t>(inByteCount);

	if (resultHead <= mCapacity)
	{
		std::memcpy(outData, mbuffer + mHead, inByteCount);
		mHead = resultHead;
	}
	
	// Handle copy failure
}
