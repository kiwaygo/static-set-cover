#pragma once

#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>

#include "TypeSet.h"

namespace set_cover {

class GreedyTag {};

template <typename tTiePolicy> class Greedy : public GreedyTag {
public:
  using TiePolicy = tTiePolicy;
};

class Left;
class Right;

class FirstOneWins {
public:
  template <typename tLeft, typename tRight> using Pick = Left;
};

class LastOneWins {
public:
  template <typename tLeft, typename tRight> using Pick = Right;
};

class TightestOneWins {
public:
  template <typename tLeft, typename tRight>
  using Pick = std::conditional_t<size<tLeft>() <= size<tRight>(), Left, Right>;
};

class LoosestOneWins {
public:
  template <typename tLeft, typename tRight>
  using Pick = std::conditional_t<size<tLeft>() >= size<tRight>(), Left, Right>;
};

using Arbitrary = FirstOneWins;

template <typename tAlgorithm, typename = void> class MinSetCover;

template <typename tAlgorithm>
class MinSetCover<tAlgorithm,
                  std::enable_if_t<std::is_base_of_v<GreedyTag, tAlgorithm>>> {

  template <std::size_t tI, typename tSet, typename... tCandidates>
  static constexpr auto winner() {
    static_assert(tI < sizeof...(tCandidates));
    using CandidatesTuple = std::tuple<tCandidates...>;
    using IthCandidate = std::tuple_element_t<tI, CandidatesTuple>;
    if constexpr (tI == sizeof...(tCandidates) - 1) {
      return IthCandidate{};
    } else {
      using CurrentWinner = decltype(winner<tI + 1, tSet, tCandidates...>());
      if constexpr (commonality<tSet, IthCandidate>() ==
                    commonality<tSet, CurrentWinner>()) {
        if constexpr (std::is_same_v<
                          typename tAlgorithm::TiePolicy::template Pick<
                              IthCandidate, CurrentWinner>,
                          Left>) {
          return IthCandidate{};
        } else {
          return CurrentWinner{};
        }
      } else if constexpr (commonality<tSet, IthCandidate>() >
                           commonality<tSet, CurrentWinner>()) {
        return IthCandidate{};
      } else {
        return CurrentWinner{};
      }
    }
  }

  template <typename tSet, typename... tCandidates>
  static constexpr auto winner() {
    static_assert(sizeof...(tCandidates) != 0);
    return winner<0, tSet, tCandidates...>();
  }

public:
  template <typename tSet, typename... tCandidates>
  static constexpr auto eval() {
    if constexpr (tSet() == 0) {
      return std::tuple<>{};
    } else {
      using Winner = decltype(winner<tSet, tCandidates...>());
      using RemainderSet =
          std::integral_constant<std::size_t, tSet() & (~Winner())>;
      return std::tuple_cat(std::tuple<Winner>{},
                            eval<RemainderSet, tCandidates...>());
    }
  }
};

} // namespace set_cover
