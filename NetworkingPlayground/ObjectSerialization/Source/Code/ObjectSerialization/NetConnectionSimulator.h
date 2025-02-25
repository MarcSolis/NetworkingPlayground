#pragma once
#include "ObjectSerialization/Streams/StreamTypes.h"
#include "ObjectSerialization/Streams/OutputMemoryStream.h"
#include "ObjectSerialization/Streams/InputMemoryStream.h"
#include "ISerializableObject.h"
#include <iostream>


class NetConnectionSimulator
{
public:

	template <typename ObjectTypeT>
		requires Serialization::Stream::is_serializable_Object<ObjectTypeT>
	void SimulateReplication(const ObjectTypeT* src, ObjectTypeT* dest);

};

template<typename ObjectTypeT>
	requires Serialization::Stream::is_serializable_Object<ObjectTypeT>
void NetConnectionSimulator::SimulateReplication(const ObjectTypeT* src, ObjectTypeT* dest)
{
	Serialization::Stream::OutputMemoryStream outputStream;
	{
		ObjectTypeT tempObj{*src};
		static_cast<Serialization::ISerializableObject*>(&tempObj)->Serialize(outputStream);
		memset(&tempObj, 0, sizeof(ObjectTypeT));	// Simulating data mismatch on mem address
	}
	Serialization::Stream::InputMemoryStream inputStream(outputStream.GetBufferPtr(), outputStream.GetLength());
	static_cast<Serialization::ISerializableObject*>(dest)->Deserialize(inputStream);

	std::cout << "[NetConnectionSimulator] Transmitted data for " << typeid(ObjectTypeT).name() << ": " << outputStream.GetLength() << " bytes" << std::endl;
}
