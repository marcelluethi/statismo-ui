#include "StatismoUI.h"
#include "thrift/UI.h"

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/protocol/TCompactProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include <memory>

namespace StatismoUI
{

class ServerConnection
{
public:
  void
  open()
  {
    m_transport->open();
  }
  void
  close()
  {
    m_transport->close();
  }

  ui::UIClient &
  GetThriftUI()
  {
    return m_ui;
  }

  ServerConnection()
    : m_socket(std::make_shared<apache::thrift::transport::TSocket>("localhost", 8000))
    , m_transport(std::make_shared<apache::thrift::transport::TFramedTransport>(m_socket))
    , m_protocol(std::make_shared<apache::thrift::protocol::TBinaryProtocol>(m_transport))
    , m_ui(m_protocol)
  {}

private:
  static ServerConnection * theinstance;

  std::shared_ptr<apache::thrift::transport::TSocket>    m_socket;
  std::shared_ptr<apache::thrift::transport::TTransport> m_transport;
  std::shared_ptr<apache::thrift::protocol::TProtocol>   m_protocol;
  ui::UIClient                                           m_ui;
};

ServerConnection &
ServerConnectionInstance()
{
  static ServerConnection con{};
  return con;
}


StatismoUI::StatismoUI()
{
  ServerConnectionInstance().open();
}

StatismoUI::~StatismoUI()
{
  ServerConnectionInstance().close();
}

Group
StatismoUI::createGroup(const std::string & name)
{
  ui::Group g;
  ServerConnectionInstance().GetThriftUI().createGroup(g, name);
  return Group(g.name, g.id);
}

TriangleMeshView
StatismoUI::showTriangleMesh(const Group & group, const MeshType * mesh, const std::string & name)
{
  ui::TriangleMesh thriftMesh = meshToThriftMesh(mesh);

  ui::TriangleMeshView tmvThrift;
  ServerConnectionInstance().GetThriftUI().showTriangleMesh(tmvThrift, groupToThriftGroup(group), thriftMesh, name);

  TriangleMeshView tmv = triangleMeshViewFromThriftMeshView(tmvThrift);
  return tmv;
}

void
StatismoUI::showPointCloud(const Group & group, const std::list<PointType> points, const std::string & name)
{
  ui::PointList pts;

  for (const auto & pt : points)
  {
    ui::Point3D         p;
    p.x = pt.GetElement(0);
    p.y = pt.GetElement(1);
    p.z = pt.GetElement(2);
    pts.push_back(p);
  }
  ServerConnectionInstance().GetThriftUI().showPointCloud(groupToThriftGroup(group), pts, name);
}

ShapeModelView
StatismoUI::showStatisticalShapeModel(const Group & group, const StatisticalModelType * ssm, const std::string & name)
{
  ui::StatisticalShapeModel model;
  model.reference = meshToThriftMesh(ssm->GetRepresenter()->GetReference());

  ui::KLBasis             klBasis;
  ui::ListOfDoubleVectors eigenVectors;
  ui::DoubleVector        eigenValues;
  ui::DoubleVector        meanVector;

  vnl_matrix<float>    pcaBasisMatrix = ssm->GetOrthonormalPCABasisMatrix();
  vnl_vector<float>    meanVecStatismo = ssm->GetMeanVector();
  statismo::VectorType refVec = ssm->GetRepresenter()->SampleToSampleVector(ssm->GetRepresenter()->GetReference());

  for (unsigned i = 0; i < pcaBasisMatrix.cols(); ++i)
  {
    ui::DoubleVector v;
    for (unsigned j = 0; j < pcaBasisMatrix.rows(); ++j)
    {
      v.push_back(pcaBasisMatrix(j, i));
    }
    eigenVectors.push_back(v);
    eigenValues.push_back(ssm->GetPCAVarianceVector()[i]);
  }


  for (unsigned j = 0; j < pcaBasisMatrix.rows(); j++)
  {
    // statismo stores the mean not as a vector, but as point positions (i.e. ref + df)
    // we need to subtract the reference to get df alone
    meanVector.push_back(meanVecStatismo[j] - refVec[j]);
  }


  klBasis.eigenvalues = eigenValues;
  klBasis.eigenvectors = eigenVectors;
  model.klbasis = klBasis;
  model.mean = meanVector;
  // ui::ShapeModelTransformationView tv;
  ui::ShapeModelView thriftSSMView;
  ServerConnectionInstance().GetThriftUI().showStatisticalShapeModel(
    thriftSSMView, groupToThriftGroup(group), model, name);

  return shapeModelViewFromThrift(thriftSSMView);
}

void
StatismoUI::showLandmark(const Group &            group,
                         const PointType &        point,
                         const vnl_matrix<double> cov,
                         const std::string &      name)
{
  if (cov.cols() != 3 || cov.rows() != 3)
  {
    throw std::invalid_argument(" cov matrix must be 3 x 3");
  }
  ui::Landmark landmark;
  landmark.name = name;

  ui::Point3D tpoint;
  tpoint.x = point.GetElement(0);
  tpoint.y = point.GetElement(1);
  tpoint.z = point.GetElement(2);

  landmark.point = tpoint;

  vnl_svd<double>           svd(cov);
  ui::UncertaintyCovariance tcov;
  ui::Vector3D              varianceVec;
  varianceVec.x = svd.W(0);
  varianceVec.y = svd.W(1);
  varianceVec.z = svd.W(2);

  ui::Vector3D pc1;
  ui::Vector3D pc2;
  ui::Vector3D pc3;
  pc1.x = svd.U(0, 0);
  pc1.y = svd.U(0, 1);
  pc1.z = svd.U(0, 2);
  pc2.x = svd.U(1, 0);
  pc2.y = svd.U(1, 1);
  pc2.z = svd.U(1, 2);
  pc3.x = svd.U(2, 0);
  pc3.y = svd.U(2, 1);
  pc3.z = svd.U(2, 2);

  tcov.variances = varianceVec;
  tcov.principalAxis1 = pc1;
  tcov.principalAxis2 = pc2;
  tcov.principalAxis3 = pc3;

  landmark.uncertainty = tcov;
  ServerConnectionInstance().GetThriftUI().showLandmark(groupToThriftGroup(group), landmark, name);
}

ImageView
StatismoUI::showImage(const Group & group, const ImageType * image, const std::string & name)
{
  ui::Point3D origin;
  origin.x = image->GetOrigin().GetElement(0);
  origin.y = image->GetOrigin().GetElement(1);
  origin.z = image->GetOrigin().GetElement(2);

  ui::IntVector3D size;
  size.i = image->GetLargestPossibleRegion().GetSize().GetElement(0);
  size.j = image->GetLargestPossibleRegion().GetSize().GetElement(1);
  size.k = image->GetLargestPossibleRegion().GetSize().GetElement(2);

  ui::Vector3D spacing;
  spacing.x = image->GetSpacing().GetElement(0);
  spacing.y = image->GetSpacing().GetElement(1);
  spacing.z = image->GetSpacing().GetElement(2);

  ui::ImageDomain domain;
  domain.origin = origin;
  domain.spacing = spacing;
  domain.size = size;

  ui::ImageData                         data;
  ImageType::PixelContainerConstPointer pixelContainer = image->GetPixelContainer();
  for (unsigned i = 0; i < pixelContainer->Size(); ++i)
  {
    data.push_back((*pixelContainer)[i]);
  }

  ui::Image thriftImage;
  thriftImage.data = data;
  thriftImage.domain = domain;

  ui::ImageView thriftImageView;
  ServerConnectionInstance().GetThriftUI().showImage(thriftImageView, groupToThriftGroup(group), thriftImage, name);

  ImageView iv(thriftImageView.id, thriftImageView.window, thriftImageView.level, thriftImageView.opacity);
  return iv;
}

void
StatismoUI::updateShapeModelTransformationView(const ShapeModelTransformationView & smv)
{
  ServerConnectionInstance().GetThriftUI().updateShapeModelTransformation(shapeModelTransformationViewToThrift(smv));
}

void
StatismoUI::updateTriangleMeshView(const TriangleMeshView & tmv)
{
  ui::TriangleMeshView thriftTmv = thriftMeshViewFromTriangleMeshView(tmv);
  ServerConnectionInstance().GetThriftUI().updateTriangleMeshView(thriftTmv);
}

ui::Group
StatismoUI::groupToThriftGroup(const Group & group)
{
  ui::Group thriftGroup;
  thriftGroup.id = group.GetId();
  thriftGroup.name = group.GetName().c_str();
  return thriftGroup;
}

void
StatismoUI::updateImageView(const ImageView & imageView)
{
  ui::ImageView thriftImageView = imageViewToThriftImageView(imageView);


  ServerConnectionInstance().GetThriftUI().updateImageView(thriftImageView);
}

TriangleMeshView
StatismoUI::triangleMeshViewFromThriftMeshView(ui::TriangleMeshView tmvThrift)
{
  Color color(tmvThrift.color.r, tmvThrift.color.g, tmvThrift.color.b);

  TriangleMeshView tmv(tmvThrift.id, color, tmvThrift.opacity, tmvThrift.lineWidth);
  return tmv;
}

ui::TriangleMeshView
StatismoUI::thriftMeshViewFromTriangleMeshView(const TriangleMeshView & tmv)
{
  ui::TriangleMeshView thriftTmv;
  Color                color = tmv.GetColor();
  thriftTmv.id = tmv.GetId();
  thriftTmv.color.r = color.red;
  thriftTmv.color.g = color.green;
  thriftTmv.color.b = color.blue;
  thriftTmv.opacity = tmv.GetOpacity();
  thriftTmv.lineWidth = tmv.GetLineWidth();
  return thriftTmv;
}

ShapeModelTransformationView
StatismoUI::shapeModelTransformationViewFromThrift(const ui::ShapeModelTransformationView & tvthrift)
{
  vnl_vector<float> coeffs(tvthrift.shapeTransformation.coefficients.size());
  for (unsigned i = 0; i < coeffs.size(); ++i)
  {
    coeffs[i] = tvthrift.shapeTransformation.coefficients[i];
  }

  itk::Euler3DTransform<float>::Pointer        eulerTransform = itk::Euler3DTransform<float>::New();
  itk::Euler3DTransform<float>::InputPointType center;
  center.SetElement(0, tvthrift.poseTransformation.rotation.center.x);
  center.SetElement(1, tvthrift.poseTransformation.rotation.center.y);
  center.SetElement(2, tvthrift.poseTransformation.rotation.center.z);

  eulerTransform->SetCenter(center);
  eulerTransform->SetRotation(tvthrift.poseTransformation.rotation.angleX,
                              tvthrift.poseTransformation.rotation.angleY,
                              tvthrift.poseTransformation.rotation.angleZ);
  itk::Vector<float> v(3);
  v.SetElement(0, tvthrift.poseTransformation.translation.x);
  v.SetElement(1, tvthrift.poseTransformation.translation.y);
  v.SetElement(2, tvthrift.poseTransformation.translation.z);
  eulerTransform->SetTranslation(v);

  PoseTransformation pose(*eulerTransform);
  return ShapeModelTransformationView(tvthrift.id, pose, ShapeTransformation(std::move(coeffs)));
}

ShapeModelView
StatismoUI::shapeModelViewFromThrift(const ui::ShapeModelView & smvThrift)
{
  TriangleMeshView             tmv = triangleMeshViewFromThriftMeshView(smvThrift.meshView);
  ShapeModelTransformationView smv = shapeModelTransformationViewFromThrift(smvThrift.shapeModelTransformationView);
  ShapeModelView               ssmView(tmv, smv);
  return ssmView;
}

ui::ShapeModelTransformationView
StatismoUI::shapeModelTransformationViewToThrift(const ShapeModelTransformationView & tv)
{
  ui::ShapeModelTransformationView tvthrift;
  ui::RigidTransformation          rtThrift;
  ui::EulerTransform               eulerThrift;

  eulerThrift.angleX = tv.GetPoseTransformation().GetAngleX();
  eulerThrift.angleY = tv.GetPoseTransformation().GetAngleY();
  eulerThrift.angleZ = tv.GetPoseTransformation().GetAngleZ();
  ui::Point3D center;

  center.x = tv.GetPoseTransformation().GetCenter()[0];
  center.y = tv.GetPoseTransformation().GetCenter()[1];
  center.z = tv.GetPoseTransformation().GetCenter()[2];

  eulerThrift.center = center;
  rtThrift.rotation = eulerThrift;

  ui::TranslationTransform translationThrift;
  translationThrift.x = tv.GetPoseTransformation().GetTranslation()[0];
  translationThrift.y = tv.GetPoseTransformation().GetTranslation()[1];
  translationThrift.z = tv.GetPoseTransformation().GetTranslation()[2];
  rtThrift.translation = translationThrift;

  ui::ShapeTransformation stThrift;
  ui::DoubleVector        coeffsThrift;

  vnl_vector<float> coeffs = tv.GetShapeTransformation().GetCoefficients();
  for (unsigned i = 0; i < coeffs.size(); ++i)
  {
    coeffsThrift.push_back(coeffs[i]);
  }

  stThrift.coefficients = coeffsThrift;
  tvthrift.shapeTransformation = stThrift;
  tvthrift.poseTransformation = rtThrift;
  tvthrift.id = tv.GetId();
  return tvthrift;
}

ui::TriangleMesh
StatismoUI::meshToThriftMesh(const MeshType * mesh)
{
  ui::PointList pts;

  for (unsigned i = 0; i < mesh->GetNumberOfPoints(); i++)
  {
    ui::Point3D         p;
    MeshType::PointType pt = mesh->GetPoint(i);
    p.x = pt.GetElement(0);
    p.y = pt.GetElement(1);
    p.z = pt.GetElement(2);
    pts.push_back(p);
  }

  ui::TriangleCellList cells;
  for (unsigned i = 0; i < mesh->GetNumberOfCells(); i++)
  {
    ui::TriangleCell     c;
    MeshType::CellType * cell = mesh->GetCells()->GetElement(i);
    c.id1 = cell->GetPointIdsContainer().GetElement(0);
    c.id2 = cell->GetPointIdsContainer().GetElement(1);
    c.id3 = cell->GetPointIdsContainer().GetElement(2);
    cells.push_back(c);
  }

  ui::TriangleMesh thriftMesh;
  thriftMesh.vertices = pts;
  thriftMesh.topology = cells;

  return thriftMesh;
}

ui::ImageView
StatismoUI::imageViewToThriftImageView(const ImageView & imageView)
{
  ui::ImageView thriftImageView;
  thriftImageView.id = imageView.GetId();
  thriftImageView.opacity = imageView.GetOpacity();
  thriftImageView.level = imageView.GetLevel();
  thriftImageView.window = imageView.GetWindow();
  return thriftImageView;
}

ui::ShapeModelView
StatismoUI::shapeModelViewToThriftShapeModelView(const ShapeModelView & smv)
{
  ui::ShapeModelView               thriftSmv;
  ui::ShapeModelTransformationView tssmView =
    shapeModelTransformationViewToThrift(smv.GetShapeModelTransformationView());
  ui::TriangleMeshView tMeshView = thriftMeshViewFromTriangleMeshView(smv.GetTriangleMeshView());
  thriftSmv.shapeModelTransformationView = tssmView;
  thriftSmv.meshView = tMeshView;
  return thriftSmv;
}

void
StatismoUI::removeGroup(const Group & group)
{
  ServerConnectionInstance().GetThriftUI().removeGroup(groupToThriftGroup(group));
}

void
StatismoUI::removeTriangleMesh(const TriangleMeshView & tmv)
{
  ui::TriangleMeshView thriftTmv = thriftMeshViewFromTriangleMeshView(tmv);
  ServerConnectionInstance().GetThriftUI().removeTriangleMesh(thriftTmv);
}

void
StatismoUI::removeImage(const ImageView & imv)
{
  ui::ImageView thriftImageView = imageViewToThriftImageView(imv);
  ServerConnectionInstance().GetThriftUI().removeImage(thriftImageView);
}

void
StatismoUI::removeShapeModelTransformation(const ShapeModelTransformationView & ssmtview)
{
  ui::ShapeModelTransformationView tssmView = shapeModelTransformationViewToThrift(ssmtview);
  ServerConnectionInstance().GetThriftUI().removeShapeModelTransformation(tssmView);
}
void
StatismoUI::removeShapeModel(const ShapeModelView & ssmview)
{
  ui::ShapeModelView smvThrift = shapeModelViewToThriftShapeModelView(ssmview);
  ServerConnectionInstance().GetThriftUI().removeShapeModel(smvThrift);
}

} // namespace StatismoUI