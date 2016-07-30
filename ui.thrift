
namespace cpp ui // The generated c++ code will be put inside example namespace

struct Group {
    1: required i32 id;
    2: required string name;
}

struct Point3D {
   1: required double x;
   2: required double y;
   3: required double z;
}

struct Vector3D {
    1: required double x;
    2: required double y;
    3: required double z;
}

struct IntVector3D {
    1: required i32 i;
    2: required i32 j;
    3: required i32 k;
}

typedef list<Point3D> PointList


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

struct ImageDomain {
    1: required Point3D origin;
    2: required IntVector3D size;
    3: required Vector3D spacing;
}

typedef list<i16> ImageData

struct Image {
    1: required ImageDomain domain;
    2: required ImageData data;
}

service UI {
  Group createGroup(1:string name);
  void showPointCloud(1: Group g, 2:PointList p, 3:string name);
  void showTriangleMesh(1: Group g, 2:TriangleMesh m, 3:string name);
  void showImage(1: Group g, 2:Image img, 3:string name);
}


