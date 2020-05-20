#ifndef UI_STATISMOUI_H
#define UI_STATISMOUI_H

#include "statismo/ITK/itkIO.h"
#include "statismo/ITK/itkStandardMeshRepresenter.h"
#include "statismo/ITK/itkStatisticalModel.h"

#include <itkRigid3DTransform.h>
#include <itkEuler3DTransform.h>
#include <itkMesh.h>

#include <itkImage.h>
#include <itkImageFileReader.h>
#include <vnl/algo/vnl_svd.h>


// foreward declarations for
namespace ui
{
class Group;
class ShapeModelView;
class ShapeModelTransformationView;
class TriangleMesh;
class TriangleMeshView;
class ImageView;
} // namespace ui

namespace StatismoUI
{
class Group
{
public:
  Group(const std::string & name, int id)
    : m_name(name)
    , m_id(id)
  {}

  int
  GetId() const
  {
    return m_id;
  }

  std::string
  GetName() const
  {
    return m_name;
  }


private:
  std::string m_name;
  int         m_id;
};

class ShapeTransformation
{
public:
  ShapeTransformation(vnl_vector<float> && coefficients)
    : m_coefficients(std::move(coefficients))
  {}

  vnl_vector<float>
  GetCoefficients() const
  {
    return m_coefficients;
  }

  ShapeTransformation &
  SetShapeTransformation(vnl_vector<float> && v)
  {
    m_coefficients = std::move(v);
    return *this;
  }

private:
  vnl_vector<float> m_coefficients;
};

class PoseTransformation
{
  using Rigid3DTransformType = itk::Rigid3DTransform<float>;
  using PointType = Rigid3DTransformType::InputPointType;
  using Euler3DTransformType = itk::Euler3DTransform<float>;

public:
  PoseTransformation(const itk::Rigid3DTransform<float> & rigidTransform)
  {
    m_eulerTransform = Euler3DTransformType::New();
    m_eulerTransform->SetMatrix(rigidTransform.GetMatrix());
    m_eulerTransform->SetTranslation(rigidTransform.GetTranslation());
    m_eulerTransform->SetCenter(rigidTransform.GetCenter());
  }

  double
  GetAngleX() const
  {
    return m_eulerTransform->GetAngleX();
  }

  double
  GetAngleY() const
  {
    return m_eulerTransform->GetAngleY();
  }

  double
  GetAngleZ() const
  {
    return m_eulerTransform->GetAngleZ();
  }

  vnl_vector<float>
  GetTranslation() const
  {
    return m_eulerTransform->GetTranslation().GetVnlVector();
  }

  PointType
  GetCenter() const
  {
    return m_eulerTransform->GetCenter();
  };

private:
  Euler3DTransformType::Pointer m_eulerTransform;
};

class ShapeModelTransformationView
{
public:
  ShapeModelTransformationView(int id, PoseTransformation p, ShapeTransformation s)
    : m_shapeTransformation(std::move(s))
    , m_poseTransformation(std::move(p))
    , m_id(id)
  {}

  int
  GetId() const
  {
    return m_id;
  }

  ShapeTransformation
  GetShapeTransformation() const
  {
    return m_shapeTransformation;
  }

  ShapeModelTransformationView
  SetShapeTransformation(const ShapeTransformation & st)
  {
    m_shapeTransformation = st;
    return *this;
  }

  ShapeModelTransformationView
  SetShapeTransformation(ShapeTransformation && st)
  {
    m_shapeTransformation = std::move(st);
    return *this;
  }

  PoseTransformation
  GetPoseTransformation() const
  {
    return m_poseTransformation;
  }

  ShapeModelTransformationView &
  SetPoseTransformation(const PoseTransformation & pt)
  {
    m_poseTransformation = pt;
    return *this;
  }

  ShapeModelTransformationView &
  SetPoseTransformation(PoseTransformation && pt)
  {
    m_poseTransformation = std::move(pt);
    return *this;
  }

private:
  ShapeTransformation m_shapeTransformation;
  PoseTransformation  m_poseTransformation;
  int                 m_id;
};

struct Color
{
  Color(int r, int g, int b)
    : red(r)
    , green(g)
    , blue(b)
  {}

  int red;
  int green;
  int blue;
};


class TriangleMeshView
{
public:
  TriangleMeshView(int id, const Color color, double opacity, int lineWidth)
    : m_id(id)
    , m_color(color)
    , m_opacity(opacity)
    , m_lineWidth(lineWidth)
  {}


  int
  GetId() const
  {
    return m_id;
  }

  Color
  GetColor() const
  {
    return m_color;
  }

