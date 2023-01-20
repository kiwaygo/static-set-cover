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

namespace bool_pack_impl {
template <std::size_t tN, std::size_t... tIndices>
auto falsePack(std::index_sequence<tIndices...>)
    -> BoolPack<(tIndices, false)...>;
}

template <std::size_t tN>
auto falsePack()
    -> decltype(bool_pack_impl::falsePack<tN>(std::make_index_sequence<tN>{}));

namespace bool_pack_impl {
template <std::size_t tN, std::size_t... tIndices>
auto truePack(std::index_sequence<tIndices...>)
    -> BoolPack<(tIndices, true)...>;
}

template <std::size_t tN>
auto truePack()
    -> decltype(bool_pack_impl::truePack<tN>(std::make_index_sequence<tN>{}));

namespace bool_pack_impl {
template <std::size_t tIndex, bool... tBools, std::size_t... tIndices>
auto setIndex(BoolPack<tBools...>, std::index_sequence<tIndices...>)
    -> BoolPack<(tIndices == tIndex ? true : tBools)...>;
}

template <std::size_t tIndex, bool... tBools>
auto setIndex(BoolPack<tBools...>) -> decltype(bool_pack_impl::setIndex<tIndex>(
    BoolPack<tBools...>{}, std::make_index_sequence<sizeof...(tBools)>{}));

namespace bool_pack_impl {
template <std::size_t tIndex, bool... tBools, std::size_t... tIndices>
auto unsetIndex(BoolPack<tBools...>, std::index_sequence<tIndices...>)
    -> BoolPack<(tIndices == tIndex ? false : tBools)...>;
}

template <std::size_t tIndex, bool... tBools>
auto unsetIndex(BoolPack<tBools...>)
    -> decltype(bool_pack_impl::unsetIndex<tIndex>(
        BoolPack<tBools...>{}, std::make_index_sequence<sizeof...(tBools)>{}));

} // namespace set_cover
