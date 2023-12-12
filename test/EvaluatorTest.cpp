#include <Evaluator.h>
#include <algorithm>
#include <gtest/gtest.h>
#include <numeric>
#include <typeindex>
#include <unordered_set>

using namespace set_cover;

struct Min {
  using Type = int;
};
struct Max {
  using Type = int;
};
struct Avg {
  using Type = float;
};
struct Var {
  using Type = float;
};
struct Sorted {
  using Type = std::vector<int>;
};

using U = Universe<Min, Max, Avg, Var, Sorted>;

struct GetMin {
  using EvalList = U::KPerm<Min>;
  std::tuple<int> operator()(const std::vector<int> &aIn) {
    return *std::min_element(aIn.begin(), aIn.end());
  }
};

struct GetMax {
  using EvalList = U::KPerm<Max>;
  std::tuple<int> operator()(const std::vector<int> &aIn) {
    return *std::max_element(aIn.begin(), aIn.end());
  }
};

struct GetSorted {
  using EvalList = U::KPerm<Sorted, Min, Max>;
  std::tuple<std::vector<int>, int, int>
  operator()(const std::vector<int> &aIn) {
    std::vector<int> out = aIn;
    std::sort(out.begin(), out.end());
    return {out, out.front(), out.back()};
  }
};

struct GetAvg {
  using EvalList = U::KPerm<Avg>;
  std::tuple<float> operator()(const std::vector<int> &aIn) {
    return std::accumulate(aIn.begin(), aIn.end(), 0.0) / aIn.size();
  }
};

struct GetVar {
  using EvalList = U::KPerm<Var, Avg>;
  std::tuple<float, float> operator()(const std::vector<int> &aIn) {
    const auto [avg] = GetAvg()(aIn);
    float var = 0;
    for (int val : aIn) {
      var += (val - avg) * (val - avg);
    }
    return {var / aIn.size(), avg};
  }
};

using MyEvaluator =
    Evaluator<U, LogTypeIndex, GetMin, GetMax, GetSorted, GetAvg, GetVar>;
using Log = LogTypeIndex::Log;

TEST(EvaluatorTest, Case1) {
  const std::vector vec = {1, 5, 8, 2, 6, 3};
  MyEvaluator e;
  const auto [min] = e.eval<Min>(vec);
  const Log expectedLog = {std::type_index(typeid(GetMin))};
  EXPECT_EQ(e.getLog(), expectedLog);
  EXPECT_EQ(min, 1);
}

TEST(EvaluatorTest, Case2) {
  const std::vector vec = {1, 5, 8, 2, 6, 3};
  MyEvaluator e;
  const auto [max, sorted] = e.eval<Max, Sorted>(vec);
  const Log expectedLog = {std::type_index(typeid(GetSorted))};
  EXPECT_EQ(e.getLog(), expectedLog);
  EXPECT_EQ(max, 8);
}

TEST(EvaluatorTest, Case3) {
  const std::vector vec = {1, 5, 8, 2, 6, 3};
  MyEvaluator e;
  const auto [var, avg] = e.eval<Var, Avg>(vec);
  const Log expectedLog = {std::type_index(typeid(GetVar))};
  EXPECT_EQ(e.getLog(), expectedLog);
  EXPECT_NEAR(var, 5.806, 1e-3);
  EXPECT_NEAR(avg, 4.167, 1e-3);
}

TEST(EvaluatorTest, Case4) {
  const std::vector vec = {1, 5, 8, 2, 6, 3};
  MyEvaluator e;
  const auto [min, var, avg] = e.eval<Min, Var, Avg>(vec);
  const Log expectedLog = {std::type_index(typeid(GetMin)),
                           std::type_index(typeid(GetVar))};
  EXPECT_EQ(e.getLog(), expectedLog);
  EXPECT_EQ(min, 1);
  EXPECT_NEAR(var, 5.806, 1e-3);
  EXPECT_NEAR(avg, 4.167, 1e-3);
}

