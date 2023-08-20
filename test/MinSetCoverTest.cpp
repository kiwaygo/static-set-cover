#include <BoolPack.h>
#include <MinSetCover.h>
#include <gtest/gtest.h>

using namespace set_cover;

struct A {};
struct B {};
struct C {};
struct D {};
struct E {};

using U = Universe<A, B, C, D, E>;

using ABCDE = U::Set<A, B, C, D, E>;
using ABCD = U::Set<A, B, C, D>;
using AE = U::Set<A, E>;
using DE = U::Set<D, E>;

namespace {

template <typename tTuple, typename tElement, std::size_t... tIndices>
auto hasElementImpl(std::index_sequence<tIndices...>) -> decltype(anyOf(
    BoolPack<std::is_same_v<std::tuple_element_t<tIndices, tTuple>,
                            tElement>...>{}));

template <typename tTuple, typename tElement>
auto hasElement() -> decltype(hasElementImpl<tTuple, tElement>(
    std::make_index_sequence<std::tuple_size_v<tTuple>>{}));

} // namespace

TEST(MinSetCoverTest, GreedyAndFirstOneWinsTie) {
  using MyMinSetCover = MinSetCover<Greedy<FirstOneWins>>;
  {
    using Result = decltype(MyMinSetCover::eval<ABCDE, ABCD, AE, DE>());
    EXPECT_TRUE(decltype(hasElement<Result, ABCD>()){});
    EXPECT_TRUE(decltype(hasElement<Result, AE>()){});
    EXPECT_FALSE(decltype(hasElement<Result, DE>()){});
  }
  {
    using Result = decltype(MyMinSetCover::eval<ABCDE, ABCD, DE, AE>());
    EXPECT_TRUE(decltype(hasElement<Result, ABCD>()){});
    EXPECT_FALSE(decltype(hasElement<Result, AE>()){});
    EXPECT_TRUE(decltype(hasElement<Result, DE>()){});
  }
  {
    using Result = decltype(MyMinSetCover::eval<ABCDE, AE, ABCD, DE>());
    EXPECT_TRUE(decltype(hasElement<Result, ABCD>()){});
    EXPECT_TRUE(decltype(hasElement<Result, AE>()){});
    EXPECT_FALSE(decltype(hasElement<Result, DE>()){});
  }
  {
    using Result = decltype(MyMinSetCover::eval<ABCDE, AE, DE, ABCD>());
    EXPECT_TRUE(decltype(hasElement<Result, ABCD>()){});
    EXPECT_TRUE(decltype(hasElement<Result, AE>()){});
    EXPECT_FALSE(decltype(hasElement<Result, DE>()){});
  }
  {
    using Result = decltype(MyMinSetCover::eval<ABCDE, DE, ABCD, AE>());
    EXPECT_TRUE(decltype(hasElement<Result, ABCD>()){});
    EXPECT_FALSE(decltype(hasElement<Result, AE>()){});
    EXPECT_TRUE(decltype(hasElement<Result, DE>()){});
  }
  {
    using Result = decltype(MyMinSetCover::eval<ABCDE, DE, AE, ABCD>());
    EXPECT_TRUE(decltype(hasElement<Result, ABCD>()){});
    EXPECT_FALSE(decltype(hasElement<Result, AE>()){});
    EXPECT_TRUE(decltype(hasElement<Result, DE>()){});
  }
}

TEST(MinSetCoverTest, GreedyAndLastOneWinsTie) {
  using MyMinSetCover = MinSetCover<Greedy<LastOneWins>>;
  {
    using Result = decltype(MyMinSetCover::eval<ABCDE, ABCD, AE, DE>());
    EXPECT_TRUE(decltype(hasElement<Result, ABCD>()){});
    EXPECT_FALSE(decltype(hasElement<Result, AE>()){});
    EXPECT_TRUE(decltype(hasElement<Result, DE>()){});
  }
  {
    using Result = decltype(MyMinSetCover::eval<ABCDE, ABCD, DE, AE>());
    EXPECT_TRUE(decltype(hasElement<Result, ABCD>()){});
    EXPECT_TRUE(decltype(hasElement<Result, AE>()){});
    EXPECT_FALSE(decltype(hasElement<Result, DE>()){});
  }
  {
    using Result = decltype(MyMinSetCover::eval<ABCDE, AE, ABCD, DE>());
    EXPECT_TRUE(decltype(hasElement<Result, ABCD>()){});
    EXPECT_FALSE(decltype(hasElement<Result, AE>()){});
    EXPECT_TRUE(decltype(hasElement<Result, DE>()){});
  }
  {
    using Result = decltype(MyMinSetCover::eval<ABCDE, AE, DE, ABCD>());
    EXPECT_TRUE(decltype(hasElement<Result, ABCD>()){});
    EXPECT_FALSE(decltype(hasElement<Result, AE>()){});
    EXPECT_TRUE(decltype(hasElement<Result, DE>()){});
  }
  {
    using Result = decltype(MyMinSetCover::eval<ABCDE, DE, ABCD, AE>());
    EXPECT_TRUE(decltype(hasElement<Result, ABCD>()){});
    EXPECT_TRUE(decltype(hasElement<Result, AE>()){});
    EXPECT_FALSE(decltype(hasElement<Result, DE>()){});
  }
  {
    using Result = decltype(MyMinSetCover::eval<ABCDE, DE, AE, ABCD>());
    EXPECT_TRUE(decltype(hasElement<Result, ABCD>()){});
    EXPECT_TRUE(decltype(hasElement<Result, AE>()){});
    EXPECT_FALSE(decltype(hasElement<Result, DE>()){});
  }
}