  TriangleMeshView &
  SetColor(const Color & c)
  {
    m_color = c;
    return *this;
  }


  double
  GetOpacity() const
  {
    return m_opacity;
  }


  TriangleMeshView &
  SetOpacity(double opacity)
  {
    m_opacity = opacity;
    return *this;
  }

  int
  GetLineWidth() const
  {
    return m_lineWidth;
  };

  TriangleMeshView &
  SetLineWidth(int lineWidth)
  {
    m_lineWidth = lineWidth;
    return *this;
  }

private:
  int    m_id;
  Color  m_color;
  double m_opacity;
  int    m_lineWidth;
};


class ShapeModelView
{
public:
  ShapeModelView(const TriangleMeshView &             triangleMeshView,
                 const ShapeModelTransformationView & shapeModelTransformationView)
    : m_triangleMeshView(triangleMeshView)
    , m_shapeModelTransformationView(shapeModelTransformationView)
  {}

  TriangleMeshView
  GetTriangleMeshView() const
  {
    return m_triangleMeshView;
  }

  ShapeModelTransformationView
  GetShapeModelTransformationView() const
  {
    return m_shapeModelTransformationView;
  }

private:
  TriangleMeshView             m_triangleMeshView;
  ShapeModelTransformationView m_shapeModelTransformationView;
};


class ImageView
{
public:
  ImageView(int id, double window, double level, double opacity)
    : m_id(id)
    , m_window(window)
    , m_level(level)
    , m_opacity(opacity)
  {}


  int
  GetId() const
  {
    return m_id;
  }

  double
  GetWindow() const
  {
    return m_window;
  }

  ImageView &
  SetWindow(double window)
  {
    m_window = window;
    return *this;
  }

  double
  GetLevel() const
  {
    return m_level;
  }

  ImageView &
  SetLevel(double level)
  {
    m_level = level;
    return *this;
  }

  double
  GetOpacity() const
  {
    return m_opacity;
  }


  ImageView &
  SetOpacity(double opacity)
  {
    m_opacity = opacity;
    return *this;
  }


private:
  int    m_id;
  double m_window;
  double m_level;
  double m_opacity;
};


class StatismoUI
{
  using MeshType = itk::Mesh<float, 3>;
  using PointType = MeshType::PointType;
  using ImageType = itk::Image<short, 3>;
  using StatisticalModelType = itk::StatisticalModel<MeshType>;

public:
  StatismoUI();
  ~StatismoUI();

  Group
  createGroup(const std::string & name);

  TriangleMeshView
  showTriangleMesh(const Group & group, const MeshType * mesh, const std::string & name);

  void
  showPointCloud(const Group & group, const std::list<PointType> points, const std::string & name);

  ShapeModelView
  showStatisticalShapeModel(const Group & group, const StatisticalModelType * ssm, const std::string & name);

  void
  showLandmark(const Group & group, const PointType & point, const vnl_matrix<double> cov, const std::string & name);

  ImageView
  showImage(const Group & group, const ImageType * image, const std::string & name);

  void
  updateShapeModelTransformationView(const ShapeModelTransformationView & smv);

  void
  updateTriangleMeshView(const TriangleMeshView & tmv);

  void
  updateImageView(const ImageView & imv);

  void
  removeGroup(const Group & group);
  void
  removeTriangleMesh(const TriangleMeshView & tmv);
  void
  removeImage(const ImageView & imv);
  void
  removeShapeModelTransformation(const ShapeModelTransformationView & ssmtview);
  void
  removeShapeModel(const ShapeModelView & ssmview);

private:
  ui::Group
  groupToThriftGroup(const Group & group);

  ShapeModelView
  shapeModelViewFromThrift(const ui::ShapeModelView & smvThrift);

  ui::ShapeModelView
  shapeModelViewToThriftShapeModelView(const ShapeModelView & tv);

  ShapeModelTransformationView
  shapeModelTransformationViewFromThrift(const ui::ShapeModelTransformationView & tvthrift);

  ui::ShapeModelTransformationView
  shapeModelTransformationViewToThrift(const ShapeModelTransformationView & tv);

  ui::ImageView
  imageViewToThriftImageView(const ImageView & iv);

  TriangleMeshView
  triangleMeshViewFromThriftMeshView(ui::TriangleMeshView tmvThrift);

  ui::TriangleMeshView
  thriftMeshViewFromTriangleMeshView(const TriangleMeshView & tmv);

  ui::TriangleMesh
  meshToThriftMesh(const MeshType * mesh);
};

} // namespace StatismoUI

#endif // UI_STATISMOUI_H
