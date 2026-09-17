#include <benchmark/benchmark.h>

#include <triangulation.h>

#include <cmath>
#include <random>
#include <vector>

namespace {

// Convex, non-self-intersecting polygon: exercises the fast path where
// ResolveIntersections finds nothing to do and the cost is dominated by
// y-monotone decomposition + triangulation.
std::vector<geom::Point2D> MakeConvexPolygon(size_t n) {
  std::vector<geom::Point2D> polygon_v;
  polygon_v.reserve(n);
  const double step = M_PI_2 / static_cast<double>(n);
  const double distance = 1e3;
  for (size_t i = 0; i < n; i++) {
    const double angle = step * static_cast<double>(i);
    polygon_v.push_back({distance * std::cos(angle), distance * std::sin(angle)});
  }
  return polygon_v;
}

// Random (generally self-intersecting) polygon: exercises the
// Bentley-Ottmann style ResolveIntersections path. Seeded for
// run-to-run reproducibility of the benchmark.
std::vector<geom::Point2D> MakeRandomPolygon(size_t n) {
  std::mt19937 rng(42);
  std::uniform_real_distribution<double> dist(0.0, 100.0);
  std::vector<geom::Point2D> polygon_v;
  polygon_v.reserve(n);
  for (size_t i = 0; i < n; i++)
    polygon_v.push_back({dist(rng), dist(rng)});
  return polygon_v;
}

}  // namespace

static void BM_ConvexPolygonTriangulation(benchmark::State& state) {
  const std::vector<geom::Point2D> polygon_v =
      MakeConvexPolygon(static_cast<size_t>(state.range(0)));
  for (auto _ : state)
    benchmark::DoNotOptimize(geom::Triangulate(polygon_v));
  state.SetComplexityN(state.range(0));
}
BENCHMARK(BM_ConvexPolygonTriangulation)
    ->Arg(100)
    ->Arg(1000)
    ->Arg(10000)
    ->Arg(100000)
    ->Unit(benchmark::kMillisecond)
    ->Complexity();

static void BM_RandomPolygonTriangulation(benchmark::State& state) {
  const std::vector<geom::Point2D> polygon_v =
      MakeRandomPolygon(static_cast<size_t>(state.range(0)));
  for (auto _ : state)
    benchmark::DoNotOptimize(geom::Triangulate(polygon_v));
  state.SetComplexityN(state.range(0));
}
BENCHMARK(BM_RandomPolygonTriangulation)
    ->Arg(10)
    ->Arg(100)
    ->Arg(300)
    ->Unit(benchmark::kMillisecond)
    ->Complexity();

BENCHMARK_MAIN();
