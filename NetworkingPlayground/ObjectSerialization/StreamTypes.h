#pragma once
#include <concepts>


namespace Serialization {
	class ISerializableObject;
}

namespace Stream {

	template <typename T>
	concept is_primitive_type = std::is_arithmetic_v<T> || std::is_enum_v<T>;

	template <typename T>
	concept is_serializable_Object = std::is_base_of_v<Serialization::ISerializableObject, T>;
}