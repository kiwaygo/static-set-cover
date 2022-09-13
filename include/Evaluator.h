#pragma once

#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>

#include "TupleUtil.h"
#include "TypeMap.h"
#include "TypeSet.h"

template <typename... tEvaluables>
auto makeEvalType(std::tuple<tEvaluables...>)
    -> std::tuple<typename tEvaluables::Type...>;

template <typename tUniverse, typename... tCovers> struct Evaluator {
  using Universe = tUniverse;

  template <typename tCover>
  static constexpr std::size_t sEvalSet = toSet(typename tCover::EvalList{});

  template <typename tCover>
  using EvalSet = std::integral_constant<std::size_t, sEvalSet<tCover>>;

  using CoverByEvalSet = Map<MapItem<EvalSet<tCovers>, tCovers>...>;

protected:
  template <std::size_t tEvalSet, typename... tArgs>
  static auto sparseEval(tArgs &&...aArgs) {
    if constexpr (tEvalSet == 0) {
      return decltype(makeEvalType(typename Universe::AsTuple{})){};
    } else {
      constexpr std::size_t bestEvalSet =
          argmaxCommonality<tEvalSet, sEvalSet<tCovers>...>();
      using BestCover = typename CoverByEvalSet::template Find<
          std::integral_constant<std::size_t, bestEvalSet>>;
      auto src = BestCover()(std::forward<tArgs>(aArgs)...);
      auto tgt =
          sparseEval<tEvalSet &(~bestEvalSet)>(std::forward<tArgs>(aArgs)...);
      replace(tgt, src,
              std::make_index_sequence<std::tuple_size_v<decltype(src)>>{},
              typename BestCover::EvalList{});
      return tgt;
    }
  }

public:
  template <typename... tEvaluables, typename... Args>
  static auto eval(Args &&...aArgs) {
    constexpr std::size_t set = Universe::template sSet<tEvaluables...>;
    auto resultTuple = sparseEval<set>(std::forward<Args>(aArgs)...);
    using QueryOrder = typename Universe::template KPerm<tEvaluables...>;
    return reorder(resultTuple, QueryOrder{});
  }
};
