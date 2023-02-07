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
using AB = U::Set<A, B>;
using BD = U::Set<B, D>;
using CE = U::Set<C, E>;
using BE = U::Set<B, E>;

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
  using Result = decltype(MyMinSetCover::eval<ABCDE, AB, BD, CE, BE>());
  EXPECT_TRUE(decltype(hasElement<Result, AB>()){});
  EXPECT_TRUE(decltype(hasElement<Result, BD>()){});
  EXPECT_TRUE(decltype(hasElement<Result, CE>()){});
  EXPECT_FALSE(decltype(hasElement<Result, BE>()){});
}

TEST(MinSetCoverTest, GreedyAndLastOneWinsTie) {
  using MyMinSetCover = MinSetCover<Greedy<LastOneWins>>;
  using Result = decltype(MyMinSetCover::eval<ABCDE, AB, BD, CE, BE>());
  EXPECT_TRUE(decltype(hasElement<Result, AB>()){});
  EXPECT_TRUE(decltype(hasElement<Result, BD>()){});
  EXPECT_TRUE(decltype(hasElement<Result, CE>()){});
  EXPECT_TRUE(decltype(hasElement<Result, BE>()){});
}

using ABCD = U::Set<A, B, C, D>;
using DE = U::Set<D, E>;
using _E = U::Set<E>;

TEST(MinSetCoverTest, GreedyAndTightestOneWinsTie) {
  using MyMinSetCover = MinSetCover<Greedy<TightestOneWins>>;
  using Result = decltype(MyMinSetCover::eval<ABCDE, ABCD, DE, _E>());
  EXPECT_TRUE(decltype(hasElement<Result, ABCD>()){});
  EXPECT_FALSE(decltype(hasElement<Result, DE>()){});
  EXPECT_TRUE(decltype(hasElement<Result, _E>()){});
}

TEST(MinSetCoverTest, GreedyAndLoosestOneWinsTie) {
  using MyMinSetCover = MinSetCover<Greedy<LoosestOneWins>>;
  using Result = decltype(MyMinSetCover::eval<ABCDE, ABCD, DE, _E>());
  EXPECT_TRUE(decltype(hasElement<Result, ABCD>()){});
  EXPECT_TRUE(decltype(hasElement<Result, DE>()){});
  EXPECT_FALSE(decltype(hasElement<Result, _E>()){});
}
