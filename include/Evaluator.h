#pragma once

#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>

#include "Impl.h"
#include "TupleUtil.h"
#include "TypeMap.h"
#include "TypeSet.h"

namespace set_cover {

template <typename... tEvaluables>
auto makeEvalType(Impl, std::tuple<tEvaluables...>)
    -> std::tuple<typename tEvaluables::Type...>;

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

template <typename tUniverse, typename... tCovers> struct Evaluator {

  template <typename tCover>
  using EvalSet = decltype(toSet(typename tCover::EvalList{}));

  using CoverByEvalSet = Map<MapItem<EvalSet<tCovers>, tCovers>...>;

protected:
  template <typename tEvalSet, typename... tArgs>
  static auto sparseEval(tArgs &&...aArgs)
      -> decltype(makeEvalType(Impl{}, typename tUniverse::AsTuple{})) {
    if constexpr (tEvalSet() == 0) {
      return {};
    } else {
      using BestEvalSet =
          decltype(argmaxAlignment<tEvalSet, EvalSet<tCovers>...>());
      using BestCover = typename CoverByEvalSet::template Find<BestEvalSet>;
      auto src = BestCover()(std::forward<tArgs>(aArgs)...);
      using RemainderEvalSet =
          std::integral_constant<std::size_t, tEvalSet() & (~BestEvalSet())>;
      auto tgt = sparseEval<RemainderEvalSet>(std::forward<tArgs>(aArgs)...);
      replace(tgt, src,
              std::make_index_sequence<std::tuple_size_v<decltype(src)>>{},
              typename BestCover::EvalList{});
      return tgt;
    }
  }

public:
  template <typename... tEvaluables, typename... Args>
  static auto eval(Args &&...aArgs) {
    auto resultTuple =
        sparseEval<typename tUniverse::template Set<tEvaluables...>>(
            std::forward<Args>(aArgs)...);
    using QueryOrder = typename tUniverse::template KPerm<tEvaluables...>;
    return reorder(resultTuple, QueryOrder{});
  }
};

} // namespace set_cover
