#pragma once

#include <vector>

// bullet
#include <LinearMath/btAlignedAllocator.h>

// project - util
#include <infd/util/handle_vector.hpp>

namespace infd::util {
	template <typename T>
	using aligned_vector = std::vector<T, btAlignedAllocator<T, 16>>;

	template <typename T>
	using aligned_handle_vector = handle_vector<T, btAlignedAllocator<T, 16>>;
}
