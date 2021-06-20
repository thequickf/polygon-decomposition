#include <gtest/gtest.h>

#include <dcel_polygon2d.h>
#include <geom_utils.h>
#include <polygon2d.h>
#include <resolve_intersections.h>
#include <test_utils/decomposition_utils.h>

#include <cstdlib>
#include <ctime>
#include <list>
#include <optional>
#include <vector>

namespace decomposition_tests {

namespace {

std::list<geom::Polygon2D> FindAnswer(const geom::Polygon2D& polygon) {
  geom::DcelPolygon2D dcel_polygon(polygon);

  std::vector<geom::Segment2D> segments;
  const geom::Polygon2D::Vertex* current = polygon.GetAnyVertex();
  for (size_t i = 0; i < polygon.Size(); i++, current = current->next) {
    geom::Point2D a = current->point, b = current->next->point;
    segments.push_back({a, b});
  }

  auto RemoveSegment = [&](geom::Segment2D segment) {
    for (auto it = segments.begin(); it != segments.end(); it++) {
      if (DoubleEqual(*it, segment)) {
        segments.erase(it);
        break;
      }
    }
  };

  auto RepairSegments = [&](geom::Segment2D a,
                            geom::Segment2D b) {
    RemoveSegment(a);
    RemoveSegment(b);

    const geom::Point2D int_pnt = geom::IntersectionPoint(a, b).value();
    segments.push_back({a.a, int_pnt});
    segments.push_back({b.a, int_pnt});
    segments.push_back({int_pnt, a.b });
    segments.push_back({int_pnt, b.b });
  };

  auto ResolveOneIntersection = [&]() {
    for (size_t i = 0; i < segments.size(); i++) {
      for (size_t j = i + 1; j < segments.size(); j++) {
        std::optional<geom::Point2D> int_pnt_opt =
            geom::IntersectionPoint(segments[i], segments[j]);
        if (int_pnt_opt) {
          if (!IsIntersectionOnVertex(segments[i], segments[j])) {
            dcel_polygon.ResolveIntersection(segments[i], segments[j]);
            RepairSegments(segments[i], segments[j]);
            return true;
          }
        }
      }
    }
    return false;
  };

  while (ResolveOneIntersection()) { }

  return dcel_polygon.GetPolygons();
}

bool EqualAnswers(const std::list<geom::Polygon2D>& answer1,
                  const std::list<geom::Polygon2D>& answer2) {
  if (answer1.size() != answer2.size())
    return false;
  for (const geom::Polygon2D& polygon_i : answer1) {
    bool found = false;
    for (const geom::Polygon2D& polygon_j : answer2)
      if (PolygonEqual(polygon_i, polygon_j))
        found = true;
    if (!found)
      return false;
  }
  return true;
}

}  // namespace

struct ResolveIntersectionsCase {
  std::vector<geom::Point2D> polygon_v;
  std::list<std::vector<geom::Point2D> > result;

