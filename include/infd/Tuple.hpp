#pragma once

// #include <type_traits>

/**
 * Backing implementation for variadic Tuple type.
 *
 * @tparam index Index of element within tuple
 * @tparam T Type of this tuple frame
 * @tparam Args Types of following tuple frames
 */
// template <std::size_t index, typename T = void, typename... Args>
// class TupleImpl : public TupleImpl<index+1, Args...> {
// protected:
//     T _contents;
// 
//     template <std::size_t indexST, typename ...argsST>
//     static TupleImpl<indexST, argsST...>* tuple_shift(TupleImpl<indexST, argsST...> *tuple) {
//         return tuple;
//     }
// public:
//     template<std::size_t Index>
//     auto& at() {
//         return std::remove_reference_t<decltype(*tuple_shift<Index>(this))>::_contents;
//     }
// };

/**
 * Backing implementation for zero args case.
 * @tparam index Unused
 */
// template<std::size_t index>
// class TupleImpl<index, void> {
// };
// 
// // Alias the tuple type to hide the index argument
// template<typename... Args>
// using Tuple = TupleImpl<0, Args...>;


#include <cstddef>
#include <type_traits>


namespace infd {
	namespace detail {
		template <std::size_t Index, typename T>
		class TupleLeaf {
		public:
			constexpr TupleLeaf() : _content{} {}
			constexpr TupleLeaf(const T &content) : _content(content) {}
			constexpr TupleLeaf(T &&content) : _content(std::move(content)) {}
			T _content;
		};

		template <typename T>
		constexpr bool always_false = false;

		template <std::size_t Index, typename T>
		const TupleLeaf<Index, T>& tupleResolveLeafType(const TupleLeaf<Index, T> &) {
			static_assert(always_false<T>, "tupleResolveLeafType is not allowed in evaluated context!");
		}

		template <std::size_t Index, typename Tuple>
		using tuple_leaf_t = std::remove_cvref_t<decltype(tupleResolveLeafType<Index>(std::declval<Tuple>()))>;

		template <typename IndexSequence, typename ...Ts>
		class TupleImpl;

		template <std::size_t ...Is, typename ...Ts>
		class TupleImpl<std::index_sequence<Is...>, Ts...> : 
			public TupleLeaf<Is, Ts>... {

		public:
			constexpr TupleImpl() : TupleLeaf<Is, Ts>{}... {}

			template <typename ...Args>
			constexpr TupleImpl(Args &&...args) : TupleLeaf<Is, Ts>{std::forward<Args>(args)}... {}
		};
	}

	template <typename ...Ts>
	class Tuple : detail::TupleImpl<std::make_index_sequence<sizeof...(Ts)>, Ts...> {
		using base = detail::TupleImpl<std::make_index_sequence<sizeof...(Ts)>, Ts...>;
	public:
		constexpr Tuple() : base{} {}
		
		template <typename ...Args>
		constexpr Tuple(Args &&...args) : base{std::forward<Args>(args)...} {}

		template <std::size_t Index>
		constexpr auto& at() noexcept {
			return static_cast<detail::tuple_leaf_t<Index, Tuple> &>(*this)._content;
		}

		template <std::size_t Index>
		constexpr const auto& at() const noexcept {
			return static_cast<const detail::tuple_leaf_t<Index, Tuple> &>(*this)._content;
		}
	};
}
