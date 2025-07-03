// Copyright (c) 2025 Marc Solis Serrano

namespace ConstMath
{
	template <size_t x>
	struct log2
	{
		static const uint8_t value = 1 + log2<(x >> 1)>::value;
	};

	template <>
	struct log2<1>
	{
		static const uint8_t value = 0;
	};

	template <>
	struct log2<0>;		// Declaration only, disallow this specific case.
}
