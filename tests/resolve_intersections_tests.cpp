#include <gtest/gtest.h>

#include <dcel_polygon2d.h>
#include <geom_utils.h>
#include <polygon2d.h>
#include <resolve_intersections.h>
#include <test_utils/decomposition_utils.h>

#include <list>
#include <optional>
#include <vector>

#include <debug_utils.h>

namespace decomposition_tests {

namespace {

/*
std::vector<geom::Point2D> Intersections(const geom::Polygon2D& polygon) {
  std::vector<geom::Point2D> res;
  std::vector<geom::Segment2D> segments;
  const geom::Polygon2D::Vertex* current = polygon.GetAnyVertex();
  for (size_t i = 0; i < polygon.Size(); i++, current = current->next) {
    geom::Point2D a = current->point, b = current->next->point;
    segments.push_back({a, b});
  }
  for (size_t i = 0; i < segments.size(); i++) {
    for (size_t j = i + 1; j < segments.size(); j++) {
      std::optional<geom::Point2D> int_pnt_opt =
          geom::IntersectionPoint(segments[i], segments[j]);
      if (int_pnt_opt) {
        if (!IsIntersectionOnVertex(segments[i], segments[j])) {
          res.push_back(int_pnt_opt.value());
        }
      }
    }
  }
  return res;
}
//*/

struct DoublePoint2DComparator {
  bool operator()(const geom::Point2D& lhp, const geom::Point2D& rhp) const {
    if (geom::DoubleEqual(lhp, rhp))
      return false;
    return lhp < rhp;
  }
};

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
  }
};

class ResolveIntersectionsTest :
    public testing::TestWithParam<ResolveIntersectionsCase> {
 public:
  void SetUp() override {
    expected_result_ = GetParam().result;
  }

  void TearDown() override {
    if (GetExpectedResultSize() == 0) {
      for (const geom::Polygon2D polygon :
          FindAnswer(geom::Polygon2D(GetInitialPolygonVector())))
        expected_result_.push_back(geom::AsVector(polygon));
    }

    EXPECT_EQ(answer_.size(), GetExpectedResultSize());
    for (const std::vector<geom::Point2D>& polygon_i : GetExpectedResult()) {
      bool found = false;
      for (const std::vector<geom::Point2D>& polygon_j : answer_)
        if (PolygonVectorEqual(polygon_i, polygon_j))
          found = true;
      EXPECT_TRUE(found);
    }
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

  const std::list<std::vector<geom::Point2D> >& GetExpectedResult() const {
    return expected_result_;
  }

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

  std::list<std::vector<geom::Point2D> > expected_result_;
  std::list<std::vector<geom::Point2D> > answer_;
};

TEST_P(ResolveIntersectionsTest, ResolveIntersections) {
  geom::Polygon2D polygon(GetInitialPolygonVector());
  for (const geom::Polygon2D& res_polygon : geom::ResolveIntersections(polygon)) {
    // PrintPolygon(res_polygon);
    answer_.push_back(geom::AsVector(res_polygon));
  }
  // PrintL();
}

INSTANTIATE_TEST_SUITE_P(Decomposition,
                         ResolveIntersectionsTest,
                         testing::ValuesIn(resolve_intersections_cases));

}  // decomposition_tests
