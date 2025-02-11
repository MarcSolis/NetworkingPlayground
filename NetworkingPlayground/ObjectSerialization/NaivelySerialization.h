#pragma once
#include <string.h>
#include <iostream>

namespace Serialization {

	template <typename ObjectTypeT>
	class NaivelySerialization
	{
	public:
		void SimulateSerialization(const ObjectTypeT* inObj, ObjectTypeT* outObj);

	private:
		void NaivelySendObj(const ObjectTypeT* inObj);
		void NaivelyReceiveObj(ObjectTypeT* outObj);


		char* buffer[sizeof(ObjectTypeT)/sizeof(char*)];
	};

	template<typename ObjectTypeT>
	void NaivelySerialization<ObjectTypeT>::SimulateSerialization(const ObjectTypeT* inObj, ObjectTypeT* outObj)
	{
		ObjectTypeT* address;
		{
			ObjectTypeT tempObj = *inObj;	// Use of temp object to simulate network conditions (pointers cannot be serialized due to data mismatch in memory address)
			address = &tempObj;
			NaivelySendObj(address);
		}
		memset(address, NULL, sizeof(ObjectTypeT));	// Simulating data mismatch on mem address
		NaivelyReceiveObj(outObj);

		std::cout << "[NaivelySerialization] Transmitted data for " << typeid(ObjectTypeT).name() << ": " << sizeof(ObjectTypeT) << " bytes" << std::endl;
	}

	template<typename ObjectTypeT>
	inline void NaivelySerialization<ObjectTypeT>::NaivelySendObj(const ObjectTypeT* inObj)
	{
		memcpy(buffer, inObj, sizeof(ObjectTypeT));
	}

	template<typename ObjectTypeT>
	inline void NaivelySerialization<ObjectTypeT>::NaivelyReceiveObj(ObjectTypeT* outObj)
	{
		memcpy(outObj, buffer, sizeof(ObjectTypeT));
	}
}


