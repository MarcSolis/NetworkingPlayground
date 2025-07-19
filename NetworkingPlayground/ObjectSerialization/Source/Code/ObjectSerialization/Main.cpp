#include "ObjectSerialization/RoboCat.h"
#include "ObjectSerialization/NaivelySerialization.h"
#include "ObjectSerialization/NetConnectionSimulator.h"
#include "ObjectSerialization/ByteSwapper.h"

#include "Tools/Profiler/Timer.h"

#include <cassert>


static void NaiveSerializationTest()
{
	Serialization::NaivelySerialization<NaiveRoboCat> serializer{};

	NaiveRoboCat originalRC(5, 1);
	NaiveRoboCat copyRC;

	assert(originalRC != copyRC);

	serializer.SimulateSerialization(&originalRC, &copyRC);

	assert(originalRC == copyRC);
}

static void NaiveSerializationTest2()
{
	Serialization::NaivelySerialization<RoboCat> serializer{};

	RoboCat originalRC(5, 1);
	const char name[]{ "Abel"};
	originalRC.SetName(name, sizeof(name));
	originalRC.CatchMice({ 0,1,2,3 });

	RoboCat copyRC;

	assert(originalRC != copyRC);

	serializer.SimulateSerialization(&originalRC, &copyRC);

	assert(originalRC == copyRC);	// It fails since std::vector is not naively copiable (a deep copy is required)
}

static void StreamSerializationTest()
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

static void StreamBitSerializationTest()
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

static void StreamBitSerializationPerfTest(std::vector<RoboCat>& roboCats)
{
	Serialization::Stream::DeprecatedOutputMemoryBitStream outputStream;

	{
		//profiler::Timer timer(true);
		for (auto& rb : roboCats)
		{
			rb.Serialize(outputStream);
		}
	}

	//std::cout << "Stream size: " << outputStream.GetByteLength() << " Bytes" << std::endl;
}


int main(int argc, char** argv)
{
	//NaiveSerializationTest();
	//StreamSerializationTest();
	//StreamBitSerializationTest();
	//StreamBitSerializationTestAlt();

	Serialization::Stream::DeprecatedOutputMemoryBitStream outputStream;
	Serialization::Stream::OutputMemoryBitStream outputStream52;
	RoboCat rb;

	//{
	//	profiler::Timer timer;
	//	rb.SerializeAlt(outputStream52);
	//}
	//
	//return 0;


	constexpr int iterations{1000000};

	
	std::cout << "StreamBit Base Test" << std::endl;
	{
		profiler::Timer timer;
		for (auto i = 0; i < iterations; ++i)
		{
			rb.Serialize(outputStream);
		}
	}

	std::cout << "StreamBit Optimization V5.2 Test" << std::endl;
	{
		profiler::Timer timer;
		for (auto i = 0; i < iterations; ++i)
		{
			rb.SerializeAlt(outputStream52);
		}
	}


	
	
	
	


	return 0;

	if (argc == 1)
	{
		//std::cout << "StreamBit Original Test" << std::endl;
		//rb.Serialize(outputStream);

		std::cout << "StreamBit Optimization V2 Test" << std::endl;
		//rb.SerializeAlt(outputStream2);
	}
	else
	{
		std::cout << "StreamBit Optimization V1 Test" << std::endl;
		//rb.SerializeAlt(outputStream);
	}
	
	return 0;
}