TEST(EvaluatorTest, Case5) {
  const std::vector vec = {1, 5, 8, 2, 6, 3};
  MyEvaluator e;
  const auto [min, max, avg, var] = e.eval<Min, Max, Avg, Var>(vec);
  const Log expectedLog = {std::type_index(typeid(GetSorted)),
                           std::type_index(typeid(GetVar))};
  EXPECT_EQ(e.getLog(), expectedLog);
  EXPECT_EQ(min, 1);
  EXPECT_EQ(max, 8);
  EXPECT_NEAR(var, 5.806, 1e-3);
  EXPECT_NEAR(avg, 4.167, 1e-3);
}

// Universe of same elements but defined in different order.
// This should not lead to any observable difference.
using U2 = Universe<Sorted, Var, Avg, Max, Min>;

struct GetMin2 : public GetMin {
  using EvalList = U2::KPerm<Min>;
};

struct GetMax2 : public GetMax {
  using EvalList = U2::KPerm<Max>;
};

struct GetSorted2 : public GetSorted {
  using EvalList = U2::KPerm<Sorted, Min, Max>;
};

struct GetAvg2 : public GetAvg {
  using EvalList = U2::KPerm<Avg>;
};

struct GetVar2 : public GetVar {
  using EvalList = U2::KPerm<Var, Avg>;
};

using MyEvaluator2 =
    Evaluator<U2, LogTypeIndex, GetMin2, GetMax2, GetSorted2, GetAvg2, GetVar2>;

TEST(EvaluatorTest, Case1InAlternateUniverse) {
  const std::vector vec = {1, 5, 8, 2, 6, 3};
  MyEvaluator2 e;
  const auto [min] = e.eval<Min>(vec);
  const Log expectedLog = {std::type_index(typeid(GetMin2))};
  EXPECT_EQ(e.getLog(), expectedLog);
  EXPECT_EQ(min, 1);
}

TEST(EvaluatorTest, Case2InAlternateUniverse) {
  const std::vector vec = {1, 5, 8, 2, 6, 3};
  MyEvaluator2 e;
  const auto [max, sorted] = e.eval<Max, Sorted>(vec);
  const Log expectedLog = {std::type_index(typeid(GetSorted2))};
  EXPECT_EQ(e.getLog(), expectedLog);
  EXPECT_EQ(max, 8);
}

TEST(EvaluatorTest, Case3InAlternateUniverse) {
  const std::vector vec = {1, 5, 8, 2, 6, 3};
  MyEvaluator2 e;
  const auto [var, avg] = e.eval<Var, Avg>(vec);
  const Log expectedLog = {std::type_index(typeid(GetVar2))};
  EXPECT_EQ(e.getLog(), expectedLog);
  EXPECT_NEAR(var, 5.806, 1e-3);
  EXPECT_NEAR(avg, 4.167, 1e-3);
}

TEST(EvaluatorTest, Case4InAlternateUniverse) {
  const std::vector vec = {1, 5, 8, 2, 6, 3};
  MyEvaluator2 e;
  const auto [min, var, avg] = e.eval<Min, Var, Avg>(vec);
  const Log expectedLog = {std::type_index(typeid(GetMin2)),
                           std::type_index(typeid(GetVar2))};
  EXPECT_EQ(e.getLog(), expectedLog);
  EXPECT_EQ(min, 1);
  EXPECT_NEAR(var, 5.806, 1e-3);
  EXPECT_NEAR(avg, 4.167, 1e-3);
}

TEST(EvaluatorTest, Case5InAlternateUniverse) {
  const std::vector vec = {1, 5, 8, 2, 6, 3};
  MyEvaluator2 e;
  const auto [min, max, avg, var] = e.eval<Min, Max, Avg, Var>(vec);
  const Log expectedLog = {std::type_index(typeid(GetSorted2)),
                           std::type_index(typeid(GetVar2))};
  EXPECT_EQ(e.getLog(), expectedLog);
  EXPECT_EQ(min, 1);
  EXPECT_EQ(max, 8);
  EXPECT_NEAR(var, 5.806, 1e-3);
  EXPECT_NEAR(avg, 4.167, 1e-3);
}
