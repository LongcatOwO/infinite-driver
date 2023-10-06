#pragma once

#include <concepts>
#include <ranges>
#include <type_traits>
#include <utility>

namespace infd::util {

	template <typename Range, typename ElementType>
	concept range_of = 
		std::ranges::range<Range>
	 && requires (Range r) {
		 { *r.begin() } -> std::same_as<ElementType>;
	 };

	template <typename Range, typename ElementType>
	concept view_of = 
		range_of<Range, ElementType>
	 && std::ranges::view<Range>;

	template <typename Range, typename ElementType>
	concept random_access_range_of = 
		std::ranges::random_access_range<Range>
	 && range_of<Range, ElementType>;

	template <typename Range, typename ElementType>
	concept random_access_view_of = 
		random_access_range_of<Range, ElementType>
	 && view_of<Range, ElementType>;

	template <typename Range, typename ElementType>
	concept random_access_move_only_range_of = 
		random_access_range_of<Range, ElementType>
	 && std::is_move_constructible_v<Range>
	 && std::is_move_assignable_v<Range>
	 && !std::is_copy_constructible_v<Range>
	 && !std::is_copy_assignable_v<Range>;

	template <typename Fn, typename ...Args>
	concept consumer_of = requires (Fn &&fn, Args &&...args) {
		std::forward<Fn>(fn)(std::forward<Args>(args)...);
	};

	template <typename Fn, typename R, typename ...Args>
	concept function_of = 
		consumer_of<Fn, Args...>
	 && requires (Fn &&fn, Args &&...args) {
			{ std::forward<Fn>(fn)(std::forward<Args>(args)...) } -> std::convertible_to<R>;
		};

} // namespace infd::util
