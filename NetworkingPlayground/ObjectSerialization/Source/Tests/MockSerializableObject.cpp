#include "Tests/MockSerializableObject.h"

namespace Serialization {
    namespace Tests {

		MockSerializationObject& Serialization::Tests::MockSerializationObject::SetByteVal(const uint8_t& newVal) noexcept
		{
			ui8Val = newVal;

			return *this;
		}

		MockSerializationObject& MockSerializationObject::Set2ByteVal(const uint16_t& newVal) noexcept
		{
			ui16Val = newVal;

			return *this;
		}

		MockSerializationObject& MockSerializationObject::Set4ByteVal(const uint32_t& newVal) noexcept
		{
			ui32Val = newVal;

			return *this;
		}

		MockSerializationObject& MockSerializationObject::Set8ByteVal(const uint64_t& newVal) noexcept
		{
			ui64Val = newVal;

			return *this;
		}

		bool operator==(const MockSerializationObject& lhs, const MockSerializationObject& rhs)
		{
			return lhs.ui8Val == rhs.ui8Val &&
				lhs.ui16Val == rhs.ui16Val &&
				lhs.ui32Val == rhs.ui32Val &&
				lhs.ui64Val == rhs.ui64Val;
		}

}}

