#pragma once

#include <cstddef>
#include <limits>
#include <tuple>
#include <type_traits>
#include <utility>

#include "BoolPack.h"
#include "IndexSequenceUtil.h"

namespace set_cover {

template <std::size_t tI, typename... tElements>
constexpr bool hasRepeatsImpl() {
  if constexpr (sizeof...(tElements) == tI) {
    return false;
  } else {
    using Ith = std::tuple_element<tI, std::tuple<tElements...>>;
    using SameAsIth = BoolPack<std::is_same_v<Ith, tElements>...>;
    using IsNotIth = decltype(unsetIndex<tI>(truePack<sizeof...(tElements)>()));
    if constexpr (decltype(allOf(andPack(SameAsIth{}, IsNotIth{}))){}) {
      return true;
    } else {
      return hasRepeatsImpl<tI + 1, tElements...>();
    }
  }
}

template <typename... tElements> constexpr bool hasRepeats() {
  return hasRepeatsImpl<0, tElements...>();
};

template <std::size_t tI, typename tElement, typename... tElements>
constexpr std::size_t flagImpl() {
  if constexpr (sizeof...(tElements) == tI) {
    return 0;
  } else if (std::is_same_v<std::tuple_element_t<tI, std::tuple<tElements...>>,
                            tElement>) {
    return (flagImpl<tI + 1, tElement, tElements...>() << 1) + 1;
  } else {
    return flagImpl<tI + 1, tElement, tElements...>() << 1;
  }
}

template <typename tElement, typename... tElements>
constexpr std::size_t flag() {
  static_assert(hasRepeats<tElements...>() == false);
  return flagImpl<0, tElement, tElements...>();
};

template <std::size_t tI, typename tElement, typename... tElements>
constexpr std::size_t flagIndexImpl() {
  if constexpr (sizeof...(tElements) == tI) {
    return std::numeric_limits<std::size_t>::max();
  } else if (std::is_same_v<std::tuple_element_t<tI, std::tuple<tElements...>>,
                            tElement>) {
    return tI;
  } else {
    return flagIndexImpl<tI + 1, tElement, tElements...>();
  }
}

template <typename tElement, typename... tElements>
constexpr std::size_t flagIndex() {
  static_assert(hasRepeats<tElements...>() == false);
  return flagIndexImpl<0, tElement, tElements...>();
}

constexpr std::size_t commonality(const std::size_t aSet0,
                                  const std::size_t aSet1) {
  std::size_t intersection = aSet0 & aSet1;
  std::size_t commonality = 0;
  while (intersection) {
    commonality += intersection & 1;
    intersection >>= 1;
  }
  return commonality;
}

struct NotList;

template <std::size_t... tFlagIndices>
constexpr std::size_t toSet(std::index_sequence<tFlagIndices...>) {
  return (... | (1 << tFlagIndices));
}

template <std::size_t tSet> constexpr auto toCanonicalList() {
  if constexpr (tSet == 0) {
    return std::index_sequence<>();
  } else if (tSet & 1) {
    return prepend<0>(add<1>(toCanonicalList<(tSet >> 1)>()));
  } else {
    return add<1>(toCanonicalList<(tSet >> 1)>());
  }
}

// Universe defines a canonical order over elements.
template <typename... tElements> struct Universe {
  static_assert(hasRepeats<tElements...>() == false);

  // A set of elements is represented as bitset indexed in the canonical order.
  template <typename... tSetElements>
  static constexpr std::size_t sSet = (... |
                                       flag<tSetElements, tElements...>());

  // A list of elements is represented as a sequence of flag indices.
  template <typename... tListElements>
  using List = std::index_sequence<flagIndex<tListElements, tElements...>()...>;

  // A k-permutation of elements is a list that ensures no item duplication.
  template <typename... tKPermElements>
  using KPerm = std::conditional_t<hasRepeats<tKPermElements...>(), NotList,
                                   List<tKPermElements...>>;

  using AsTuple = std::tuple<tElements...>;
};

template <std::size_t tSet, std::size_t... tCandidateSets>
constexpr std::size_t argmaxCommonality() {
  static_assert(sizeof...(tCandidateSets) != 0);
  std::size_t currentMax = 0, pick = 0;
  for (const std::size_t candidateSet : {tCandidateSets...}) {
    if (commonality(tSet, candidateSet) > currentMax) {
      currentMax = commonality(tSet, candidateSet);
      pick = candidateSet;
    }
  }
  return pick;
}

}