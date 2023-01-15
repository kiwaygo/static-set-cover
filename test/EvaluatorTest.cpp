#include <Evaluator.h>
#include <algorithm>
#include <gtest/gtest.h>
#include <numeric>
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
using Log = std::unordered_set<std::string>;

struct GetMin {
  using EvalList = U::KPerm<Min>;
  std::tuple<int> operator()(const std::vector<int> &aIn, Log *aLog) {
    if (aLog) {
      aLog->insert("GetMin");
    }
    return *std::min_element(aIn.begin(), aIn.end());
  }
};

struct GetMax {
  using EvalList = U::KPerm<Max>;
  std::tuple<int> operator()(const std::vector<int> &aIn, Log *aLog) {
    if (aLog) {
      aLog->insert("GetMax");
    }
    return *std::max_element(aIn.begin(), aIn.end());
  }
};

struct GetSorted {
  using EvalList = U::KPerm<Sorted, Min, Max>;
  std::tuple<std::vector<int>, int, int> operator()(const std::vector<int> &aIn,
                                                    Log *aLog) {
    if (aLog) {
      aLog->insert("GetSorted");
    }
    std::vector<int> out = aIn;
    std::sort(out.begin(), out.end());
    return {out, out.front(), out.back()};
  }
};

struct GetAvg {
  using EvalList = U::KPerm<Avg>;
  std::tuple<float> operator()(const std::vector<int> &aIn, Log *aLog) {
    if (aLog) {
      aLog->insert("GetAvg");
    }
    return std::accumulate(aIn.begin(), aIn.end(), 0.0) / aIn.size();
  }
};

struct GetVar {
  using EvalList = U::KPerm<Var, Avg>;
  std::tuple<float, float> operator()(const std::vector<int> &aIn, Log *aLog) {
    if (aLog) {
      aLog->insert("GetVar");
    }
    const auto [avg] = GetAvg()(aIn, nullptr);
    float var = 0;
    for (int val : aIn) {
      var += (val - avg) * (val - avg);
    }
    return {var / aIn.size(), avg};
  }
};

using MyEvaluator = Evaluator<U, GetMin, GetMax, GetSorted, GetAvg, GetVar>;

TEST(EvaluatorTest, Case1) {
  const std::vector vec = {1, 5, 8, 2, 6, 3};
  Log myLog;
  const auto [min] = MyEvaluator::eval<Min>(vec, &myLog);
  const Log expectedLog = {"GetMin"};
  EXPECT_EQ(myLog, expectedLog);
  EXPECT_EQ(min, 1);
}

TEST(EvaluatorTest, Case2) {
  const std::vector vec = {1, 5, 8, 2, 6, 3};
  Log myLog;
  const auto [max, sorted] = MyEvaluator::eval<Max, Sorted>(vec, &myLog);
  const Log expectedLog = {"GetSorted"};
  EXPECT_EQ(myLog, expectedLog);
  EXPECT_EQ(max, 8);
}

TEST(EvaluatorTest, Case3) {
  const std::vector vec = {1, 5, 8, 2, 6, 3};
  Log myLog;
  const auto [var, avg] = MyEvaluator::eval<Var, Avg>(vec, &myLog);
  const Log expectedLog = {"GetVar"};
  EXPECT_EQ(myLog, expectedLog);
  EXPECT_NEAR(var, 5.806, 1e-3);
  EXPECT_NEAR(avg, 4.167, 1e-3);
}

TEST(EvaluatorTest, Case4) {
  const std::vector vec = {1, 5, 8, 2, 6, 3};
  Log myLog;
  const auto [min, var, avg] = MyEvaluator::eval<Min, Var, Avg>(vec, &myLog);
  const Log expectedLog = {"GetMin", "GetVar"};
  EXPECT_EQ(myLog, expectedLog);
  EXPECT_EQ(min, 1);
  EXPECT_NEAR(var, 5.806, 1e-3);
  EXPECT_NEAR(avg, 4.167, 1e-3);
}

TEST(EvaluatorTest, Case5) {
  const std::vector vec = {1, 5, 8, 2, 6, 3};
  Log myLog;
  const auto [min, max, avg, var] =
      MyEvaluator::eval<Min, Max, Avg, Var>(vec, &myLog);
  const Log expectedLog = {"GetSorted", "GetVar"};
  EXPECT_EQ(myLog, expectedLog);
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
    Evaluator<U2, GetMin2, GetMax2, GetSorted2, GetAvg2, GetVar2>;

TEST(EvaluatorTest, Case1InAlternateUniverse) {
  const std::vector vec = {1, 5, 8, 2, 6, 3};
  Log myLog;
  const auto [min] = MyEvaluator2::eval<Min>(vec, &myLog);
  const Log expectedLog = {"GetMin"};
  EXPECT_EQ(myLog, expectedLog);
  EXPECT_EQ(min, 1);
}

TEST(EvaluatorTest, Case2InAlternateUniverse) {
  const std::vector vec = {1, 5, 8, 2, 6, 3};
  Log myLog;
  const auto [max, sorted] = MyEvaluator2::eval<Max, Sorted>(vec, &myLog);
  const Log expectedLog = {"GetSorted"};
  EXPECT_EQ(myLog, expectedLog);
  EXPECT_EQ(max, 8);
}

TEST(EvaluatorTest, Case3InAlternateUniverse) {
  const std::vector vec = {1, 5, 8, 2, 6, 3};
  Log myLog;
  const auto [var, avg] = MyEvaluator2::eval<Var, Avg>(vec, &myLog);
  const Log expectedLog = {"GetVar"};
  EXPECT_EQ(myLog, expectedLog);
  EXPECT_NEAR(var, 5.806, 1e-3);
  EXPECT_NEAR(avg, 4.167, 1e-3);
}

TEST(EvaluatorTest, Case4InAlternateUniverse) {
  const std::vector vec = {1, 5, 8, 2, 6, 3};
  Log myLog;
  const auto [min, var, avg] = MyEvaluator2::eval<Min, Var, Avg>(vec, &myLog);
  const Log expectedLog = {"GetMin", "GetVar"};
  EXPECT_EQ(myLog, expectedLog);
  EXPECT_EQ(min, 1);
  EXPECT_NEAR(var, 5.806, 1e-3);
  EXPECT_NEAR(avg, 4.167, 1e-3);
}

TEST(EvaluatorTest, Case5InAlternateUniverse) {
  const std::vector vec = {1, 5, 8, 2, 6, 3};
  Log myLog;
  const auto [min, max, avg, var] =
      MyEvaluator2::eval<Min, Max, Avg, Var>(vec, &myLog);
  const Log expectedLog = {"GetSorted", "GetVar"};
  EXPECT_EQ(myLog, expectedLog);
  EXPECT_EQ(min, 1);
  EXPECT_EQ(max, 8);
  EXPECT_NEAR(var, 5.806, 1e-3);
  EXPECT_NEAR(avg, 4.167, 1e-3);
}
