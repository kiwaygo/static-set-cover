#pragma once

#include <cstddef>
#include <tuple>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <unordered_set>
#include <utility>

#include "MinSetCover.h"
#include "TupleUtil.h"
#include "TypeMap.h"
#include "TypeSet.h"

namespace set_cover {

using GreedyMinSetCover = MinSetCover<Greedy<TightestOneWins>>;

namespace evaluator_impl {
template <typename... tEvaluables>
auto makeEvalType(std::tuple<tEvaluables...>)
    -> std::tuple<typename tEvaluables::Type...>;
}

class LogNothing {
public:
  using Log = void;
  void clearLog() {}
  void log(const std::type_info &aFunctor) {}
};

class LogTypeIndex {
public:
  using Log = std::unordered_set<std::type_index>;
  void clearLog() { mLog.clear(); }
  void log(const std::type_info &aFunctor) {
    mLog.insert(std::type_index(aFunctor));
  }
  Log getLog() const { return mLog; }

private:
  Log mLog;
};

template <typename tUniverse, typename tLogPolicy, typename... tFunctors>
struct Evaluator : public tLogPolicy {

  template <typename tFunctor>
  using EvalSet = decltype(toSet(typename tFunctor::EvalList{}));

  using FunctorByEvalSet = Map<MapItem<EvalSet<tFunctors>, tFunctors>...>;

protected:
  template <size_t tI, typename tMinSetCover, typename... tArgs>
  auto sparseEval(tArgs &&...aArgs)
      -> decltype(evaluator_impl::makeEvalType(typename tUniverse::AsTuple{})) {
    if constexpr (tI == std::tuple_size_v<tMinSetCover>) {
      return {};
    } else {
      using IthFunctor = typename FunctorByEvalSet::template Find<
          std::tuple_element_t<tI, tMinSetCover>>;
      auto src = IthFunctor{}(std::forward<tArgs>(aArgs)...);
      this->log(typeid(IthFunctor));
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
  auto eval(Args &&...aArgs) {
    using MyEvalSet = typename tUniverse::template Set<tEvaluables...>;
    using MyMinSetCover =
        decltype(GreedyMinSetCover::eval<MyEvalSet, EvalSet<tFunctors>...>());
    this->clearLog();
    auto resultTuple =
        sparseEval<0, MyMinSetCover>(std::forward<Args>(aArgs)...);
    using QueryOrder = typename tUniverse::template KPerm<tEvaluables...>;
    return reorder(resultTuple, QueryOrder{});
  }
};

} // namespace set_cover
