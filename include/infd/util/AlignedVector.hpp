#pragma once

#include <vector>

// bullet
#include <LinearMath/btAlignedAllocator.h>

namespace infd::util {
	template <typename T>
	using AlignedVector = std::vector<T, btAlignedAllocator<T, 16>>;
}
