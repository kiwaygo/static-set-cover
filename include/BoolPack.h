#pragma once

#include <cstddef>
#include <type_traits>
#include <utility>

namespace set_cover {

template <bool...> struct BoolPack {};

template <bool... tBools>
auto allOf(BoolPack<tBools...>)
    -> std::is_same<BoolPack<tBools..., true>, BoolPack<true, tBools...>>;

template <bool... tBools>
auto noneOf(BoolPack<tBools...>)
    -> std::is_same<BoolPack<tBools..., false>, BoolPack<false, tBools...>>;

template <bool... tBools>
auto anyOf(BoolPack<tBools...>)
    -> std::negation<decltype(noneOf(BoolPack<tBools...>{}))>;

template <bool... tBools0, bool... tBools1>
auto andPack(BoolPack<tBools0...>, BoolPack<tBools1...>)
    -> BoolPack<(tBools0 && tBools1)...>;

template <bool... tBools0, bool... tBools1>
auto orPack(BoolPack<tBools0...>, BoolPack<tBools1...>)
    -> BoolPack<(tBools0 || tBools1)...>;

template <std::size_t tN, std::size_t... tIndices>
auto falsePackImpl(std::index_sequence<tIndices...>)
    -> BoolPack<(tIndices, false)...>;

template <std::size_t tN>
auto falsePack() -> decltype(falsePackImpl<tN>(std::make_index_sequence<tN>{}));

template <std::size_t tN, std::size_t... tIndices>
auto truePackImpl(std::index_sequence<tIndices...>)
    -> BoolPack<(tIndices, true)...>;

template <std::size_t tN>
auto truePack() -> decltype(truePackImpl<tN>(std::make_index_sequence<tN>{}));

template <std::size_t tIndex, bool... tBools, std::size_t... tIndices>
auto setIndexImpl(BoolPack<tBools...>, std::index_sequence<tIndices...>)
    -> BoolPack<(tIndices == tIndex ? true : tBools)...>;

template <std::size_t tIndex, bool... tBools>
auto setIndex(BoolPack<tBools...>) -> decltype(setIndexImpl<tIndex>(
    BoolPack<tBools...>{}, std::make_index_sequence<sizeof...(tBools)>{}));

template <std::size_t tIndex, bool... tBools, std::size_t... tIndices>
auto unsetIndexImpl(BoolPack<tBools...>, std::index_sequence<tIndices...>)
    -> BoolPack<(tIndices == tIndex ? false : tBools)...>;

template <std::size_t tIndex, bool... tBools>
auto unsetIndex(BoolPack<tBools...>) -> decltype(unsetIndexImpl<tIndex>(
    BoolPack<tBools...>{}, std::make_index_sequence<sizeof...(tBools)>{}));

} // namespace set_cover
