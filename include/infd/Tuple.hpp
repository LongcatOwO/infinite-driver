#pragma once

#include <type_traits>

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

template<std::size_t index>
class TupleImpl<index, void> {
};

template<typename... Args>
using Tuple = TupleImpl<0, Args...>;