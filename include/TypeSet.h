#pragma once

#include <cstddef>
#include <limits>
#include <tuple>
#include <type_traits>
#include <utility>

#include "BoolPack.h"
#include "IndexSequenceUtil.h"

namespace set_cover {

namespace type_set_impl {

template <std::size_t tI, typename... tElements> constexpr bool hasRepeats() {
  if constexpr (sizeof...(tElements) == tI) {
    return false;
  } else {
    using Ith = std::tuple_element<tI, std::tuple<tElements...>>;
    using SameAsIth = BoolPack<std::is_same_v<Ith, tElements>...>;
    using IsNotIth = decltype(unsetIndex<tI>(truePack<sizeof...(tElements)>()));
    if constexpr (decltype(anyOf(andPack(SameAsIth{}, IsNotIth{}))){}) {
      return true;
    } else {
      return hasRepeats<tI + 1, tElements...>();
    }
  }
}

} // namespace type_set_impl

template <typename... tElements> constexpr bool hasRepeats() {
  return type_set_impl::hasRepeats<0, tElements...>();
};

namespace type_set_impl {

template <std::size_t tI, typename tElement, typename... tElements>
constexpr std::size_t flag() {
  if constexpr (sizeof...(tElements) == tI) {
    return 0;
  } else if (std::is_same_v<std::tuple_element_t<tI, std::tuple<tElements...>>,
                            tElement>) {
    return (flag<tI + 1, tElement, tElements...>() << 1) + 1;
  } else {
    return flag<tI + 1, tElement, tElements...>() << 1;
  }
}

} // namespace type_set_impl

template <typename tElement, typename... tElements>
constexpr std::size_t flag() {
  static_assert(hasRepeats<tElements...>() == false);
  return type_set_impl::flag<0, tElement, tElements...>();
};

namespace type_set_impl {

template <std::size_t tI, typename tElement, typename... tElements>
constexpr std::size_t flagIndex() {
  if constexpr (sizeof...(tElements) == tI) {
    return std::numeric_limits<std::size_t>::max();
  } else if (std::is_same_v<std::tuple_element_t<tI, std::tuple<tElements...>>,
                            tElement>) {
    return tI;
  } else {
    return flagIndex<tI + 1, tElement, tElements...>();
  }
}

} // namespace type_set_impl

template <typename tElement, typename... tElements>
constexpr std::size_t flagIndex() {
  static_assert(hasRepeats<tElements...>() == false);
  return type_set_impl::flagIndex<0, tElement, tElements...>();
}

template <typename tSet> constexpr std::size_t size() {
  std::size_t bits = tSet();
  std::size_t size = 0;
  while (bits) {
    size += bits & 1;
    bits >>= 1;
  }
  return size;
}

template <typename tSet0, typename tSet1> constexpr std::size_t commonality() {
  std::size_t intersection = tSet0() & tSet1();
  std::size_t commonality = 0;
  while (intersection) {
    commonality += intersection & 1;
    intersection >>= 1;
  }
  return commonality;
}

template <typename tSet0, typename tSet1> constexpr std::size_t difference() {
  std::size_t exclusiveOr = tSet0() ^ tSet1();
  std::size_t difference = 0;
  while (exclusiveOr) {
    difference += exclusiveOr & 1;
    exclusiveOr >>= 1;
  }
  return difference;
}

struct NotList;

template <std::size_t... tFlagIndices>
constexpr auto toSet(std::index_sequence<tFlagIndices...>)
    -> std::integral_constant<size_t, (... | (1 << tFlagIndices))>;

template <typename tSet> constexpr auto toCanonicalList() {
  if constexpr (tSet() == 0) {
    return std::index_sequence<>();
  } else if (tSet() & 1) {
    return prepend<0>(add<1>(toCanonicalList<(tSet() >> 1)>()));
  } else {
    return add<1>(toCanonicalList<(tSet() >> 1)>());
  }
}

// Universe defines a canonical order over elements.
template <typename... tElements> struct Universe {
  static_assert(hasRepeats<tElements...>() == false);

  // A set of elements is represented as bitset indexed in the canonical order.
  template <typename... tSetElements>
  using Set =
      std::integral_constant<std::size_t,
                             (... | flag<tSetElements, tElements...>())>;

  // A list of elements is represented as a sequence of flag indices.
  template <typename... tListElements>
  using List = std::index_sequence<flagIndex<tListElements, tElements...>()...>;

  // A k-permutation of elements is a list that ensures no item duplication.
  template <typename... tKPermElements>
  using KPerm = std::conditional_t<hasRepeats<tKPermElements...>(), NotList,
                                   List<tKPermElements...>>;

  using AsTuple = std::tuple<tElements...>;
};

} // namespace set_cover
