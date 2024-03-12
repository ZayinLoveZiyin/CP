namespace geometry2d {

constexpr long double eps = 1e-7;
constexpr long double pi = std::acos(-1);

int fsign(long double x) { return (x > eps) - (x < -eps); }
long double sqr(long double x) { return x * x; }

// Ax^2+Bx+c=0
std::vector<long double> solveEquationP2(long double A, long double B,
                                         long double C) {
  long double delta = B * B - 4 * A * C;
  if (fsign(delta) < 0) return {};
  if (fsign(delta) == 0) return {-0.5 * B / A};
  long double sqrt_delta = std::sqrt(delta);
  long double x1 = -0.5 * (B - sqrt_delta) / A,
              x2 = -0.5 * (B + sqrt_delta) / A;
  if (fsign(x1 - x2) > 0) std::swap(x1, x2);
  return {x1, x2};
}

struct Point;
struct Point3D {
  long double x, y, z;

  explicit Point3D(long double x = 0, long double y = 0, long double z = 0)
      : x(x), y(y), z(z) {}

  explicit Point3D(const Point& p);

  Point3D operator-(const Point3D& p) const {
    return Point3D(x - p.x, y - p.y, z - p.z);
  }
  long double innerProd(const Point3D& p) const {
    return x * p.x + y * p.y + z * p.z;
  }
  Point3D crossProd(const Point3D& p) const {
    return Point3D(y * p.z - z * p.y, -x * p.z + z * p.x, x * p.y - y * p.x);
  }
};

struct Point {
  long double x, y;
  explicit Point(long double x = 0, long double y = 0) : x(x), y(y) {}
  Point operator+(const Point& p) const { return Point(x + p.x, y + p.y); }
  Point operator-(const Point& p) const { return Point(x - p.x, y - p.y); }
  Point operator/(const long double& l) const { return Point(x / l, y / l); }
  Point operator*(const long double& l) const { return Point(x * l, y * l); }
  Point unit() const {
    long double l = len();
    assert(fsign(l) > 0);
    return *this / l;
  }
  long double crossProd(const Point& p) const { return x * p.y - y * p.x; }
  long double innerProd(const Point& p) const { return x * p.x + y * p.y; }
  long double lenSqr() const { return sqr(x) + sqr(y); }
  long double len() const { return std::sqrt(lenSqr()); }
  long double distanceSqr(const Point& p) const {
    return sqr(x - p.x) + sqr(y - p.y);
  }
  long double distance(const Point& p) const {
    return std::sqrt(distanceSqr(p));
  }
  long double angleWith(const Point& p) const {
    return std::atan2(crossProd(p), innerProd(p));
  }
  // return (-pi,pi]
  long double angle() const { return atan2(y, x); }
  int angleSign() const {
    if (!y) return fsign(x) < 0;
    return fsign(y);
  }
  Point rotate90() const { return Point(-y, x); }

  static long double crossProd(const Point& o, const Point& a, const Point& b) {
    return (a - o).crossProd(b - o);
  }
  static long double innerProd(const Point& o, const Point& a, const Point& b) {
    return (a - o).innerProd(b - o);
  }

  friend std::ostream& operator<<(std::ostream& os, Point p) {
    return os << "(" << p.x << "," << p.y << ")";
  }

  friend bool operator==(const Point& lhs, const Point& rhs) {
    return !fsign(lhs.x - rhs.x) && !fsign(lhs.y - rhs.y);
  }

  struct HorizontalComparer {
    bool operator()(const Point& lhs, const Point& rhs) const {
      return lhs.x < rhs.x || (lhs.x == rhs.x && lhs.y < rhs.y);
    }
  };

  struct PolarComparer {
    bool operator()(const Point& lhs, const Point& rhs) const {
      int angleSign_diff = lhs.angleSign() - rhs.angleSign();
      if (angleSign_diff) return angleSign_diff < 0;
      int crossProd_sign = fsign(lhs.crossProd(rhs));
      if (crossProd_sign) return crossProd_sign > 0;
      return lhs.lenSqr() < rhs.lenSqr();
    }
  };
};

Point3D::Point3D(const Point& p) : Point3D(p.x, p.y, p.x * p.x + p.y * p.y) {}

struct Line {
  Point pivot, unit_direction;
  Line(Point pivot, Point direction)
      : pivot(pivot), unit_direction(direction.unit()) {}
  long double angle() const { return unit_direction.angle(); }
  // 1: on the left
  // 0: in the line
  // -1: on the right
  int side(const Point& p) {
    return fsign(unit_direction.crossProd(p - pivot));
  }
  bool isParallel(const Line& l) {
    return fsign(unit_direction.crossProd(l.unit_direction)) == 0;
  }
  bool isSame(const Line& l) {
    return isParallel(l) &&
           fsign(unit_direction.crossProd(l.pivot - pivot)) == 0;
  }
  bool isSameDirection(const Line& l) {
    return isParallel(l) &&
           fsign(unit_direction.innerProd(l.unit_direction)) > 0;
  }
  Point intersection(const Line& l) {
    assert(!isParallel(l));
    return pivot + unit_direction *
                       (l.unit_direction.crossProd(l.pivot - pivot)) /
                       l.unit_direction.crossProd(unit_direction);
  }

  friend std::ostream& operator<<(std::ostream& os, Line l) {
    return os << "(" << l.pivot << "," << l.unit_direction << ")";
  }
};

struct Segment {
  Point a, b;
  Segment(Point a, Point b) : a(a), b(b) {}
  bool hasIntersection(const Segment& s) const {
    return fsign(Point::crossProd(a, b, s.a)) *
                   fsign(Point::crossProd(a, b, s.b)) <
               0 &&
           fsign(Point::crossProd(s.a, s.b, a)) *
                   fsign(Point::crossProd(s.a, s.b, b)) <
               0;
  }
  bool isPointIn(const Point& p) const {
    return fsign(Point::innerProd(p, a, b)) <= 0 &&
           fsign(Point::crossProd(p, a, b)) == 0;
  }

  Point lerp(const long double& ratio) const { return a + (b - a) * ratio; }

  friend std::ostream& operator<<(std::ostream& os, Segment s) {
    return os << "(" << s.a << "," << s.b << ")";
  }
};

struct Polygon {
  static bool isConvexInCCW(const std::vector<Point>& p) {
    for (int i = 0; i < p.size(); ++i) {
      int l = (i + p.size() - 1) % p.size();
      int r = (i + 1) % p.size();
      if (Point::crossProd(p[i], p[l], p[r]) > 0) return false;
    }
    return true;
  }
  static std::vector<int> convexHullId(const std::vector<Point>& p) {
    if (p.size() == 0) return {};
    if (p.size() == 1) return {0};
    std::vector<int> ids(p.size());
    std::iota(ids.begin(), ids.end(), 0);
    sort(ids.begin(), ids.end(),
         [&, comp = Point::HorizontalComparer()](int i, int j) {
           return comp(p[i], p[j]);
         });
    std::vector<int> res;
    for (int i : ids) {
      while (res.size() > 1 &&
             fsign(Point::crossProd(p[res.end()[-2]], p[res.end()[-1]],
                                    p[i])) <= 0)
        res.pop_back();
      res.push_back(i);
    }
    ids.pop_back();
    std::reverse(ids.begin(), ids.end());
    int lower_size = res.size();
    for (int i : ids) {
      while (res.size() > lower_size &&
             fsign(Point::crossProd(p[res.end()[-2]], p[res.end()[-1]],
                                    p[i])) <= 0)
        res.pop_back();
      res.push_back(i);
    }
    res.pop_back();
    return res;
  }
  static std::vector<Point> convexHullPoint(const std::vector<Point>& p) {
    std::vector<int> ids = convexHullId(p);
    std::vector<Point> res;
    for (int i : ids) res.push_back(p[i]);
    return res;
  }

  // should be guaranteed that `convex` is a convex hull in cww
  static bool isPointInConvexCCW(const Point& p,
                                 const std::vector<Point>& convex) {
    assert(Polygon::isConvexInCCW(convex));
    for (int i = 0; i < convex.size(); ++i) {
      Point a = convex[i];
      Point b = convex[(i + 1) % convex.size()];
      if (Point::crossProd(a, b, p) < 0) return false;
    }
    return true;
  }
};

struct Circle {
  Point o;
  long double r;
  Circle() : Circle(Point(0, 0), 0) {}
  Circle(Point o, long double r) : o(o), r(r) {}
  Point pointInDirection(long double angle) {
    return Point(o.x + r * std::cos(angle), o.y + r * std::sin(angle));
  }

  std::vector<Point> intersection(const Line& l) const {
    long double A = 1;
    long double B = l.unit_direction.innerProd(l.pivot - o) * 2;
    long double C = o.distanceSqr(l.pivot) - sqr(r);
    std::vector<long double> roots = solveEquationP2(A, B, C);
    std::vector<Point> intersects;
    for (long double x : roots)
      intersects.push_back(l.pivot + l.unit_direction * x);
    return intersects;
  }
  std::vector<Point> intersection(const Segment& s) const {
    std::vector<Point> line_intersects = intersection(Line(s.a, s.b - s.a));
    std::vector<Point> intersects;
    for (const Point& p : line_intersects)
      if (s.isPointIn(p)) {
        intersects.push_back(p);
      }
    return intersects;
  }

  // triangle oab
  long double overlapAreaWithTriangle(const Point& a, const Point& b) const {
    if (!fsign(Point::crossProd(o, a, b))) return 0;
    std::vector<Point> key_points;
    key_points.push_back(a);
    for (const Point& p : intersection(Segment(a, b))) key_points.push_back(p);
    key_points.push_back(b);
    long double res = 0;
    for (int i = 1; i < key_points.size(); ++i) {
      Point mid_point = (key_points[i - 1] + key_points[i]) / 2;
      Point ray1 = key_points[i - 1] - o;
      Point ray2 = key_points[i] - o;
      if (o.distanceSqr(mid_point) <= sqr(r)) {
        res += std::abs(ray1.crossProd(ray2));
      } else {
        res += sqr(r) * std::abs(ray1.angleWith(ray2));
      }
    }
    dbg(a, b, key_points, res);
    return 0.5 * res;
  }

  long double overlapAreaWithPolygon(const std::vector<Point>& p) const {
    long double res = 0;
    for (int i = 0; i < p.size(); ++i) {
      int j = (i + 1) % p.size();
      res += overlapAreaWithTriangle(p[i], p[j]) *
             fsign(Point::crossProd(o, p[i], p[j]));
    }
    dbg(res);
    return res;
  }

  // 1: outside
  // 0: edge
  // -1: inside
  // equal to solving following determinant (a,b,c is counter-clockwise)
  // | ax, ay, ax^2+ay^2, 1 |
  // | bx, by, bx^2+by^2, 1 |
  // | cx, cy, cx^2+cy^2, 1 |
  // | px, py, px^2+py^2, 1 |
  static int side(const Point& a, Point b, Point c, const Point& p) {
    if (fsign(Point::crossProd(a, b, c)) < 0) std::swap(b, c);
    Point3D a3(a), b3(b), c3(c), p3(p);
    b3 = b3 - a3;
    c3 = c3 - a3;
    p3 = p3 - a3;
    Point3D f = b3.crossProd(c3);
    return fsign(p3.innerProd(f));
  }

  friend std::ostream& operator<<(std::ostream& os, const Circle& c) {
    return os << "(" << c.o << "," << c.r << ")";
  }
};

// return the intersection convex in ccw, should be guaranteed that the
// intersection is finite.
struct HalfPlaneIntersection {
  static std::vector<Point> solve(std::vector<Line> lines) {
    sort(lines.begin(), lines.end(),
         [comp = Point::PolarComparer()](auto l1, auto l2) {
           if (l1.isSameDirection(l2)) {
             return l1.side(l2.pivot) < 0;
           } else {
             return comp(l1.unit_direction, l2.unit_direction);
           }
         });

    std::deque<Line> key_lines;
    std::deque<Point> key_points;
    for (int i = 0; i < lines.size(); ++i) {
      if (i > 0 && lines[i - 1].isSameDirection(lines[i])) continue;
      while (key_points.size() && lines[i].side(key_points.back()) <= 0) {
        key_lines.pop_back();
        key_points.pop_back();
      }
      while (key_points.size() && lines[i].side(key_points.front()) <= 0) {
        key_lines.pop_front();
        key_points.pop_front();
      }
      if (key_lines.size()) {
        // since it's guaranteed that the intersection is finite, therefore must
        // be empty.
        if (lines[i].isParallel(key_lines.back())) return {};
        key_points.push_back(lines[i].intersection(key_lines.back()));
      }
      key_lines.push_back(lines[i]);
    }

    while (key_points.size() &&
           key_lines.front().side(key_points.back()) <= 0) {
      key_lines.pop_back();
      key_points.pop_back();
    }

    if (key_lines.size() <= 2) return {};

    std::vector<Point> convex;
    for (int i = 0; i < key_lines.size(); ++i)
      convex.emplace_back(
          key_lines[i].intersection(key_lines[(i + 1) % key_lines.size()]));
    return convex;
  }
};

struct Triangulation {
  struct Edge {
    int v;
    std::list<Edge>::iterator rev;
    Edge(int v = 0) : v(v) {}
  };
  // delaunay triangulation
  // should be guaranteed that all points are pairwise distinct
  static std::vector<std::pair<int, int>> nearest(const std::vector<Point>& p) {
    std::vector<std::list<Edge>> neighbor(p.size());
    std::vector<int> id(p.size());
    std::iota(id.begin(), id.end(), 0);
    std::sort(id.begin(), id.end(),
              [&, comp = Point::HorizontalComparer()](int i, int j) {
                return comp(p[i], p[j]);
              });

    auto addedge = [&](int u, int v) {
      neighbor[u].push_front(v);
      neighbor[v].push_front(u);
      neighbor[u].front().rev = neighbor[v].begin();
      neighbor[v].front().rev = neighbor[u].begin();
    };
    std::function<void(int, int)> divide = [&](int l, int r) {
      if (r - l + 1 <= 3) {
        for (int i = l; i <= r; ++i)
          for (int j = l; j < i; ++j) addedge(id[i], id[j]);
        return;
      }

      int mid = (l + r) >> 1;
      divide(l, mid);
      divide(mid + 1, r);

      auto get_base_LR_edge = [&]() {
        std::vector<int> stk;
        for (int i = l; i <= r; ++i) {
          while (stk.size() >= 2 &&
                 fsign(Point::crossProd(p[id[stk.end()[-2]]],
                                        p[id[stk.end()[-1]]], p[id[i]])) < 0)
            stk.pop_back();
          stk.push_back(i);
        }
        for (int i = 1; i < stk.size(); ++i)
          if (stk[i - 1] <= mid && stk[i] > mid)
            return std::make_pair(id[stk[i - 1]], id[stk[i]]);
      };

      auto [ld, rd] = get_base_LR_edge();

      while (true) {
        addedge(ld, rd);
        Point ptL = p[ld], ptR = p[rd];
        int ch = -1, side = -1;
        for (auto it = neighbor[ld].begin(); it != neighbor[ld].end(); ++it) {
          if (fsign(Point::crossProd(ptL, ptR, p[it->v])) > 0 &&
              (!~ch || Circle::side(ptL, ptR, p[ch], p[it->v]) < 0)) {
            ch = it->v;
            side = 0;
          }
        }
        for (auto it = neighbor[rd].begin(); it != neighbor[rd].end(); ++it) {
          if (fsign(Point::crossProd(ptR, p[it->v], ptL)) > 0 &&
              (!~ch || Circle::side(ptL, ptR, p[ch], p[it->v]) < 0)) {
            ch = it->v;
            side = 1;
          }
        }
        if (!~ch) break;
        assert(side == 0 || side == 1);
        if (!side) {
          for (auto it = neighbor[ld].begin(); it != neighbor[ld].end();) {
            if (Segment(ptL, p[it->v]).hasIntersection(Segment(ptR, p[ch]))) {
              neighbor[it->v].erase(it->rev);
              neighbor[ld].erase(it++);
            } else {
              ++it;
            }
          }
          ld = ch;
        } else {
          for (auto it = neighbor[rd].begin(); it != neighbor[rd].end();) {
            if (Segment(ptR, p[it->v]).hasIntersection(Segment(ptL, p[ch]))) {
              neighbor[it->v].erase(it->rev);
              neighbor[rd].erase(it++);
            } else {
              ++it;
            }
          }
          rd = ch;
        }
      }
    };

    divide(0, p.size() - 1);

    std::vector<std::pair<int, int>> edges;
    for (int u = 0; u < p.size(); ++u)
      for (auto e : neighbor[u])
        if (u < e.v) edges.emplace_back(u, e.v);
    return edges;
  }

  // should be guaranteed that p is strictly convex
  static std::vector<std::pair<int, int>> furthest(
      const std::vector<Point>& p) {
    assert(Polygon::isConvexInCCW(p));
    std::vector<std::pair<int, int>> edges;
    if (p.size() < 3) {
      for (int i = 0; i < p.size(); ++i)
        for (int j = 0; j < i; ++j) {
          edges.emplace_back(i, j);
        }
      return edges;
    }

    std::vector<std::list<Edge>> neighbor(p.size());
    std::vector<int> ids(p.size());
    std::iota(ids.begin(), ids.end(), 0);

    // calculate cw, ccw
    std::vector<int> cw(p.size()), ccw(p.size());
    for (int i = 0; i < p.size(); ++i) {
      cw[i] = (i + p.size() - 1) % p.size();
      ccw[i] = (i + 1) % p.size();
    }
    std::random_shuffle(ids.begin(), ids.end());
    for (int i = ids.size() - 1; i >= 2; --i) {
      int u = ids[i];
      std::tie(ccw[cw[u]], cw[ccw[u]]) = std::make_pair(ccw[u], cw[u]);
    }

    std::vector<std::list<Edge>> lines(p.size());
    auto bind_rev_edge = [&](std::list<Edge>::iterator lhs,
                             std::list<Edge>::iterator rhs) {
      lhs->rev = rhs;
      rhs->rev = lhs;
    };

    bind_rev_edge(neighbor[ids[0]].emplace(neighbor[ids[0]].begin(), ids[1]),
                  neighbor[ids[1]].emplace(neighbor[ids[1]].begin(), ids[0]));

    for (int i = 2; i < ids.size(); ++i) {
      int u = ids[i];
      int cur = ccw[u];
      auto cur_iter = neighbor[cur].begin();
      while (1) {
        while (cur_iter != neighbor[cur].end()) {
          auto next_iter = std::next(cur_iter);
          if (next_iter == neighbor[cur].end()) {
            if (cur != cw[u]) break;
          } else {
            if (Circle::side(p[cur], p[cur_iter->v], p[next_iter->v], p[u]) < 0)
              break;
          }
          neighbor[cur].erase(cur_iter++);
        }
        bind_rev_edge(neighbor[u].emplace(neighbor[u].begin(), cur),
                      neighbor[cur].emplace(cur_iter, u));
        if (cur == cw[u]) break;
        std::tie(cur, cur_iter) =
            std::make_pair(cur_iter->v, std::next(cur_iter->rev));
      }
    }

    for (int u = 0; u < p.size(); ++u)
      for (auto e : neighbor[u])
        if (u < e.v) edges.emplace_back(u, e.v);
    return edges;
  }
};

struct PlanarGraphDuality {
  struct DirectionalEdge {
    int v, id = -1;
    Point direction;
    int rev;
    DirectionalEdge(int v, Point direction) : v(v), direction(direction) {}
  };

  // return all points' id in each faces and the edges between faces
  static std::pair<std::vector<std::vector<int>>,
                   std::vector<std::pair<int, int>>>
  solve(const std::vector<Point>& p,
        const std::vector<std::pair<int, int>>& edges) {
    std::vector<DirectionalEdge> directional_edges;
    directional_edges.reserve(edges.size() * 2);
    std::vector<std::vector<int>> out_edges(p.size());
    for (auto [u, v] : edges) {
      out_edges[u].push_back(directional_edges.size());
      directional_edges.emplace_back(v, p[v] - p[u]);
      out_edges[v].push_back(directional_edges.size());
      directional_edges.emplace_back(u, p[u] - p[v]);
      directional_edges.end()[-1].rev = out_edges[u].back();
      directional_edges.end()[-2].rev = out_edges[v].back();
    }
    const auto comp = [&, t_comp = Point::PolarComparer()](int lhs, int rhs) {
      return t_comp(directional_edges[lhs].direction,
                    directional_edges[rhs].direction);
    };
    for (int u = 0; u < p.size(); ++u)
      std::sort(out_edges[u].begin(), out_edges[u].end(), comp);
    std::vector<std::vector<int>> faces;
    for (int u = 0; u < p.size(); ++u) {
      for (int e_id : out_edges[u]) {
        if (~directional_edges[e_id].id) continue;
        std::vector<int> pids;
        for (int cur_e_id = e_id;;) {
          if (~directional_edges[cur_e_id].id) break;
          directional_edges[cur_e_id].id = faces.size();
          int v = directional_edges[cur_e_id].v;
          pids.push_back(v);
          auto it = std::lower_bound(out_edges[v].begin(), out_edges[v].end(),
                                     directional_edges[cur_e_id].rev, comp);
          assert(*it == directional_edges[cur_e_id].rev);
          if (it == out_edges[v].begin())
            cur_e_id = out_edges[v].back();
          else
            cur_e_id = *std::prev(it);
        }
        faces.push_back(pids);
      }
    }
    std::vector<std::pair<int, int>> face_edges;
    for (int u = 0; u < p.size(); ++u)
      for (int e_id : out_edges[u]) {
        int rev_id = directional_edges[e_id].rev;
        if (e_id < rev_id) continue;
        face_edges.emplace_back(directional_edges[e_id].id,
                                directional_edges[rev_id].id);
      }
    return std::make_pair(faces, face_edges);
  }
};

struct Voronoi {
  static constexpr long double kBoundaryInf = 50000;

  // should be guaranteed that
  // 1. all points are pairwise distinct
  // 2. boundary had better to be a convex
  // 3. all points are inside boundary
  static std::vector<std::vector<Point>> nearest(
      const std::vector<Point>& p,
      const std::vector<Line>& boundary = {
          Line(Point(-kBoundaryInf, -kBoundaryInf), Point(1, 0)),
          Line(Point(kBoundaryInf, -kBoundaryInf), Point(0, 1)),
          Line(Point(kBoundaryInf, kBoundaryInf), Point(-1, 0)),
          Line(Point(-kBoundaryInf, kBoundaryInf), Point(0, -1)),
      }) {
    // p0 in the left
    auto bisector = [&](const Point& p0, const Point& p1) {
      auto dir = (p1 - p0).rotate90();
      auto mid = (p0 + p1) / 2;
      return Line(mid, dir);
    };
    auto edges = Triangulation::nearest(p);
    std::vector<std::vector<Line>> limit(p.size(), boundary);
    for (auto [i, j] : edges) {
      limit[i].push_back(bisector(p[i], p[j]));
      limit[j].push_back(bisector(p[j], p[i]));
    }
    std::vector<std::vector<Point>> regions(p.size());
    for (int i = 0; i < p.size(); ++i) {
      regions[i] = HalfPlaneIntersection::solve(limit[i]);
    }
    return regions;
  }

  // should be guaranteed that
  // 1. p is strictly convex
  // 2. boundary had better to be a convex
  // 3. all points are inside boundary
  static std::vector<std::vector<Point>> furthest(
      const std::vector<Point>& p,
      const std::vector<Line>& boundary = {
          Line(Point(-kBoundaryInf, -kBoundaryInf), Point(1, 0)),
          Line(Point(kBoundaryInf, -kBoundaryInf), Point(0, 1)),
          Line(Point(kBoundaryInf, kBoundaryInf), Point(-1, 0)),
          Line(Point(-kBoundaryInf, kBoundaryInf), Point(0, -1)),
      }) {
    // p0 in the right
    auto bisector = [&](const Point& p0, const Point& p1) {
      auto dir = (p0 - p1).rotate90();
      auto mid = (p0 + p1) / 2;
      return Line(mid, dir);
    };
    auto edges = Triangulation::furthest(p);
    std::vector<std::vector<Line>> limit(p.size(), boundary);
    for (auto [i, j] : edges) {
      limit[i].push_back(bisector(p[i], p[j]));
      limit[j].push_back(bisector(p[j], p[i]));
    }
    std::vector<std::vector<Point>> regions(p.size());
    for (int i = 0; i < p.size(); ++i) {
      regions[i] = HalfPlaneIntersection::solve(limit[i]);
    }
    return regions;
  }
};

}  // namespace geometry2d

using namespace geometry2d;