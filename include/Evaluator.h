#pragma once

#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>

#include "Impl.h"
#include "MinSetCover.h"
#include "TupleUtil.h"
#include "TypeMap.h"
#include "TypeSet.h"

namespace set_cover {

using GreedyMinSetCover = MinSetCover<Greedy<TightestOneWins>>;

template <typename... tEvaluables>
auto makeEvalType(Impl, std::tuple<tEvaluables...>)
    -> std::tuple<typename tEvaluables::Type...>;

template <typename tUniverse, typename... tFunctors> struct Evaluator {

  template <typename tFunctor>
  using EvalSet = decltype(toSet(typename tFunctor::EvalList{}));

  using FunctorByEvalSet = Map<MapItem<EvalSet<tFunctors>, tFunctors>...>;

protected:
  template <size_t tI, typename tMinSetCover, typename... tArgs>
  static auto sparseEval(tArgs &&...aArgs)
      -> decltype(makeEvalType(Impl{}, typename tUniverse::AsTuple{})) {
    if constexpr (tI == std::tuple_size_v<tMinSetCover>) {
      return {};
    } else {
      using IthFunctor = typename FunctorByEvalSet::template Find<
          std::tuple_element_t<tI, tMinSetCover>>;
      auto src = IthFunctor{}(std::forward<tArgs>(aArgs)...);
      auto tgt =
          sparseEval<tI + 1, tMinSetCover>(std::forward<tArgs>(aArgs)...);
      replace(tgt, src,
              std::make_index_sequence<std::tuple_size_v<decltype(src)>>{},
              typename IthFunctor::EvalList{});
      return tgt;
    }
  }

public:
  template <typename... tEvaluables, typename... Args>
  static auto eval(Args &&...aArgs) {
    using MyEvalSet = typename tUniverse::template Set<tEvaluables...>;
    using MyMinSetCover =
        decltype(GreedyMinSetCover::eval<MyEvalSet, EvalSet<tFunctors>...>());
    auto resultTuple =
        sparseEval<0, MyMinSetCover>(std::forward<Args>(aArgs)...);
    using QueryOrder = typename tUniverse::template KPerm<tEvaluables...>;
    return reorder(resultTuple, QueryOrder{});
  }
};

} // namespace set_cover
