#pragma once
#include "ObjectSerialization/Streams/StreamDefinitions.h"
#include "Tools/ConstMath/ConstMath.h"

#include <cstddef>
#include <concepts>


namespace Serialization {

	namespace Stream {


		//Defines a valid type to operate with raw data of size N
		template <uint8_t N>
		struct StreamType;

		template <>
		struct StreamType<1>
		{
			using type = uint8_t;
		};

		template <>
		struct StreamType<2>
		{
			using type = uint16_t;
		};

		template <>
		struct StreamType<4>
		{
			using type = uint32_t;
		};

		template <>
		struct StreamType<8>
		{
			using type = uint64_t;
		};


		template <is_primitive_type T>
			requires (sizeof(T) <= MaxDataTypeByteSize)
		struct AsStreamType
		{
			using type = StreamType<sizeof(T)>::type;

			static_assert(sizeof(type) == sizeof(T));
		};


		template <typename T>
		concept is_stream_type = std::is_same_v<T, typename AsStreamType<T>::type>;
	}



}

