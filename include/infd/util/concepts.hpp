#pragma once

#include <concepts>
#include <ranges>
#include <type_traits>

namespace infd::util {
	template <typename Range, typename ElementType>
	concept random_access_range_of = 
		std::ranges::random_access_range<Range>
	 && requires (Range r) {
		 { r[0] } -> std::same_as<ElementType>;
	 };

	template <typename Range, typename ElementType>
	concept random_access_view_of = 
		random_access_range_of<Range, ElementType>
	 && std::ranges::view<Range>;

	template <typename Range, typename ElementType>
	concept random_access_move_only_range_of = 
		random_access_range_of<Range, ElementType>
	 && std::is_move_constructible_v<Range>
	 && std::is_move_assignable_v<Range>
	 && !std::is_copy_constructible_v<Range>
	 && !std::is_copy_assignable_v<Range>;
}
