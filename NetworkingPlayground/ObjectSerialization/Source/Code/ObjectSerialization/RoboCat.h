#pragma once
#include "ObjectSerialization/ISerializableObject.h"
#include "ObjectSerialization/Streams/OutputMemoryBitStream2.h"

#include <cstdint>
#include <vector>


#define ONE_BIT 1

class NaiveRoboCat
{
public:
	NaiveRoboCat();
	NaiveRoboCat(int32_t mHealth, int32_t mMeowCount);

	bool operator==(const NaiveRoboCat& other);
	bool operator!=(const NaiveRoboCat& other);

private:
	int32_t mHealth;
	int32_t mMeowCount;
};


class Object
{
public:
	virtual ~Object() = default;

protected:
	virtual void Update() = 0;
};

class RoboCat : public Object, public Serialization::ISerializableObject
{
public:
	RoboCat() noexcept;
	RoboCat(int32_t health, int32_t meowCount) noexcept;

	void SetName(const char* name, size_t length);
	void CatchMices(std::vector<int32_t> miceIndices);

	bool operator==(const RoboCat& other);
	bool operator!=(const RoboCat& other);

	virtual void Serialize(Serialization::Stream::OutputMemoryBitStream& stream) override;
	void SerializeAlt(Serialization::Stream::OutputMemoryBitStream& stream);
	void SerializeAlt(Serialization::Stream::OutputMemoryBitStream2& stream);
	void SerializeAlt(Serialization::Stream::OutputMemoryBitStream3& stream);
	void SerializeAlt(Serialization::Stream::OutputMemoryBitStream4& stream);
	void SerializeAlt(Serialization::Stream::OutputMemoryBitStream5& stream);

protected:
	virtual void Update() override {};
	virtual void Serialize(Serialization::Stream::OutputMemoryStream& stream) override;
	virtual void Deserialize(Serialization::Stream::InputMemoryStream& stream) override;

	//virtual void Serialize(Serialization::Stream::OutputMemoryBitStream& stream) override;
	//virtual void Deserialize(Serialization::Stream::InputMemoryStream& stream) override;

	

private:
	static constexpr uint32_t BUFFER_SIZE = 16;
	int32_t mHealth;
	int32_t mMeowCount;
	char mName[BUFFER_SIZE]{"Unknown"};
	std::vector<int32_t> mMiceIndices;


	uint64_t dummyVal64{9876543210};
	uint32_t dummyVal32{123456789};
	uint16_t dummyVal16{5555};
	bool dummyB{true};
};
