#include "RoboCat.h"
#include "NaivelySerialization.h"
#include "NetConnectionSimulator.h"
#include <cassert>
#include "ByteSwapper.h"
#include "Tools/Profiler/Timer.h"


void NaiveSerializationTest()
{
	Serialization::NaivelySerialization<NaiveRoboCat> serializer{};

	NaiveRoboCat originalRC(5, 1);
	NaiveRoboCat copyRC;

	assert(originalRC != copyRC);

	serializer.SimulateSerialization(&originalRC, &copyRC);

	assert(originalRC == copyRC);
}

void NaiveSerializationTest2()
{
	Serialization::NaivelySerialization<RoboCat> serializer{};

	RoboCat originalRC(5, 1);
	const char name[]{ "Abel"};
	originalRC.SetName(name, sizeof(name));
	originalRC.CatchMices({ 0,1,2,3 });

	RoboCat copyRC;

	assert(originalRC != copyRC);

	serializer.SimulateSerialization(&originalRC, &copyRC);

	assert(originalRC == copyRC);	// It fails since std::vector is not naively copiable (a deep copy is required)
}

void StreamSerializationTest()
{
	NetConnectionSimulator connectionSimulator;

	RoboCat originalRC(5, 1);
	const char name[]{"Abel"};
	originalRC.SetName(name, sizeof(name));

	RoboCat copyRC;

	assert(originalRC != copyRC);

	connectionSimulator.SimulateStreamReplication(&originalRC, &copyRC);

	assert(originalRC == copyRC);
}

void StreamBitSerializationTest()
{
	NetConnectionSimulator connectionSimulator;

	RoboCat originalRC(5, 1);
	const char name[]{"Abel"};
	originalRC.SetName(name, sizeof(name));

	RoboCat copyRC;

	assert(originalRC != copyRC);

	connectionSimulator.SimulateBitStreamReplication(&originalRC, &copyRC);

	//assert(originalRC == copyRC);
}

void StreamBitSerializationTestAlt()
{
	NetConnectionSimulator connectionSimulator;

	RoboCat originalRC(5, 1);
	const char name[]{"Abel"};
	originalRC.SetName(name, sizeof(name));

	RoboCat copyRC;

	assert(originalRC != copyRC);

	connectionSimulator.SimulateBitStreamReplicationAlt(&originalRC, &copyRC);

	//assert(originalRC == copyRC);
}

void StreamBitSerializationPerfTest()
{
	std::vector<RoboCat> roboCats(4096);
	Serialization::Stream::OutputMemoryBitStream outputStream;

	{
		profiler::Timer timer(true);
		for (auto&& rb : roboCats)
		{
			rb.Serialize(outputStream);
		}
	}

	std::cout << "Stream size: " << outputStream.GetByteLength() << " Bytes" << std::endl;
}

void StreamBitAltSerializationPerfTest()
{
	std::vector<RoboCat> roboCats(4096);
	Serialization::Stream::OutputMemoryBitStream outputStream;

	{
		profiler::Timer timer(true);
		for (auto&& rb : roboCats)
		{
			rb.SerializeAlt(outputStream);
		}
	}

	std::cout << "Stream size: " << outputStream.GetByteLength() << " Bytes" << std::endl;
}


#pragma optimize("", off)
int main(int argc, char** argv)
{
	//NaiveSerializationTest();
	//StreamSerializationTest();
	if (argc == 1)
	{
		std::cout << "StreamBit Original Test" << std::endl;
		StreamBitSerializationPerfTest();
	}
	else
	{
		std::cout << "StreamBit Alternative Test" << std::endl;
		StreamBitAltSerializationPerfTest();
	}
	
	return 0;
}