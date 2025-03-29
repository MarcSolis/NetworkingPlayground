#pragma once
#include "ObjectSerialization/Streams/StreamTypes.h"
#include <cstdint>
#include <bit>


namespace Serialization {

	constexpr inline uint16_t ByteSwap2(uint16_t inData) noexcept
	{
		return (inData >> 8) | (inData << 8);
	}

	constexpr inline uint32_t ByteSwap4(uint32_t inData) noexcept
	{
		return ((inData >> 24) & 0x000000ff) |
			((inData >> 8) & 0x0000ff00) |
			((inData << 8) & 0x00ff0000) |
			((inData << 24) & 0xff000000);
	}

	constexpr inline uint64_t ByteSwap8(uint64_t inData) noexcept
	{
		return ((inData >> 56) & 0x00000000000000ff) |
			((inData >> 40) & 0x000000000000ff00) |
			((inData >> 24) & 0x0000000000ff0000) |
			((inData >> 8) & 0x00000000ff000000) |
			((inData << 8) & 0x000000ff00000000) |
			((inData << 24) & 0x0000ff0000000000) |
			((inData << 40) & 0x00ff000000000000) |
			((inData << 56) & 0xff00000000000000);
	}


	template <Stream::is_primitive_type T, size_t size> class ByteSwapper;

	// specialization for 1
	template <Stream::is_primitive_type T>
		requires is_same_size<T, 1>
	class ByteSwapper<T, 1>
	{
	public:
		T Swap(const T inData) const noexcept
		{
			return inData;
		}
	};

	// specialization for 2
	template <Stream::is_primitive_type T>
		requires is_same_size<T, 2>
	class ByteSwapper<T, 2>
	{
	public:
		constexpr T Swap(const T inData) const noexcept
		{
			uint16_t result = ByteSwap2(std::bit_cast<uint16_t>(inData));
			return std::bit_cast<T>(result);
		}
	};

	// specialization for 4
	template <Stream::is_primitive_type T>
		requires is_same_size<T, 4>
	class ByteSwapper<T, 4>
	{
	public:
		constexpr T Swap(T inData) const noexcept
		{
			uint32_t result = ByteSwap4(std::bit_cast<uint32_t>(inData));
			return std::bit_cast<T>(result);
		}
	};

	// specialization for 8
	template <Stream::is_primitive_type T>
		requires is_same_size<T, 8>
	class ByteSwapper<T, 8>
	{
	public:
		constexpr T Swap(T inData) const noexcept
		{
			uint64_t result = ByteSwap8(std::bit_cast<uint64_t>(inData));
			return std::bit_cast<T>(result);
		}
	};

	template <Stream::is_primitive_type T>
	constexpr T ByteSwap(T inData)
	{
		return ByteSwapper<T, sizeof(T)>().Swap(inData);
	}
}
