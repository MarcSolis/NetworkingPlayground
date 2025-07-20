#include "ObjectSerialization/Streams/OutputMemoryStream.h"

#include <cstdlib>
#include <cstring>
#include <algorithm>


namespace Serialization { namespace Stream {

	OutputMemoryStream::OutputMemoryStream() : mBuffer(nullptr), mHead(0), mCapacity(0)
	{
		ReallocBuffer(32);
	}

	OutputMemoryStream::~OutputMemoryStream()
	{
		std::free(mBuffer);
	}

	void OutputMemoryStream::WriteInternal(const void* inData, size_t inByteCount)
	{
		// ensure we have space 
		uint32_t resultHead = mHead + static_cast<uint32_t>(inByteCount);
		if (resultHead > mCapacity)
		{
			ReallocBuffer(std::max(mCapacity * 2, resultHead));
		}

		std::memcpy(mBuffer + mHead, inData, inByteCount);
		mHead = resultHead;
	}

	void OutputMemoryStream::ReallocBuffer(uint32_t inNewLength)
	{
		if (auto reallocated = std::realloc(mBuffer, inNewLength))
		{
			mBuffer = static_cast<char*>(reallocated);
			mCapacity = inNewLength;
		}

		//handle realloc failure 
	}
}}

