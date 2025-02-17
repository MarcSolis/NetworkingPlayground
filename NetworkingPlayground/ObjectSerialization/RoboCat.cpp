#include "RoboCat.h"
#include "OutputMemoryStream.h"
#include "InputMemoryStream.h"
#include <cassert>

#pragma region NaiveRoboCat
NaiveRoboCat::NaiveRoboCat() : mHealth(10), mMeowCount(3) 
{
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


RoboCat::RoboCat() : mHealth(10), mMeowCount(3)
{
}

RoboCat::RoboCat(int32_t health, int32_t meowCount) : mHealth(health), mMeowCount(meowCount)
{
}

void RoboCat::SetName(const char* name, size_t length)
{
	assert(length <= BUFFER_SIZE);

	memset(mName, 0, sizeof(mName));
	memcpy(mName, name, length);
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

