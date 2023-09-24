#pragma once

#include <cstddef>
#include <type_traits>

namespace infd {
	namespace detail {
        /**
         * Backing implementation for a single leaf within a Tuple.
         * @tparam Index Index of this element within the tuple
         * @tparam T Type of this element
         */
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

        /**
         * Backing implementation of Tuple, with element indexes baked into the templates.
         *
         * @tparam Is Variadic indexes of Tuple elements
         * @tparam Ts Variadic types of Tuple elements
         */
		template <std::size_t ...Is, typename ...Ts>
		class TupleImpl<std::index_sequence<Is...>, Ts...> : 
			public TupleLeaf<Is, Ts>... {

		public:
			constexpr TupleImpl() : TupleLeaf<Is, Ts>{}... {}

			template <typename ...Args>
			constexpr TupleImpl(Args &&...args) : TupleLeaf<Is, Ts>{std::forward<Args>(args)}... {}
		};
	}

    /**
     * A const-length type-safe implementation of a Tuple for easy groupings of related data types.
     *
     * @tparam Ts Variadic types of Tuple elements
     */
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
