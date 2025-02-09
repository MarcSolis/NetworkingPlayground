#pragma once
#include <cstdint>
#include <vector>

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
protected:
	virtual void Update() = 0;
};

class RoboCat : Object
{
public:
	RoboCat();
	RoboCat(int32_t health, int32_t meowCount);

	void SetName(const char* name, size_t length);
	void CatchMices(std::vector<int32_t> miceIndices);

	bool operator==(const RoboCat& other);
	bool operator!=(const RoboCat& other);

protected:
	virtual void Update() override {};

private:
	static constexpr uint32_t BUFFER_SIZE = 64;
	int32_t mHealth;
	int32_t mMeowCount;
	char mName[BUFFER_SIZE]{"Unknown"};
	std::vector<int32_t> mMiceIndices;
};
