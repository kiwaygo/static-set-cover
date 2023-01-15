#pragma once

#include <cstddef>
#include <limits>
#include <tuple>
#include <type_traits>
#include <utility>

#include "BoolPack.h"
#include "Impl.h"
#include "IndexSequenceUtil.h"

namespace set_cover {

template <std::size_t tI, typename... tElements>
constexpr bool hasRepeats(Impl) {
  if constexpr (sizeof...(tElements) == tI) {
    return false;
  } else {
    using Ith = std::tuple_element<tI, std::tuple<tElements...>>;
    using SameAsIth = BoolPack<std::is_same_v<Ith, tElements>...>;
    using IsNotIth = decltype(unsetIndex<tI>(truePack<sizeof...(tElements)>()));
    if constexpr (decltype(anyOf(andPack(SameAsIth{}, IsNotIth{}))){}) {
      return true;
    } else {
      return hasRepeats<tI + 1, tElements...>(Impl{});
    }
  }
}

template <typename... tElements> constexpr bool hasRepeats() {
  return hasRepeats<0, tElements...>(Impl{});
};

template <std::size_t tI, typename tElement, typename... tElements>
constexpr std::size_t flag(Impl) {
  if constexpr (sizeof...(tElements) == tI) {
    return 0;
  } else if (std::is_same_v<std::tuple_element_t<tI, std::tuple<tElements...>>,
                            tElement>) {
    return (flag<tI + 1, tElement, tElements...>(Impl{}) << 1) + 1;
  } else {
    return flag<tI + 1, tElement, tElements...>(Impl{}) << 1;
  }
}

template <typename tElement, typename... tElements>
constexpr std::size_t flag() {
  static_assert(hasRepeats<tElements...>() == false);
  return flag<0, tElement, tElements...>(Impl{});
};

template <std::size_t tI, typename tElement, typename... tElements>
constexpr std::size_t flagIndex(Impl) {
  if constexpr (sizeof...(tElements) == tI) {
    return std::numeric_limits<std::size_t>::max();
  } else if (std::is_same_v<std::tuple_element_t<tI, std::tuple<tElements...>>,
                            tElement>) {
    return tI;
  } else {
    return flagIndex<tI + 1, tElement, tElements...>(Impl{});
  }
}

template <typename tElement, typename... tElements>
constexpr std::size_t flagIndex() {
  static_assert(hasRepeats<tElements...>() == false);
  return flagIndex<0, tElement, tElements...>(Impl{});
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
  while (difference) {
    difference += difference & 1;
    difference >>= 1;
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

template <std::size_t tI, typename tSet, typename... tCandidateSets>
constexpr std::size_t argmaxAlignment(Impl) {
  static_assert(tI < sizeof...(tCandidateSets));
  if constexpr (tI == sizeof...(tCandidateSets) - 1) {
    return tI;
  } else {
    constexpr std::size_t currentMax =
        argmaxAlignment<tI + 1, tSet, tCandidateSets...>(Impl{});
    using IthCandidate =
        std::tuple_element_t<tI, std::tuple<tCandidateSets...>>;
    using CurrentMaxCandidate =
        std::tuple_element_t<currentMax, std::tuple<tCandidateSets...>>;
    if constexpr (commonality<tSet, IthCandidate>() ==
                  commonality<tSet, CurrentMaxCandidate>()) {
      return difference<tSet, IthCandidate>() <=
                     commonality<tSet, CurrentMaxCandidate>()
                 ? tI
                 : currentMax;
    } else if constexpr (commonality<tSet, IthCandidate>() >=
                         commonality<tSet, CurrentMaxCandidate>()) {
      return tI;
    } else {
      return currentMax;
    }
  }
}

template <typename tSet, typename... tCandidateSets>
constexpr auto argmaxAlignment() {
  static_assert(sizeof...(tCandidateSets) != 0);
  return std::tuple_element_t<argmaxAlignment<0, tSet, tCandidateSets...>(
                                  Impl{}),
                              std::tuple<tCandidateSets...>>{};
}

} // namespace set_cover
