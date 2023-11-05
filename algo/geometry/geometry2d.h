namespace geometry2d {

constexpr double eps = 1e-9;
int fsign(double x) { return (x > eps) - (x < -eps); }

struct Point;
struct Point3D {
  double x, y, z;

  explicit Point3D(double x = 0, double y = 0, double z = 0)
      : x(x), y(y), z(z) {}

  explicit Point3D(const Point& p);

  Point3D operator-(const Point3D& p) const {
    return Point3D(x - p.x, y - p.y, z - p.z);
  }
  double inner_prod(const Point3D& p) const {
    return x * p.x + y * p.y + z * p.z;
  }
  Point3D cross_prod(const Point3D& p) const {
    return Point3D(y * p.z - z * p.y, -x * p.z + z * p.x, x * p.y - y * p.x);
  }
};

struct Point {
  double x, y;
  explicit Point(double x = 0, double y = 0) : x(x), y(y) {}
  Point operator+(const Point& p) const { return Point(x + p.x, y + p.y); }
  Point operator-(const Point& p) const { return Point(x - p.x, y - p.y); }
  Point operator/(const double& l) const { return Point(x / l, y / l); }
  Point operator*(const double& l) const { return Point(x * l, y * l); }
  Point unit() const {
    double l = len();
    assert(fsign(l) > 0);
    return *this / l;
  }
  double cross_prod(const Point& p) const { return x * p.y - y * p.x; }
  double inner_prod(const Point& p) const { return x * p.x + y * p.y; }
  double len() const { return std::sqrt(x * x + y * y); }
  double len2() const { return x * x + y * y; }
  // return (-pi,pi]
  double angle() const { return atan2(y, x); }
  int angle_sign() const {
    if (!y) return fsign(x) < 0;
    return fsign(y);
  }
  Point rotate90() const { return Point(-y, x); }

  static double cross_prod(const Point& o, const Point& a, const Point& b) {
    return (o - a).cross_prod(o - b);
  }
  // 1: outside
  // 0: edge
  // -1: inside
  static int circle_side(const Point& a, Point b, Point c, const Point& p) {
    if (fsign(cross_prod(a, b, c)) < 0) std::swap(b, c);
    Point3D a3(a), b3(b), c3(c), p3(p);
    b3 = b3 - a3;
    c3 = c3 - a3;
    p3 = p3 - a3;
    Point3D f = b3.cross_prod(c3);
    return fsign(p3.inner_prod(f));
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
      int angle_sign_diff = lhs.angle_sign() - rhs.angle_sign();
      if (angle_sign_diff) return angle_sign_diff < 0;
      int cross_prod_sign = fsign(lhs.cross_prod(rhs));
      if (cross_prod_sign) return cross_prod_sign > 0;
      return lhs.len2() < rhs.len2();
    }
  };
};

Point3D::Point3D(const Point& p) : Point3D(p.x, p.y, p.x * p.x + p.y * p.y) {}

struct Line {
  Point pivot, unit_direction;
  Line(Point pivot, Point direction)
      : pivot(pivot), unit_direction(direction.unit()) {}
  double angle() const { return unit_direction.angle(); }
  // 1: on the left
  // 0: in the line
  // -1: on the right
  int side(const Point& p) {
    return fsign(unit_direction.cross_prod(p - pivot));
  }
  bool is_parallel(const Line& l) {
    return fsign(unit_direction.cross_prod(l.unit_direction)) == 0;
  }
  bool is_same_direction(const Line& l) {
    return is_parallel(l) && unit_direction.inner_prod(l.unit_direction) > 0;
  }
  Point intersection(const Line& l) {
    assert(!is_parallel(l));
    return pivot + unit_direction *
                       (l.unit_direction.cross_prod(l.pivot - pivot)) /
                       l.unit_direction.cross_prod(unit_direction);
  }

  friend std::ostream& operator<<(std::ostream& os, Line l) {
    return os << "(" << l.pivot << "," << l.unit_direction << ")";
  }
};

struct Segment {
  Point a, b;
  Segment(Point a, Point b) : a(a), b(b) {}
  bool has_intersection(const Segment& s) {
    return fsign(Point::cross_prod(a, b, s.a)) *
                   fsign(Point::cross_prod(a, b, s.b)) <
               0 &&
           fsign(Point::cross_prod(s.a, s.b, a)) *
                   fsign(Point::cross_prod(s.a, s.b, b)) <
               0;
  }

  friend std::ostream& operator<<(std::ostream& os, Segment s) {
    return os << "(" << s.a << "," << s.b << ")";
  }
};

struct Polygon {
  static bool is_convex_in_ccw(const std::vector<Point>& p) {
    for (int i = 0; i < p.size(); ++i) {
      int l = (i + p.size() - 1) % p.size();
      int r = (i + 1) % p.size();
      if (Point::cross_prod(p[i], p[l], p[r]) > 0) return false;
    }
    return true;
  }
};

