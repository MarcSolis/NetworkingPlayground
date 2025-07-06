#include "ObjectSerialization/RoboCat.h"
#include "ObjectSerialization/Streams/OutputMemoryStream.h"
#include "ObjectSerialization/Streams/InputMemoryStream.h"
#include "ObjectSerialization/Streams/OutputMemoryBitStream.h"

#include <cstring>
#include <cassert>
#include <cstdlib>

#pragma region NaiveRoboCat
NaiveRoboCat::NaiveRoboCat() : mHealth(10), mMeowCount(3) 
{
	mHealth = rand();
	mMeowCount = rand();
}


NaiveRoboCat::NaiveRoboCat(int32_t mHealth, int32_t mMeowCount) 
	: mHealth(mHealth), mMeowCount(mMeowCount)
{
}

bool NaiveRoboCat::operator==(const NaiveRoboCat& other)
{
	return mHealth == other.mHealth && mMeowCount == other.mMeowCount;
}

bool NaiveRoboCat::operator!=(const NaiveRoboCat& other)
{
	return !operator==(other);
}
#pragma endregion // Naive


RoboCat::RoboCat() noexcept : mHealth(10), mMeowCount(3)
{
}

RoboCat::RoboCat(int32_t health, int32_t meowCount) noexcept : mHealth(health), mMeowCount(meowCount)
{
}

void RoboCat::SetName(const char* name, size_t length)
{
	assert(length <= BUFFER_SIZE);

	std::memset(mName, 0, sizeof(mName));
	std::memcpy(mName, name, length);
}

void RoboCat::CatchMices(std::vector<int32_t> miceIndices)
{
	mMiceIndices = miceIndices;
}

bool RoboCat::operator==(const RoboCat& other)
{
	bool sameString = strcmp(mName, other.mName) == 0;
	return mHealth == other.mHealth && mMeowCount == other.mMeowCount && sameString && mMiceIndices == other.mMiceIndices;
}

bool RoboCat::operator!=(const RoboCat& other)
{
	return !operator==(other);
}

void RoboCat::Serialize(Serialization::Stream::OutputMemoryStream& stream)
{
	stream.Write(mHealth);
	stream.Write(mMeowCount);
	stream.Write(mName);
	//no solution for mMiceIndices yet 
}

void RoboCat::Deserialize(Serialization::Stream::InputMemoryStream& stream)
{
	stream.Read(mHealth);
	stream.Read(mMeowCount);
	stream.Read(mName);
	//no solution for mMiceIndices yet 
}

void RoboCat::Serialize(Serialization::Stream::OutputMemoryBitStream& stream)
{
	stream.Write(dummyB, 1);
	stream.Write(dummyVal32);
	stream.Write(dummyVal64);

	/*
	stream.Write(mHealth, 32);
	stream.Write(mMeowCount, 32);
	*/
	//stream.Write(mName);
	//no solution for mMiceIndices yet 
}

void RoboCat::SerializeAlt(Serialization::Stream::OutputMemoryBitStream& stream)
{
	stream.Write<bool>(dummyB);
	stream.Write(dummyVal32);
	stream.Write(dummyVal64);

	/*
	stream.WriteAlt(mHealth, 32);
	stream.WriteAlt(mMeowCount, 32);
	*/
	//stream.Write(mName);
	//no solution for mMiceIndices yet 
}

#define SERIALIZE_VALUES				\
	stream.Write<bool, 1>(dummyB);		\
	stream.Write<bool>(dummyB);			\
	stream.Write(dummyVal32);			\
	stream.Write(dummyVal64);			

void RoboCat::SerializeAlt(Serialization::Stream::OutputMemoryBitStream2& stream)
{
	SERIALIZE_VALUES
}

void RoboCat::SerializeAlt(Serialization::Stream::OutputMemoryBitStream21& stream)
{
	SERIALIZE_VALUES
}

void RoboCat::SerializeAlt(Serialization::Stream::OutputMemoryBitStream3& stream)
{
	SERIALIZE_VALUES
}

void RoboCat::SerializeAlt(Serialization::Stream::OutputMemoryBitStream4& stream)
{
	SERIALIZE_VALUES
}

void RoboCat::SerializeAlt(Serialization::Stream::OutputMemoryBitStream5& stream)
{
	SERIALIZE_VALUES
}

void RoboCat::SerializeAlt(Serialization::Stream::OutputMemoryBitStream6& stream)
{
	SERIALIZE_VALUES
}

void RoboCat::SerializeAlt(Serialization::Stream::OutputMemoryBitStream61& stream)
{
	SERIALIZE_VALUES
}
