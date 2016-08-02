//
// Created by marcel on 30.07.16.
//

#ifndef UI_STATISMOUI_H
#define UI_STATISMOUI_H


#include "gen-cpp/UI.h"

#include <thrift/protocol/TBinaryProtocol.h>

#include <thrift/protocol/TCompactProtocol.h>

#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include <iostream>
#include <itkRigid3DTransform.h>
#include <itkEuler3DTransform.h>
#include "itkMesh.h"
#include "itkStatismoIO.h"
#include "itkStandardMeshRepresenter.h"
#include "itkStatisticalModel.h"
#include "itkImage.h"
#include "itkImageFileReader.h"



class Group {
public:
    Group(const std::string& name, int id) : m_name(name), m_id(id) {}

    int GetId() const {return m_id;}
    const std::string& GetName() const { return m_name; }

private:
    std::string m_name;
    int m_id;
};

class ShapeTransformation {
public:
    ShapeTransformation(vnl_vector<float> coefficients)
            : m_coefficients(coefficients) {}

    vnl_vector<float> GetCoefficients() const {
        return m_coefficients;
    }

    ShapeTransformation SetShapeTransformation(const vnl_vector<float> v) const  {
        return ShapeTransformation(v);
    }

private:
    vnl_vector<float> m_coefficients;
};

class PoseTransformation {
    typedef itk::Rigid3DTransform<float> Rigid3DTransformType;
    typedef Rigid3DTransformType::InputPointType PointType;
    typedef itk::Euler3DTransform<float> Euler3DTransformType;

public:
    PoseTransformation(itk::Rigid3DTransform<float>* rigidTransform)
    {
        m_eulerTransform = Euler3DTransformType::New();
        m_eulerTransform->SetMatrix(rigidTransform->GetMatrix());
        m_eulerTransform->SetCenter(rigidTransform->GetCenter());
    }

    double GetAngleX() const {
        return m_eulerTransform->GetAngleX();
    }


    double GetAngleY() const {
        return m_eulerTransform->GetAngleY();
    }


    double GetAngleZ() const {
        return m_eulerTransform->GetAngleZ();
    }

    vnl_vector<float> GetTranslation() const {
        return m_eulerTransform->GetTranslation().GetVnlVector();
    }

    PointType GetCenter() const {
        return m_eulerTransform->GetCenter();
    };

private:
    Euler3DTransformType::Pointer m_eulerTransform;

};

class ShapeModelTransformationView {
public:
    ShapeModelTransformationView(int id, const PoseTransformation& p, const ShapeTransformation& s)
            : m_shapeTransformation(s), m_poseTransformation(p), m_id(id)
    {}

    int GetId() const {return m_id;}

    const ShapeTransformation& GetShapeTransformation() const {
        return m_shapeTransformation;
    }

    ShapeModelTransformationView SetShapeTransformation(const ShapeTransformation& st) const  {
        return ShapeModelTransformationView(m_id, m_poseTransformation, st);
    }

    const PoseTransformation& GetPoseTransformation() const {
        return m_poseTransformation;
    }

    ShapeModelTransformationView SetPoseTransformation(const PoseTransformation& pt) const {
        return ShapeModelTransformationView(m_id, pt, m_shapeTransformation);
    }

private:
    ShapeTransformation m_shapeTransformation;
    PoseTransformation m_poseTransformation;
    int m_id;
};



class StatismoUI {

//    using namespace apache::thrift;
//    using namespace apache::thrift::protocol;
//    using namespace apache::thrift::transport;

//    using namespace boost;

    typedef itk::Mesh<float, 3> MeshType;
    typedef itk::Image<short, 3> ImageType;
    typedef itk::StatisticalModel<MeshType> StatisticalModelType;

public:

    StatismoUI()  :
            m_socket(new apache::thrift::transport::TSocket("localhost", 8000)),
        m_transport(new apache::thrift::transport::TFramedTransport(m_socket)),
        m_protocol(new apache::thrift::protocol::TBinaryProtocol(m_transport)),
            m_ui(m_protocol)

    {
        m_transport->open();
    }

    ~StatismoUI() {
        m_transport->close();
    }

    Group createGroup(const std::string& name) {
        ui::Group g ;
        m_ui.createGroup(g, name);
        return Group(g.name , g.id);
    }

    void showTriangleMesh(const Group& group, const MeshType* mesh, const std::string& name) {
        ui::TriangleMesh thriftMesh = meshToThriftMesh(mesh);

        m_ui.showTriangleMesh(groupToThriftGroup(group), thriftMesh, name);
    }



