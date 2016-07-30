
namespace cpp ui // The generated c++ code will be put inside example namespace

struct Point {
   1: required double x;
   2: required double y;
   3: required double z;
}

typedef list<Point> PointList


struct TriangleCell {
    1: required i32 id1;
    2: required i32 id2;
    3: required i32 id3;
}

typedef list<TriangleCell> TriangleCellList

struct TriangleMesh {
    1: required PointList vertices;
    2: required TriangleCellList topology;
}

service UI {
  void showPointCloud(1:PointList p);
  void showTriangleMesh(1:TriangleMesh m);
}


