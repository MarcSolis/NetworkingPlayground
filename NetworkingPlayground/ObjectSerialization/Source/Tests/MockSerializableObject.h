// Copyright (c) 2024 Super Teams Mobile, S.L. All rights reserved

#pragma once

#include <gmock/gmock.h>

#include "ObjectSerialization/ISerializableObject.h"
#include "ObjectSerialization/Streams/OutputMemoryStream.h"
#include "ObjectSerialization/Streams/InputMemoryStream.h"
#include "ObjectSerialization/Streams/OutputMemoryBitStream.h"


namespace Serialization { namespace Tests
{
	class MockSerializationObject : public ISerializableObject
	{
	public:
		MockSerializationObject() : 
			ui8Val(std::numeric_limits<uint8_t>().max() / 2),
			ui16Val(std::numeric_limits<uint16_t>().max() / 2),
			ui32Val(std::numeric_limits<uint32_t>().max() / 2),
			ui64Val(std::numeric_limits<uint64_t>().max() / 2)
		{
			SetOnCallActions();
		}

		MockSerializationObject(const MockSerializationObject& other) : ui8Val(other.ui8Val),
			ui16Val(other.ui16Val), ui32Val(other.ui32Val), ui64Val(other.ui64Val)
		{
			SetOnCallActions();
		}

		MockSerializationObject& SetByteVal(const uint8_t& newVal) noexcept;
		MockSerializationObject& Set2ByteVal(const uint16_t& newVal) noexcept;
		MockSerializationObject& Set4ByteVal(const uint32_t& newVal) noexcept;
		MockSerializationObject& Set8ByteVal(const uint64_t& newVal) noexcept;

		MOCK_METHOD(void, Serialize, (Stream::OutputMemoryStream& stream), (override));
		MOCK_METHOD(void, Deserialize, (Stream::InputMemoryStream& stream), (override));
		MOCK_METHOD(void, Serialize, (Stream::DeprecatedOutputMemoryBitStream& stream), (override));
		MOCK_METHOD(void, Serialize, (Stream::OutputMemoryBitStream& stream), (override));


		friend bool operator==(const MockSerializationObject& lhs, const MockSerializationObject& rhs);


		uint8_t ui8Val;
		uint16_t ui16Val;
		uint32_t ui32Val;
		uint64_t ui64Val;

	private:
		void SetOnCallActions();
	};

	
	inline void MockSerializationObject::SetOnCallActions()
	{
		ON_CALL(*this, Serialize(testing::An<Stream::OutputMemoryStream&>())).WillByDefault([this](Stream::OutputMemoryStream& stream)
			{
				stream.Write(ui8Val);
				stream.Write(ui16Val);
				stream.Write(ui32Val);
				stream.Write(ui64Val);
			});

		ON_CALL(*this, Deserialize(testing::An<Stream::InputMemoryStream&>())).WillByDefault([this](Stream::InputMemoryStream& stream)
			{
				stream.Read(ui8Val);
				stream.Read(ui16Val);
				stream.Read(ui32Val);
				stream.Read(ui64Val);
			});

		ON_CALL(*this, Serialize(testing::An<Stream::OutputMemoryBitStream&>())).WillByDefault([this](Stream::OutputMemoryBitStream& stream)
			{
				stream.Write(ui8Val);
				stream.Write(ui16Val);
				stream.Write(ui32Val);
				stream.Write(ui64Val);
			});
	}
}}
