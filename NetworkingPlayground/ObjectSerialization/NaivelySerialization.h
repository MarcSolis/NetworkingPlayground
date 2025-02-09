#pragma once
#include <string.h>
#include <iostream>

namespace Serialization {
	template <typename ObjectType>
	class NaivelySerialization
	{
	public:
		void SimulateSerialization(const ObjectType* inObj, ObjectType* outObj);


	private:
		void NaivelySendObj(const ObjectType* inObj);
		void NaivelyReceiveObj(ObjectType* outObj);


		char* buffer[sizeof(ObjectType)/sizeof(char*)];
	};

	template<typename ObjectType>
	void NaivelySerialization<ObjectType>::SimulateSerialization(const ObjectType* inObj, ObjectType* outObj)
	{
		ObjectType* address;
		{
			ObjectType tempObj = *inObj;	// Use of temp object to simulate network conditions (pointers cannot be serialized due to data mismatch in memory address)
			address = &tempObj;
			NaivelySendObj(address);
		}
		memset(address, NULL, sizeof(ObjectType));	// Simulating data mismatch on mem address
		NaivelyReceiveObj(outObj);

		std::cout << "Transmitted data for " << typeid(ObjectType).name() << ": " << sizeof(ObjectType) << " bytes" << std::endl;
	}

	template<typename ObjectType>
	inline void NaivelySerialization<ObjectType>::NaivelySendObj(const ObjectType* inObj)
	{
		memcpy(buffer, inObj, sizeof(ObjectType));
	}

	template<typename ObjectType>
	inline void NaivelySerialization<ObjectType>::NaivelyReceiveObj(ObjectType* outObj)
	{
		memcpy(outObj, buffer, sizeof(ObjectType));
	}
}


