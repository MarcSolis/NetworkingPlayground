#pragma once
#include "ObjectSerialization/Streams/StreamDefinitions.h"
#include "ObjectSerialization/ByteSwapper.h"
#include "ObjectSerialization/Streams/StreamType.h"

#include <memory>


namespace Serialization { namespace Stream {

	// TODO: Once socket architecture is implemented, decide if this creates the buffer, gets an owning buffer ref, or a non-owning ref.
	// For simplicity, non-owning ref assumed meanwhile.

#pragma region InputOptV1
	class InputMemoryBitStream
	{
	public:
		InputMemoryBitStream(const byte* buffer, const uint32_t mByteLength);

		InputMemoryBitStream(const InputMemoryBitStream& other) = delete;
		InputMemoryBitStream(InputMemoryBitStream&& other) noexcept;
		InputMemoryBitStream& operator=(const InputMemoryBitStream& other) = delete;
		InputMemoryBitStream& operator=(InputMemoryBitStream&& other) noexcept;
		~InputMemoryBitStream() = default;

		template<uint32_t OutBitCount, is_primitive_type T>
		void Read(T& outData);

		template<is_primitive_type T, uint32_t OutBitCount = sizeof(T) << 3>
		void Read(T& outData);


	private:
		inline const byte* GetCurrentByte() noexcept { return mBuffer + (mBitHead >> 3); }

		template<is_stream_type T, uint32_t OutBitCount>
		void ReadInternal(T& outData);
		void ReadBits(byte& dest, const byte bitsToRead);

		const byte* mBuffer;
		uint32_t mByteLength;
		uint32_t mBitHead;

		static constexpr std::endian Endian{std::endian::little};
	};

	template<uint32_t OutBitCount, is_primitive_type T>
	inline void InputMemoryBitStream::Read(T& outData)
	{
		// reduces memory usage by constraining instantiations to stream types
		typedef typename AsStreamType<T>::type StreamType;
		ReadInternal<StreamType, OutBitCount>(reinterpret_cast<StreamType&>(outData));
	}

	template<is_primitive_type T, uint32_t OutBitCount>
	inline void InputMemoryBitStream::Read(T& outData)
	{
		// reduces memory usage by constraining instantiations to stream types
		typedef typename AsStreamType<T>::type StreamType;
		ReadInternal<StreamType, OutBitCount>(reinterpret_cast<StreamType&>(outData));
	}

	template<is_stream_type T, uint32_t OutBitCount>
	inline void InputMemoryBitStream::ReadInternal(T& outData)
	{
		static_assert(OutBitCount <= (sizeof(outData) << 3), "More bits requested than type provides!");
		static_assert(sizeof(outData) <= MaxDataTypeByteSize, "Unsupported type, maximum type size exceeded");

		constexpr uint32_t OutByteCount = (OutBitCount + 7) >> 3;

		//if (mBitHead >= (mByteLength << 3)) [[unlikely]]
		//{
		//	return;
		//}

		const byte bufferOffset = (mBitHead & 0x7);

		std::memcpy(&outData, GetCurrentByte(), OutByteCount);
		mBitHead += OutBitCount;
		
		outData >>= bufferOffset;	// Align data

		ReadBits(*(reinterpret_cast<byte*>(&outData) + (OutBitCount >> 3)), bufferOffset);

		constexpr T dataMask = ~(~T(0) << OutBitCount);
		outData &= dataMask;

		if constexpr (Endian != std::endian::native)
		{
			outData = ByteSwap(outData);
		}
	}

#pragma endregion // V1

	class InputMemoryBitStreamV2
	{
	public:
		InputMemoryBitStreamV2(const byte* buffer, const uint32_t mByteLength);

		InputMemoryBitStreamV2(const InputMemoryBitStreamV2& other) = delete;
		InputMemoryBitStreamV2(InputMemoryBitStreamV2&& other) noexcept;
		InputMemoryBitStreamV2& operator=(const InputMemoryBitStreamV2& other) = delete;
		InputMemoryBitStreamV2& operator=(InputMemoryBitStreamV2&& other) noexcept;
		~InputMemoryBitStreamV2() = default;

		template<uint32_t OutBitCount, is_primitive_type T>
		void Read(T& outData);

		template<is_primitive_type T, uint32_t OutBitCount = sizeof(T) << 3>
		void Read(T& outData);


	private:
		[[nodiscard]] inline const byte* GetCurrentByte() noexcept { return mBuffer + (mBitHead >> 3); }

		template<is_stream_type T, uint32_t OutBitCount>
		void ReadInternal(T& outData);

		const byte* mBuffer;
		uint32_t mByteLength;
		uint32_t mBitHead;

		static constexpr std::endian Endian{std::endian::little};
	};

	template<uint32_t OutBitCount, is_primitive_type T>
	inline void InputMemoryBitStreamV2::Read(T& outData)
	{
		// reduces memory usage by constraining instantiations to stream types
		typedef typename AsStreamType<T>::type StreamType;
		ReadInternal<StreamType, OutBitCount>(reinterpret_cast<StreamType&>(outData));
	}

	template<is_primitive_type T, uint32_t OutBitCount>
	inline void InputMemoryBitStreamV2::Read(T& outData)
	{
		// reduces memory usage by constraining instantiations to stream types
		typedef typename AsStreamType<T>::type StreamType;
		ReadInternal<StreamType, OutBitCount>(reinterpret_cast<StreamType&>(outData));
	}

	template<is_stream_type T, uint32_t OutBitCount>
	inline void InputMemoryBitStreamV2::ReadInternal(T& outData)
	{
		static_assert(OutBitCount <= (sizeof(outData) << 3), "More bits requested than type provides!");
		static_assert(sizeof(outData) <= MaxDataTypeByteSize, "Unsupported type, maximum type size exceeded");

		if constexpr ((sizeof(T) >> 3) > (OutBitCount + 6))	// At least 7 extra bits
		{
			std::memcpy(&outData, GetCurrentByte(), sizeof(outData));
			const byte bitOffset = mBitHead & 0x7;
			outData >>= bitOffset;
		}
		else
		{
			auto bufferStart = GetCurrentByte();
			std::memcpy(&outData, bufferStart, sizeof(outData));
			const byte bitOffset = mBitHead & 0x7;
			outData >>= bitOffset;

			byte pendingBits = *(bufferStart + sizeof(outData));
			pendingBits &= ~(0xff << bitOffset);	// get only the bits of the current byte to preserve

			byte& outDataLastByte = *(reinterpret_cast<byte*>(&outData) + sizeof(outData));
			outDataLastByte |= pendingBits;
		}

		if constexpr ((sizeof(T) >> 3) > OutBitCount)
		{
			constexpr T dataMask = ~(~T(0) << OutBitCount);
			outData &= dataMask;
		}

		mBitHead += OutBitCount;
	}

}}


