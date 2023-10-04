#pragma once

// std
#include <chrono>

// bullet
#include <LinearMath/btScalar.h>

namespace infd::scene {
	using Float = btScalar;
	using Clock = std::chrono::steady_clock;
	using TimePoint = Clock::time_point;
	using Duration = std::chrono::microseconds;
} // namespace infd::scene
