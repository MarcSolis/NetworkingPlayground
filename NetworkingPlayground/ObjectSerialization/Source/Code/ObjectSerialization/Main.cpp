#include "ObjectSerialization/RoboCat.h"
#include "ObjectSerialization/NaivelySerialization.h"
#include "ObjectSerialization/Simulators/NetConnectionSimulator.h"
#include "ObjectSerialization/ByteSwapper.h"

#include "Tools/Profiler/Timer.h"

#include <cassert>

#pragma region FunctionalTesting
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
	Simulator::NetConnectionSimulator connectionSimulator;

	RoboCat originalRC(5, 1);
	const char name[]{"Abel"};
	originalRC.SetName(name, sizeof(name));

	RoboCat copyRC;

	assert(originalRC != copyRC);

	connectionSimulator.SimulateByteStreamReplication(&originalRC, &copyRC);

	assert(originalRC == copyRC);
}

static void DeprecatedStreamBitSerializationTest()
{
	Simulator::NetConnectionSimulator connectionSimulator;

	RoboCat originalRC(5, 1);
	const char name[]{"Abel"};
	originalRC.SetName(name, sizeof(name));

	RoboCat copyRC;

	assert(originalRC != copyRC);

	connectionSimulator.SimulateDeprecatedBitStreamReplication(&originalRC, &copyRC);

	assert(originalRC.NetEqual(copyRC));
}

static void StreamBitSerializationTest()
{
	Simulator::NetConnectionSimulator connectionSimulator;

	RoboCat originalRC;
	const char name[]{"Abel"};
	originalRC.SetName(name, sizeof(name));

	RoboCat copyRC;

	assert(originalRC != copyRC);

	connectionSimulator.SimulateBitStreamReplication(&originalRC, &copyRC);

	assert(originalRC.NetEqual(copyRC));
}

static void StreamBitSerializationV2Test()
{
	Simulator::NetConnectionSimulator connectionSimulator;

	RoboCat originalRC;
	const char name[]{"Abel"};
	originalRC.SetName(name, sizeof(name));

	RoboCat copyRC;

	assert(originalRC != copyRC);

	connectionSimulator.SimulateBitStreamReplicationV2(&originalRC, &copyRC);

	assert(originalRC.NetEqual(copyRC));
}

#pragma endregion //Functinal Testing

#pragma region PerfTesting
static void DeprecatedStreamBitOutputSerializationPerf(const int iterations)
{
	RoboCat rb;
	Serialization::Stream::DeprecatedOutputMemoryBitStream outputStream;

	std::cout << "StreamBit Output Base Test" << std::endl;
	{
		profiler::Timer timer;
		for (auto i = 0; i < iterations; ++i)
		{
			rb.Serialize(outputStream);
		}
	}
}

static void StreamBitOutputSerializationPerf(const int iterations)
{
	RoboCat rb;
	Serialization::Stream::OutputMemoryBitStream outputStream;

	std::cout << "StreamBit Output Optimization V5.2 Test" << std::endl;
	{
		profiler::Timer timer;
		for (auto i = 0; i < iterations; ++i)
		{
			rb.Serialize(outputStream);
		}
	}
}

static void DeprecatedStreamBitInputSerializationPerf(const int iterations)
{
	RoboCat rb;
	Serialization::Stream::DeprecatedOutputMemoryBitStream outputStream;

	for (auto i = 0; i < iterations; ++i)
	{
		rb.Serialize(outputStream);
	}

	Serialization::Stream::DeprecatedInputMemoryBitStream inputStream(outputStream.GetBufferPtr(), outputStream.GetBitLength());

	std::cout << "StreamBit Input Base Test" << std::endl;
	{
		profiler::Timer timer;
		for (auto i = 0; i < iterations; ++i)
		{
			rb.Deserialize(inputStream);
		}
	}
}


static void StreamBitInputSerializationPerf(const int iterations)
{
	RoboCat rb;
	Serialization::Stream::OutputMemoryBitStream outputStream;

	for (auto i = 0; i < iterations; ++i)
	{
		rb.Serialize(outputStream);
	}

	Serialization::Stream::InputMemoryBitStream inputStream(outputStream.GetBufferPtr(), outputStream.GetBitLength());

	std::cout << "StreamBit Input Optimization V1 Test" << std::endl;
	{
		profiler::Timer timer;
		for (auto i = 0; i < iterations; ++i)
		{
			rb.Deserialize(inputStream);
		}
	}
}

static void StreamBitInputSerializationPerfV2(const int iterations)
{
	RoboCat rb;
	Serialization::Stream::OutputMemoryBitStream outputStream;

	for (auto i = 0; i < iterations; ++i)
	{
		rb.Serialize(outputStream);
	}

	Serialization::Stream::InputMemoryBitStreamV2 inputStream(outputStream.GetBufferPtr(), outputStream.GetBitLength());

	std::cout << "StreamBit Input Optimization V2 Test" << std::endl;
	{
		profiler::Timer timer;
		for (auto i = 0; i < iterations; ++i)
		{
			rb.Deserialize(inputStream);
		}
	}
}
#pragma endregion // Perf testing

int main(int argc, char** argv)
{

	

	std::srand(static_cast<unsigned int>(std::time({}))); // use current time as seed for random generator
	int result = 0;

	{
		profiler::Timer timer;
		for (int i = 0; i < 100000; ++i)
		{
			result += (rand() > rand()) + 1;
		}
	}
	
	std::cout << "Result: " << result << std::endl;

	return 0;



	//NaiveSerializationTest();
	//StreamSerializationTest();
	//DeprecatedStreamBitSerializationTest();
	//StreamBitSerializationTest();
	//StreamBitSerializationV2Test();
	//
	//
	//return 0;

	Serialization::Stream::DeprecatedOutputMemoryBitStream outputStream;
	Serialization::Stream::OutputMemoryBitStream outputStream52;
	RoboCat rb;

	//{
	//	profiler::Timer timer;
	//	rb.Serialize(outputStream52);
	//}
	
	//return 0;


	constexpr int iterations{1000000};

	//DeprecatedStreamBitOutputSerializationPerf(iterations);
	//StreamBitOutputSerializationPerf(iterations);

	//DeprecatedStreamBitInputSerializationPerf(iterations);
	//StreamBitInputSerializationPerf(iterations);
	StreamBitInputSerializationPerfV2(iterations);


	
	
	
	


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