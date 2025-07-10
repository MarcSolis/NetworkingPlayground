#include "Tools/Profiler/Timer.h"

#include <iostream>


namespace profiler 
{
	Timer::Timer(const bool maxResolution) : maxResolution(maxResolution)
	{
		const startTimePoint = std::chrono::high_resolution_clock::now();
	}

	void Timer::Stop()
	{
		const auto endTimePoint = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::micro> duration = endTimePoint - startTimePoint; // end - start;

		if (maxResolution)
		{
			std::cout << duration.count() << "us" << std::endl;
		}
		else
		{
			duration /= 1000;
			std::cout << duration.count() << "ms" << std::endl;
		}
	}

	Timer::~Timer()
	{
		Stop();
	}
}