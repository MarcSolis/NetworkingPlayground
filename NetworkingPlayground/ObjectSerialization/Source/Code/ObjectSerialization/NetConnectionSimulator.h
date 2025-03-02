#pragma once
#include "ObjectSerialization/Streams/StreamTypes.h"
#include "ObjectSerialization/Streams/OutputMemoryStream.h"
#include "ObjectSerialization/Streams/InputMemoryStream.h"
#include "ObjectSerialization/Streams/OutputMemoryBitStream.h"
#include "ISerializableObject.h"
#include <iostream>


class NetConnectionSimulator
{
public:

	template <typename ObjectTypeT>
		requires Serialization::Stream::is_serializable_Object<ObjectTypeT>
	void SimulateStreamReplication(const ObjectTypeT* src, ObjectTypeT* dest);

	template <typename ObjectTypeT>
		requires Serialization::Stream::is_serializable_Object<ObjectTypeT>
	void SimulateBitStreamReplication(const ObjectTypeT* src, ObjectTypeT* dest);

};

template<typename ObjectTypeT>
	requires Serialization::Stream::is_serializable_Object<ObjectTypeT>
void NetConnectionSimulator::SimulateStreamReplication(const ObjectTypeT* src, ObjectTypeT* dest)
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
inline void NetConnectionSimulator::SimulateBitStreamReplication(const ObjectTypeT* src, ObjectTypeT* dest)
{
	Serialization::Stream::OutputMemoryBitStream outputStream;
	{
		ObjectTypeT tempObj{*src};
		static_cast<Serialization::ISerializableObject*>(&tempObj)->Serialize(outputStream);
		memset(&tempObj, 0, sizeof(ObjectTypeT));	// Simulating data mismatch on mem address
	}
	//Serialization::Stream::InputMemoryBitStream inputStream(outputStream.GetBufferPtr(), outputStream.GetBitLength());
	//static_cast<Serialization::ISerializableObject*>(dest)->Deserialize(inputStream);

	std::cout << "[OutputMemoryBitStream] Transmitted data for " << typeid(ObjectTypeT).name() << ": " << outputStream.GetByteLength() << " bytes" << std::endl;
}
