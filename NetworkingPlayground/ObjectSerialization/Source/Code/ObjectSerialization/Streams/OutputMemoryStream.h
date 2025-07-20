#pragma once
#include "ObjectSerialization/Streams/StreamTypes.h"
#include "ObjectSerialization/ByteSwapper.h"

#include <cstdint>
#include <bit>


namespace Serialization { namespace Stream {

	class OutputMemoryStream
	{
	public:
		OutputMemoryStream();
		~OutputMemoryStream();

		const char* GetBufferPtr() const { return mBuffer; }
		uint32_t GetLength() const noexcept { return mHead; }

		template <is_primitive_type T>
		void Write(T inData);

		template <is_primitive_type T, size_t N>
		void Write(T(&inData)[N]);

	private:
		void WriteInternal(const void* inData, size_t inByteCount);
		void ReallocBuffer(uint32_t inNewLength);

		char* mBuffer;
		uint32_t mHead;
		uint32_t mCapacity;
		static constexpr std::endian mEndian{std::endian::little};
	};

	template<is_primitive_type T>
	inline void OutputMemoryStream::Write(T inData)
	{
		if constexpr (std::endian::native == mEndian)
		{
			WriteInternal(&inData, sizeof(inData));
		}
		else
		{
			T swappedData{Serialization::ByteSwap(inData)};
			WriteInternal(&swappedData, sizeof(swappedData));
		}
	}

	template<is_primitive_type T, size_t N>
	inline void OutputMemoryStream::Write(T(&inData)[N])
	{
		
		if constexpr (std::endian::native == mEndian)
		{
			WriteInternal(&inData, sizeof(inData));
		}
		else
		{
			T swappedData[N];
			for (size_t i = 0; i < N; ++i)
			{
				swappedData[i] = Serialization::ByteSwap(inData[i]);
			}

			WriteInternal(&swappedData, sizeof(swappedData));
		}
	}

}}


