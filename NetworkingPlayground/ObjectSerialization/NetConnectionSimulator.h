#pragma once
#include "StreamTypes.h"
#include "OutputMemoryStream.h"
#include "InputMemoryStream.h"
#include "ISerializableObject.h"
#include <iostream>


class NetConnectionSimulator
{
public:

	template <typename ObjectTypeT>
		requires Stream::is_serializable_Object<ObjectTypeT>
	void SimulateReplication(const ObjectTypeT* src, ObjectTypeT* dest);

};

template<typename ObjectTypeT>
	requires Stream::is_serializable_Object<ObjectTypeT>
void NetConnectionSimulator::SimulateReplication(const ObjectTypeT* src, ObjectTypeT* dest)
{
	Stream::OutputMemoryStream outputStream;
	{
		ObjectTypeT tempObj{*src};
		static_cast<Serialization::ISerializableObject*>(&tempObj)->Serialize(outputStream);
		memset(&tempObj, NULL, sizeof(ObjectTypeT));	// Simulating data mismatch on mem address
	}
	Stream::InputMemoryStream inputStream(outputStream.GetBufferPtr(), outputStream.GetLength());
	static_cast<Serialization::ISerializableObject*>(dest)->Deserialize(inputStream);

	std::cout << "[NetConnectionSimulator] Transmitted data for " << typeid(ObjectTypeT).name() << ": " << outputStream.GetLength() << " bytes" << std::endl;
}
