// Copyright (c) 2023 Marc Solis Serrano

#pragma once
#include <chrono>
#include <iostream>

namespace profiler 
{
	
	template <typename T>
	concept timer_resolution = requires (T)
	{
		std::is_same_v<T, std::nano>	||		// ratio<1, 1000000000>
		std::is_same_v<T, std::micro>	||		// ratio<1, 1000000>
		std::is_same_v<T, std::milli>	||		// ratio<1, 1000>
		std::is_same_v<T, std::ratio<1, 1>>;
	};

	template <timer_resolution PrecisionT = std::micro>
	class Timer
	{
	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> startTimePoint;

		static constexpr uint16_t sDecimalPrecision = 2;
		static constexpr uint16_t sDecimalThreshold = 10;

	public:
		Timer();
		~Timer();

	private:
		void Stop() const;

		static inline constexpr auto PrintUnits();
	};


	template<timer_resolution PrecisionT>
	Timer<PrecisionT>::Timer()
	{
		startTimePoint = std::chrono::high_resolution_clock::now();
	}

	template<timer_resolution PrecisionT>
	void Timer<PrecisionT>::Stop() const
	{
		const auto endTimePoint = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, PrecisionT> duration = endTimePoint - startTimePoint;

		std::cout << std::fixed << std::setprecision(sDecimalPrecision * (duration.count() <= sDecimalThreshold)) << duration.count() << PrintUnits() << std::endl;
	}

	template<timer_resolution PrecisionT>
	Timer<PrecisionT>::~Timer()
	{
		Stop();
	}

	template<timer_resolution PrecisionT>
	inline constexpr auto Timer<PrecisionT>::PrintUnits()
	{
		if constexpr (std::is_same_v<PrecisionT, std::nano>)	// ratio<1, 1.000.000.000>
		{
			return "ns";
		}

		if constexpr (std::is_same_v<PrecisionT, std::micro>)	// ratio<1, 1.000.000>
		{
			return "us";
		}

		if constexpr (std::is_same_v<PrecisionT, std::milli>)	// ratio<1, 1.000>
		{
			return "ms";
		}

		if constexpr (std::is_same_v<PrecisionT, std::ratio<1, 1>>)
		{
			return "s";
		}
	}
}
