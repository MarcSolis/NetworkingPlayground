
#include "RoboCat.h"
#include "NaivelySerialization.h"
#include <cassert>


void NaiveSerializationTest()
{
	Serialization::NaivelySerialization<NaiveRoboCat> serializer;

	NaiveRoboCat originalRC(5, 1);
	NaiveRoboCat copyRC;

	assert(originalRC != copyRC);

	serializer.SimulateSerialization(&originalRC, &copyRC);

	assert(originalRC == copyRC);
}

void NaiveSerializationTest2()
{
	Serialization::NaivelySerialization<RoboCat> serializer;

	RoboCat originalRC(5, 1);
	const char name[]{ "Abel"};
	originalRC.SetName(name, sizeof(name));
	originalRC.CatchMices({ 0,1,2,3 });

	RoboCat copyRC;

	assert(originalRC != copyRC);

	serializer.SimulateSerialization(&originalRC, &copyRC);

	assert(originalRC == copyRC);	// It fails since std::vector is not naively copiable (a deep copy is required)
}


int main()
{
	NaiveSerializationTest();

	return 0;
}