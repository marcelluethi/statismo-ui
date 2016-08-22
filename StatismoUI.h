//
// Created by marcel on 30.07.16.
//

#ifndef UI_STATISMOUI_H
#define UI_STATISMOUI_H



#include <iostream>
#include <itkRigid3DTransform.h>
#include <itkEuler3DTransform.h>
#include "itkMesh.h"
#include "itkStatismoIO.h"
#include "itkStandardMeshRepresenter.h"
#include "itkStatisticalModel.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "vnl/algo/vnl_svd.h"


// foreward declarations for
namespace ui {
    class Group;

    class ShapeModelTransformationView;

    class TriangleMesh;
}

namespace StatismoUI {
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
	//		m_eulerTransform->Register();
//		m_eulerTransform->SetIdentity();
        m_eulerTransform->SetMatrix(rigidTransform->GetMatrix());
        m_eulerTransform->SetTranslation(rigidTransform->GetTranslation());
        m_eulerTransform->SetCenter(rigidTransform->GetCenter());
    }

	PoseTransformation& operator=(const PoseTransformation& rhs) {
		if (this != &rhs) {
			m_eulerTransform = rhs.m_eulerTransform;
		}
		return *this;
	}

	PoseTransformation(const PoseTransformation& orig) {
		m_eulerTransform = orig.m_eulerTransform;
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

struct Color {
    Color(int r, int g, int b) : red(r), green(g), blue(b) {}

    int red;
    int green;
    int blue;

};

class TriangleMeshView {
public:
    TriangleMeshView(int id, const Color color, double opacity) : m_id(id), m_color(color), m_opacity(opacity)
    {   }


    int GetId() const { return m_id; }
    TriangleMeshView SetColor(const Color& c) const {
        return TriangleMeshView(m_id, c, m_opacity);
    }

    Color GetColor() const { return m_color; }
    double GetOpacity() const { return m_opacity; }

private:
    int m_id;
    Color m_color;
    double m_opacity;
};

class StatismoUI {

//    using namespace apache::thrift;
//    using namespace apache::thrift::protocol;
//    using namespace apache::thrift::transport;

//    using namespace boost;

    typedef itk::Mesh<float, 3> MeshType;
    typedef MeshType::PointType PointType;
    typedef itk::Image<short, 3> ImageType;
    typedef itk::StatisticalModel<MeshType> StatisticalModelType;

public:

    StatismoUI();

    ~StatismoUI();

    Group createGroup(const std::string& name);

    TriangleMeshView showTriangleMesh(const Group& group, const MeshType* mesh, const std::string& name);

    void showPointCloud(const Group& group, const std::list<PointType> points, const std::string& name);

    ShapeModelTransformationView showStatisticalShapeModel(const Group& group, const StatisticalModelType* ssm, const std::string& name);


	void showLandmark(const Group& group, const PointType& point, const vnl_matrix<double> cov, const std::string& name);

    void showImage(const Group& group, const ImageType* image, const std::string& name);

    void updateShapeModelTransformationView(const ShapeModelTransformationView& smv);
    void updateTriangleMeshView(const TriangleMeshView& tmv);

private:

    ui::Group groupToThriftGroup(const Group& group);
    ShapeModelTransformationView shapeModelTransformationViewFromThrift(const ui::ShapeModelTransformationView& tvthrift);
    ui::ShapeModelTransformationView shapeModelTransformationViewToThrift(const ShapeModelTransformationView& tv);
    ui::TriangleMesh meshToThriftMesh(const MeshType* mesh);


};

}

#endif //UI_STATISMOUI_H
