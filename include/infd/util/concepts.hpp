#pragma once

#include <concepts>
#include <ranges>

namespace infd::util {
	template <typename Range, typename ElementType>
	concept random_access_range_of = 
		std::ranges::random_access_range<Range>
	 && requires (Range r) {
		 { r[0] } -> std::convertible_to<ElementType&>;
	 };
}
