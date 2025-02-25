#pragma once
#include <concepts>


namespace Serialization {
	class ISerializableObject;

	template <typename T, size_t size>
	concept is_same_size = sizeof(T) == size;

	namespace Stream {

		template <typename T>
		concept is_primitive_type = std::is_arithmetic_v<T> || std::is_enum_v<T>;

		template <typename T>
		concept is_serializable_Object = std::is_base_of_v<Serialization::ISerializableObject, T>;
	}
}