// return the intersection convex in ccw, should be guaranteed that the
// intersection is finite.
struct HalfPlaneIntersection {
  static std::vector<Point> solve(std::vector<Line> lines) {
    sort(lines.begin(), lines.end(),
         [comp = Point::PolarComparer()](auto l1, auto l2) {
           if (l1.is_same_direction(l2)) {
             return l1.side(l2.pivot) < 0;
           } else {
             return comp(l1.unit_direction, l2.unit_direction);
           }
         });

    std::deque<Line> key_lines;
    std::deque<Point> key_points;
    for (int i = 0; i < lines.size(); ++i) {
      if (i > 0 && lines[i - 1].is_same_direction(lines[i])) continue;
      while (key_points.size() && lines[i].side(key_points.back()) <= 0) {
        ++CNT;
        key_lines.pop_back();
        key_points.pop_back();
      }
      while (key_points.size() && lines[i].side(key_points.front()) <= 0) {
        ++CNT;
        key_lines.pop_front();
        key_points.pop_front();
      }
      if (key_lines.size()) {
        // since it's guaranteed that the intersection is finite, therefore must
        // be empty.
        if (lines[i].is_parallel(key_lines.back())) return {};
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
  // should be guaranteed that all points are pairwise distinct
  static std::vector<std::pair<int, int>> delaunay(std::vector<Point> p) {
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

      for (int u = 0; u < p.size(); ++u)
        for (auto e : neighbor[u])
          if (u < e.v) dbg(u, e.v);

      auto get_base_LR_edge = [&]() {
        std::vector<int> stk;
        for (int i = l; i <= r; ++i) {
          while (stk.size() >= 2 &&
                 fsign(Point::cross_prod(p[id[stk.end()[-2]]],
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
          if (fsign(Point::cross_prod(ptL, ptR, p[it->v])) > 0 &&
              (!~ch || Point::circle_side(ptL, ptR, p[ch], p[it->v]) < 0)) {
            ch = it->v;
            side = 0;
          }
        }
        for (auto it = neighbor[rd].begin(); it != neighbor[rd].end(); ++it) {
          if (fsign(Point::cross_prod(ptR, p[it->v], ptL)) > 0 &&
              (!~ch || Point::circle_side(ptL, ptR, p[ch], p[it->v]) < 0)) {
            ch = it->v;
            side = 1;
          }
        }
        if (!~ch) break;
        assert(side == 0 || side == 1);
        if (!side) {
          for (auto it = neighbor[ld].begin(); it != neighbor[ld].end();) {
            if (Segment(ptL, p[it->v]).has_intersection(Segment(ptR, p[ch]))) {
              neighbor[it->v].erase(it->rev);
              neighbor[ld].erase(it++);
            } else {
              ++it;
            }
          }
          ld = ch;
        } else {
          for (auto it = neighbor[rd].begin(); it != neighbor[rd].end();) {
            if (Segment(ptR, p[it->v]).has_intersection(Segment(ptL, p[ch]))) {
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
  solve(std::vector<Point> p, std::vector<std::pair<int, int>> edges) {
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
    dbg(out_edges);
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
          dbg(cur_e_id);
          directional_edges[cur_e_id].id = faces.size();
          int v = directional_edges[cur_e_id].v;
          pids.push_back(v);
          auto it = std::lower_bound(out_edges[v].begin(), out_edges[v].end(),
                                     directional_edges[cur_e_id].rev, comp);
          dbg(cur_e_id, v, directional_edges[cur_e_id].rev,
              it - out_edges[v].begin());
          assert(*it == directional_edges[cur_e_id].rev);
          if (it == out_edges[v].begin())
            cur_e_id = out_edges[v].back();
          else
            cur_e_id = *std::prev(it);
        }
        dbg(pids);
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
  // should be guaranteed that
  // 1. all points are pairwise distinct
  // 2. boundary had better to be a convex
  // 3. all points are inside boundary
  static std::vector<std::vector<Point>> solve(
      const std::vector<Point>& p, const std::vector<Line>& boundary) {
    auto edges = Triangulation::delaunay(p);
    std::vector<std::vector<Line>> limit(p.size(), boundary);
    for (auto [i, j] : edges) {
      auto bisector = [&](const Point& p0, const Point& p1) {
        auto dir = (p1 - p0).rotate90();
        auto mid = (p0 + p1) / 2;
        return Line(mid, dir);
      };
      limit[i].push_back(bisector(p[i], p[j]));
      limit[j].push_back(bisector(p[j], p[i]));
    }
    std::vector<std::vector<Point>> areas(p.size());
    for (int i = 0; i < p.size(); ++i) {
      areas[i] = HalfPlaneIntersection::solve(limit[i]);
    }
    return areas;
  }
};

}  // namespace geometry2d

using namespace geometry2d;