using CDE = U::Set<C, D, E>;

TEST(MinSetCoverTest, GreedyAndTightestOneWinsTie) {
  using MyMinSetCover = MinSetCover<Greedy<TightestOneWins>>;
  {
    using Result = decltype(MyMinSetCover::eval<ABCDE, ABCD, CDE, DE>());
    EXPECT_TRUE(decltype(hasElement<Result, ABCD>()){});
    EXPECT_FALSE(decltype(hasElement<Result, CDE>()){});
    EXPECT_TRUE(decltype(hasElement<Result, DE>()){});
  }
  {
    using Result = decltype(MyMinSetCover::eval<ABCDE, ABCD, DE, CDE>());
    EXPECT_TRUE(decltype(hasElement<Result, ABCD>()){});
    EXPECT_FALSE(decltype(hasElement<Result, CDE>()){});
    EXPECT_TRUE(decltype(hasElement<Result, DE>()){});
  }
  {
    using Result = decltype(MyMinSetCover::eval<ABCDE, CDE, ABCD, DE>());
    EXPECT_TRUE(decltype(hasElement<Result, ABCD>()){});
    EXPECT_FALSE(decltype(hasElement<Result, CDE>()){});
    EXPECT_TRUE(decltype(hasElement<Result, DE>()){});
  }
  {
    using Result = decltype(MyMinSetCover::eval<ABCDE, CDE, DE, ABCD>());
    EXPECT_TRUE(decltype(hasElement<Result, ABCD>()){});
    EXPECT_FALSE(decltype(hasElement<Result, CDE>()){});
    EXPECT_TRUE(decltype(hasElement<Result, DE>()){});
  }
  {
    using Result = decltype(MyMinSetCover::eval<ABCDE, DE, ABCD, CDE>());
    EXPECT_TRUE(decltype(hasElement<Result, ABCD>()){});
    EXPECT_FALSE(decltype(hasElement<Result, CDE>()){});
    EXPECT_TRUE(decltype(hasElement<Result, DE>()){});
  }
  {
    using Result = decltype(MyMinSetCover::eval<ABCDE, DE, CDE, ABCD>());
    EXPECT_TRUE(decltype(hasElement<Result, ABCD>()){});
    EXPECT_FALSE(decltype(hasElement<Result, CDE>()){});
    EXPECT_TRUE(decltype(hasElement<Result, DE>()){});
  }
}

TEST(MinSetCoverTest, GreedyAndLoosestOneWinsTie) {
  using MyMinSetCover = MinSetCover<Greedy<LoosestOneWins>>;
  {
    using Result = decltype(MyMinSetCover::eval<ABCDE, ABCD, CDE, DE>());
    EXPECT_TRUE(decltype(hasElement<Result, ABCD>()){});
    EXPECT_TRUE(decltype(hasElement<Result, CDE>()){});
    EXPECT_FALSE(decltype(hasElement<Result, DE>()){});
  }
  {
    using Result = decltype(MyMinSetCover::eval<ABCDE, ABCD, DE, CDE>());
    EXPECT_TRUE(decltype(hasElement<Result, ABCD>()){});
    EXPECT_TRUE(decltype(hasElement<Result, CDE>()){});
    EXPECT_FALSE(decltype(hasElement<Result, DE>()){});
  }
  {
    using Result = decltype(MyMinSetCover::eval<ABCDE, CDE, ABCD, DE>());
    EXPECT_TRUE(decltype(hasElement<Result, ABCD>()){});
    EXPECT_TRUE(decltype(hasElement<Result, CDE>()){});
    EXPECT_FALSE(decltype(hasElement<Result, DE>()){});
  }
  {
    using Result = decltype(MyMinSetCover::eval<ABCDE, CDE, DE, ABCD>());
    EXPECT_TRUE(decltype(hasElement<Result, ABCD>()){});
    EXPECT_TRUE(decltype(hasElement<Result, CDE>()){});
    EXPECT_FALSE(decltype(hasElement<Result, DE>()){});
  }
  {
    using Result = decltype(MyMinSetCover::eval<ABCDE, DE, ABCD, CDE>());
    EXPECT_TRUE(decltype(hasElement<Result, ABCD>()){});
    EXPECT_TRUE(decltype(hasElement<Result, CDE>()){});
    EXPECT_FALSE(decltype(hasElement<Result, DE>()){});
  }
  {
    using Result = decltype(MyMinSetCover::eval<ABCDE, DE, CDE, ABCD>());
    EXPECT_TRUE(decltype(hasElement<Result, ABCD>()){});
    EXPECT_TRUE(decltype(hasElement<Result, CDE>()){});
    EXPECT_FALSE(decltype(hasElement<Result, DE>()){});
  }
}
