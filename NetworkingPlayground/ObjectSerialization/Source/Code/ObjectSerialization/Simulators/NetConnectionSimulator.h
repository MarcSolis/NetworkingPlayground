#pragma once
#include "ObjectSerialization/Streams/StreamDefinitions.h"
#include "ObjectSerialization/ISerializableObject.h"
#include "ObjectSerialization/RoboCat.h"

#include "ObjectSerialization/Streams/OutputMemoryStream.h"
#include "ObjectSerialization/Streams/InputMemoryStream.h"

#include "ObjectSerialization/Streams/OutputMemoryBitStream.h"
#include "ObjectSerialization/Streams/InputMemoryBitStream.h"

#include "ObjectSerialization/Streams/DeprecatedOutputMemoryBitStream.h"
#include "ObjectSerialization/Streams/DeprecatedInputMemoryBitStream.h"

#include <iostream>


namespace Simulator {

	class NetConnectionSimulator
	{
	public:

		template <typename ObjectTypeT>
			requires Serialization::Stream::is_serializable_Object<ObjectTypeT>
		void SimulateByteStreamReplication(const ObjectTypeT* src, ObjectTypeT* dest);

		template <typename ObjectTypeT>
			requires Serialization::Stream::is_serializable_Object<ObjectTypeT>
		void SimulateDeprecatedBitStreamReplication(const ObjectTypeT* src, ObjectTypeT* dest);

		template <typename ObjectTypeT>
			requires Serialization::Stream::is_serializable_Object<ObjectTypeT>
		void SimulateBitStreamReplication(const ObjectTypeT* src, ObjectTypeT* dest);

		template <typename ObjectTypeT>
			requires Serialization::Stream::is_serializable_Object<ObjectTypeT>
		void SimulateBitStreamReplicationV2(const ObjectTypeT* src, ObjectTypeT* dest);
	};

	template<typename ObjectTypeT>
		requires Serialization::Stream::is_serializable_Object<ObjectTypeT>
	void NetConnectionSimulator::SimulateByteStreamReplication(const ObjectTypeT* src, ObjectTypeT* dest)
	{
		Serialization::Stream::OutputMemoryStream outputStream;
		{
			ObjectTypeT tempObj{*src};
			static_cast<Serialization::ISerializableObject*>(&tempObj)->Serialize(outputStream);
			memset(&tempObj, 0, sizeof(ObjectTypeT));	// Simulating data mismatch on mem address
		}

		Serialization::Stream::InputMemoryStream inputStream(outputStream.GetBufferPtr(), outputStream.GetLength());
		static_cast<Serialization::ISerializableObject*>(dest)->Deserialize(inputStream);

		std::cout << "[OutputMemoryStream] Transmitted data for " << typeid(ObjectTypeT).name() << ": " << outputStream.GetLength() << " bytes" << std::endl;
	}

	template<typename ObjectTypeT>
		requires Serialization::Stream::is_serializable_Object<ObjectTypeT>
	inline void NetConnectionSimulator::SimulateDeprecatedBitStreamReplication(const ObjectTypeT* src, ObjectTypeT* dest)
	{
		Serialization::Stream::DeprecatedOutputMemoryBitStream outputStream;
		{
			ObjectTypeT tempObj{*src};
			static_cast<Serialization::ISerializableObject*>(&tempObj)->Serialize(outputStream);
			memset(&tempObj, 0, sizeof(ObjectTypeT));	// Simulating data mismatch on mem address
		}

		Serialization::Stream::DeprecatedInputMemoryBitStream inputStream(outputStream.GetBufferPtr(), outputStream.GetBitLength());
		static_cast<Serialization::ISerializableObject*>(dest)->Deserialize(inputStream);

		std::cout << "[OutputMemoryBitStream] Transmitted data for " << typeid(ObjectTypeT).name() << ": " << outputStream.GetByteLength() << " bytes" << std::endl;
	}

	template<typename ObjectTypeT>
		requires Serialization::Stream::is_serializable_Object<ObjectTypeT>
	inline void NetConnectionSimulator::SimulateBitStreamReplication(const ObjectTypeT* src, ObjectTypeT* dest)
	{
		Serialization::Stream::OutputMemoryBitStream outputStream;
		{
			ObjectTypeT tempObj{*src};
			static_cast<Serialization::ISerializableObject*>(&tempObj)->Serialize(outputStream);
			memset(&tempObj, 0, sizeof(ObjectTypeT));	// Simulating data mismatch on mem address
		}

		Serialization::Stream::InputMemoryBitStream inputStream(outputStream.GetBufferPtr(), outputStream.GetByteLength());
		static_cast<Serialization::ISerializableObject*>(dest)->Deserialize(inputStream);

		std::cout << "[OutputMemoryBitStream] Transmitted data for " << typeid(ObjectTypeT).name() << ": " << outputStream.GetByteLength() << " bytes" << std::endl;
	}

	template<typename ObjectTypeT>
		requires Serialization::Stream::is_serializable_Object<ObjectTypeT>
	inline void NetConnectionSimulator::SimulateBitStreamReplicationV2(const ObjectTypeT* src, ObjectTypeT* dest)
	{
		Serialization::Stream::OutputMemoryBitStream outputStream;
		{
			ObjectTypeT tempObj{*src};
			static_cast<Serialization::ISerializableObject*>(&tempObj)->Serialize(outputStream);
			memset(&tempObj, 0, sizeof(ObjectTypeT));	// Simulating data mismatch on mem address
		}

		Serialization::Stream::InputMemoryBitStreamV2 inputStream(outputStream.GetBufferPtr(), outputStream.GetByteLength());
		static_cast<Serialization::ISerializableObject*>(dest)->Deserialize(inputStream);

		std::cout << "[OutputMemoryBitStreamV2] Transmitted data for " << typeid(ObjectTypeT).name() << ": " << outputStream.GetByteLength() << " bytes" << std::endl;
	}
}

