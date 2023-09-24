#pragma once

#include <type_traits>

/**
 * Backing implementation for variadic Tuple type.
 *
 * @tparam index Index of element within tuple
 * @tparam T Type of this tuple frame
 * @tparam Args Types of following tuple frames
 */
template <std::size_t index, typename T = void, typename... Args>
class TupleImpl : public TupleImpl<index+1, Args...> {
protected:
    T _contents;

    template <std::size_t indexST, typename ...argsST>
    static TupleImpl<indexST, argsST...>* tuple_shift(TupleImpl<indexST, argsST...> *tuple) {
        return tuple;
    }
public:
    template<std::size_t Index>
    auto& at() {
        return std::remove_reference_t<decltype(*tuple_shift<Index>(this))>::_contents;
    }
};

/**
 * Backing implementation for zero args case.
 * @tparam index Unused
 */
template<std::size_t index>
class TupleImpl<index, void> {
};

// Alias the tuple type to hide the index argument
template<typename... Args>
using Tuple = TupleImpl<0, Args...>;