  ResolveIntersectionsCase(const std::vector<geom::Point2D>& polygon_v,
      const std::list<std::vector<geom::Point2D>>& result) :
      polygon_v(polygon_v), result(result) {}
};

ResolveIntersectionsCase resolve_intersections_cases[] = {
  {
    { {-2, 0}, {-1, 1}, {1, -1}, {3, 1}, {4, 0} },
    {
      { {-2, 0}, {-1, 1}, {0, 0} },
      { {2, 0}, {1, -1}, {0, 0} },
      { {2, 0}, {3, 1}, {4, 0} }
    }
  },
  {
    { {0, 2}, {1, 1}, {-1, -1}, {1, -3}, {0, -4} },
    {
      { {0, 2}, {1, 1}, {0, 0} },
      { {0, -2}, {-1, -1}, {0, 0} },
      { {0, -2}, {1, -3}, {0, -4} }
    }
  },
  {
    { {2, 0}, {1, 1}, {-1, -1}, {-3, 1}, {-4, 0} },
    {
      { {2, 0}, {0, 0}, {1, 1} },
      { {-2, 0}, {0, 0}, {-1, -1} },
      { {-2, 0}, {-4, 0}, {-3, 1} }
    }
  },
  {
    { {-2, 0}, {-1, -1}, {1, 1}, {3, -1}, {4, 0} },
    {
      { {-2, 0}, {0, 0}, {-1, -1} },
      { {2, 0}, {0, 0}, {1, 1} },
      { {2, 0}, {4, 0}, {3, -1} }
    }
  },
  {
    { {0, 0}, {18, 0}, {0, 6}, {18, 6}, {0, 12}, {18, 12} },
    { 
      { {0, 0}, {6, 4}, {18, 0} },
      { {18, 12}, {12, 8}, {0, 12} },
      { {6, 4}, {0, 6}, {9, 6} },
      { {9, 6}, {12, 8}, {18, 6} }
    }
  },
  {
    {
      {177, 54}, {428, 58}, {168, 109}, {438, 110}, {164, 157},
      {455, 152}, {177, 201}, {434, 202}, {387, 39}
    },
    {}
  },
  {
    { 
      { 253, 91 }, { 389, 149 }, { 352, 289 }, { 266, 317 }, 
      { 360, 180 }, { 458, 258 }, { 503, 332 }, { 397, 341 } 
    },
    {}
  },
  {
    {
      { 132, 100 }, { 457, 114 }, { 140, 152 }, { 475, 155 }, { 148, 198 },
      { 484, 214 }, { 153, 244 }, { 490, 254 }, { 157, 285 }, { 511, 297 },
      { 162, 326 }, { 518, 333 }, { 179, 362 }, { 519, 355 }, { 452, 390 },
      { 394, 94 }, { 400, 395 }, { 362, 74 }, { 351, 383 }, { 309, 71 },
      { 324, 396 }, { 278, 79 }, { 279, 389 }, { 250, 59 }, { 244, 378 },
      { 223, 64 }, { 218, 385 }, { 186, 57 }
    },
    {}
  },
  {
    {
      { 256, 268 }, { 381, 63 }, { 475, 207 }, { 438, 333 },
      { 317, 220 }, { 503, 126 }, { 300, 341 }, { 471, 61 }
    },
    {}
  },
  {
    {
      { 270, 71 }, { 515, 105 }, { 350, 161 }, { 572, 129 }, { 329, 50 },
      { 557, 224 }, { 291, 57 }, { 522, 304 }, { 396, 28 }, { 511, 179 },
      { 335, 76 }, { 483, 235 }, { 488, 23 }, { 429, 235 }, { 474, 126 },
      { 545, 216 }, { 304, 173 }, { 535, 254 }, { 283, 111 }, { 556, 182 },
      { 264, 169 }, { 472, 150 }, { 371, 135 }, { 433, 348 }, { 394, 413 }
    },
    {}
  },
  {
    {
      { 264, 76 }, { 532, 209 }, { 283, 316 }, { 414, 58 }, { 550, 191 },
      { 337, 203 }, { 443, 239 }, { 515, 392 }, { 243, 260 }, { 161, 186 },
      { 536, 264 }, { 248, 151 }, { 331, 95 }, { 476, 253 }, { 251, 342 },
      { 530, 31 }, { 330, 343 }, { 193, 254 }
    },
    {}
  },
  {
    {
      { 61.1277, 252.089 }, { 752.42, 823.254 },
      { 4.75759, 819.291 }, { 697.899, 525.472 },
      { 629.923, 349.454 }, { 796.909, 939.487 },
      { 151.533, 617.237 }, { 617.837, 549.804 },
      { 801.893, 245.278 }, { 68.5313, 671.656 }
    },
    {}
  },
  {
    {
      {60.8714, 7.13482}, {43.8332, 53.0389},
      {20.0216, 43.5795}, {76.4169, 74.797},
      {74.8503, 95.6949}, {37.8128, 85.6067},
      {54.3016, 11.6027}, {59.4894, 15.7281},
      {16.2059, 7.00161}, {17.1892, 15.1514}
    },
    {}
  },
  {
    {
      {44.2384, 45.0552}, {1.74621, 97.5357}, {22.6774, 25.5534},
      {91.7858, 99.2726}, {83.5407, 7.6557}, {42.8191, 46.5435}
    },
    {}
  },
  {
    {
      {53.1108, 86.1122}, {29.447, 0.315036}, {2.72557, 24.8838},
      {62.593, 64.7074}, {13.2917, 29.5329}, {28.1249, 3.11356},
    },
    {}
  },
  {
    {
      {48.2635, 36.5512}, {5.4763, 18.0554}, {51.8058, 54.7146},
      {96.9084, 25.1886}, {54.6538, 0.487669}, {27.4778, 33.1751}
    },
    {}
  }
};

class ResolveIntersectionsTest :
    public testing::TestWithParam<ResolveIntersectionsCase> {
 public:
  void SetUp() override {
    for (const std::vector<geom::Point2D>& polygon : GetParam().result)
      expected_result_.push_back(geom::Polygon2D(polygon));
  }

  void TearDown() override {
    if (GetExpectedResultSize() == 0)
      expected_result_ = FindAnswer(geom::Polygon2D(GetInitialPolygonVector()));
    EXPECT_TRUE(EqualAnswers(answer_, expected_result_));
  }

 protected:
  size_t GetInitialPolygonSize() const {
    return GetParam().polygon_v.size();
  }

  std::vector<geom::Point2D> GetInitialPolygonVector() const {
    return GetParam().polygon_v;
  }

  size_t GetExpectedResultSize() const {
    return expected_result_.size();
  }

  const std::list<geom::Polygon2D>& GetExpectedResult() const {
    return expected_result_;
  }

  std::list<geom::Polygon2D> expected_result_;
  std::list<geom::Polygon2D> answer_;
};

TEST_P(ResolveIntersectionsTest, ResolveIntersections) {
  geom::Polygon2D polygon(GetInitialPolygonVector());
  answer_ = geom::ResolveIntersections(polygon);
}

INSTANTIATE_TEST_SUITE_P(Decomposition,
                         ResolveIntersectionsTest,
                         testing::ValuesIn(resolve_intersections_cases));


TEST(FuzzingTest, ResolveIntersections) {
  std::srand(std::time(nullptr));
  const size_t fuzzing_size = 10000;
  const size_t case_size = 10;
  for (size_t i = 0; i < fuzzing_size; i++) {
    std::vector<geom::Point2D> polygon_v;
    polygon_v.reserve(case_size);
    for (size_t j = 0; j < case_size; j++)
      polygon_v.push_back({DoubleRand(0, 100), DoubleRand(0, 100)});
    const geom::Polygon2D polygon(polygon_v);
    const std::list<geom::Polygon2D> expected_answer = FindAnswer(polygon);
    const std::list<geom::Polygon2D> actual_answer =
        geom::ResolveIntersections(polygon);
    EXPECT_TRUE(EqualAnswers(expected_answer, actual_answer));
  }
}

}  // decomposition_tests
