#include "ObjectSerialization/Streams/InputMemoryStream.h"

#include <cstdlib>
#include <cstring>
#include <algorithm>


namespace Serialization {
	namespace Stream {

		InputMemoryStream::InputMemoryStream(const char* inBuffer, uint32_t inByteCount) :
			mbuffer(inBuffer), mHead(0), mCapacity(inByteCount)
		{
		}

		uint32_t InputMemoryStream::GetRemainingDataSize() const noexcept
		{
			return mCapacity - mHead;
		}

		void InputMemoryStream::ReadInternal(void* outData, size_t inByteCount)
		{
			uint32_t resultHead = mHead + static_cast<uint32_t>(inByteCount);

			if (resultHead <= mCapacity)
			{
				std::memcpy(outData, mbuffer + mHead, inByteCount);
				mHead = resultHead;
			}

			// Handle copy failure
		}
	}
}