    ShapeModelTransformationView showStatisticalShapeModel(const Group& group, const StatisticalModelType* ssm, const std::string& name) {
        ui::StatisticalShapeModel model;
        model.reference = meshToThriftMesh(ssm->GetRepresenter()->GetReference());

        ui::KLBasis klBasis;
        ui::ListOfDoubleVectors eigenVectors;
        ui::DoubleVector eigenValues;
        ui::DoubleVector meanVector;

        vnl_matrix<float> pcaBasisMatrix =  ssm->GetOrthonormalPCABasisMatrix();
        std::cout << "rows " << pcaBasisMatrix.rows() << std::endl;
        std::cout << "cols " << pcaBasisMatrix.cols() << std::endl;
        vnl_vector<float> meanVecStatismo = ssm->GetMeanVector();
        statismo::VectorType refVec = ssm->GetRepresenter()->SampleToSampleVector(ssm->GetRepresenter()->GetReference());

        for (unsigned i = 0; i < pcaBasisMatrix.cols(); ++i) {
            ui::DoubleVector v;
            for (unsigned j = 0; j < pcaBasisMatrix.rows(); ++j) {
                v.push_back(pcaBasisMatrix(j, i));
            }
            eigenVectors.push_back(v);
            eigenValues.push_back(ssm->GetPCAVarianceVector()[i]);
        }


        for (unsigned j = 0; j < pcaBasisMatrix.rows(); j++) {
            // statismo stores the mean not as a vector, but as point positions (i.e. ref + df)
            // we need to subtract the reference to get df alone
            meanVector.push_back(meanVecStatismo[j] - refVec[j]);
        }


        klBasis.eigenvalues = eigenValues;
        klBasis.eigenvectors = eigenVectors;
        model.klbasis = klBasis;
        model.mean = meanVector;
        ui::ShapeModelTransformationView tv;
        m_ui.showStatisticalShapeModel(tv, groupToThriftGroup(group), model, name);
        return shapeModelTransformationViewFromThrift(tv);
    }

    void showImage(const Group& group, const ImageType* image, const std::string& name) {
        // and now an image

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

        ui::ImageData data;
        ImageType::PixelContainerConstPointer pixelContainer =  image->GetPixelContainer();
        for (unsigned i = 0; i < pixelContainer->Size(); ++i) {
            data.push_back((*pixelContainer)[i]);
        }
        ui::Image thriftImage;
        thriftImage.data = data;
        thriftImage.domain = domain;

        m_ui.showImage(groupToThriftGroup(group), thriftImage, name);

    }

    void updateShapeModelTransformationView(const ShapeModelTransformationView& smv) {
        m_ui.updateShapeModelTransformation(shapeModelTransformationViewToThrift(smv));
    }

private:

    ui::Group groupToThriftGroup(const Group& group) {
        ui::Group thriftGroup;
        thriftGroup.id = group.GetId();
        thriftGroup.name = group.GetName().c_str();
        return thriftGroup;
    }


    ShapeModelTransformationView shapeModelTransformationViewFromThrift(const ui::ShapeModelTransformationView& tvthrift) {

        vnl_vector<float> coeffs(tvthrift.shapeTransformation.coefficients.size());
        for (unsigned i = 0; i < coeffs.size(); ++i) {
            coeffs[i] = tvthrift.shapeTransformation.coefficients[i];
        }


        itk::Euler3DTransform<float>::Pointer eulerTransform = itk::Euler3DTransform<float>::New();
        itk::Euler3DTransform<float>::InputPointType center;
        center.SetElement(0,  tvthrift.poseTransformation.rotation.center.x);
        center.SetElement(1,  tvthrift.poseTransformation.rotation.center.y);
        center.SetElement(2,  tvthrift.poseTransformation.rotation.center.z);

        eulerTransform->SetCenter(center);
        eulerTransform->SetRotation(tvthrift.poseTransformation.rotation.angleX, tvthrift.poseTransformation.rotation.angleY, tvthrift.poseTransformation.rotation.angleZ);
        itk::Vector<float> v(3);
        v.SetElement(0, tvthrift.poseTransformation.translation.x);
        v.SetElement(1, tvthrift.poseTransformation.translation.y);
        v.SetElement(2, tvthrift.poseTransformation.translation.z);
        eulerTransform->SetTranslation(v);

        return ShapeModelTransformationView(tvthrift.id, PoseTransformation(eulerTransform), ShapeTransformation(coeffs));

    }


    ui::ShapeModelTransformationView shapeModelTransformationViewToThrift(const ShapeModelTransformationView& tv) {
        ui::ShapeModelTransformationView tvthrift;
        ui::RigidTransformation rtThrift;
        ui::EulerTransform eulerThrift;

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
        ui::DoubleVector coeffsThrift;

        vnl_vector<float> coeffs = tv.GetShapeTransformation().GetCoefficients();
        for (unsigned i = 0; i < coeffs.size(); ++i) {
            coeffsThrift.push_back(coeffs[i]);
        }

        stThrift.coefficients = coeffsThrift;
        tvthrift.shapeTransformation = stThrift;
        tvthrift.poseTransformation = rtThrift;
        tvthrift.id = tv.GetId();
        return tvthrift;

    }

    ui::TriangleMesh meshToThriftMesh(const MeshType* mesh) {
        ui::PointList pts;

        for (unsigned i = 0; i < mesh->GetNumberOfPoints(); i++) {
            ui::Point3D p;
            MeshType::PointType pt = mesh->GetPoint(i);
            p.x = pt.GetElement(0);
            p.y = pt.GetElement(1);
            p.z = pt.GetElement(2);
            pts.push_back(p);
        }

        ui::TriangleCellList cells;
        for (unsigned i = 0; i < mesh->GetNumberOfCells(); i++) {
            ui::TriangleCell c;
            MeshType::CellType *cell = mesh->GetCells()->GetElement(i);
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

    boost::shared_ptr<apache::thrift::transport::TSocket> m_socket;
    boost::shared_ptr<apache::thrift::transport::TTransport> m_transport;
    boost::shared_ptr<apache::thrift::protocol::TProtocol> m_protocol;
    ui::UIClient m_ui;
};


#endif //UI_STATISMOUI_H
