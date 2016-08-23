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
#include "gen-cpp/ui_types.h"


// foreward declarations for
namespace ui {
    class Group;

    class ShapeModelView;

    class ShapeModelTransformationView;

    class TriangleMesh;

    class TriangleMeshView;
}

namespace StatismoUI {
    class Group {
    public:
        Group(const std::string &name, int id) : m_name(name), m_id(id) {}

        int GetId() const { return m_id; }

        const std::string &GetName() const { return m_name; }


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

        ShapeTransformation &SetShapeTransformation(const vnl_vector<float> v) {
            m_coefficients = v;
            return *this;
        }

    private:
        vnl_vector<float> m_coefficients;
    };

    class PoseTransformation {
        typedef itk::Rigid3DTransform<float> Rigid3DTransformType;
        typedef Rigid3DTransformType::InputPointType PointType;
        typedef itk::Euler3DTransform<float> Euler3DTransformType;

    public:
        PoseTransformation(itk::Rigid3DTransform<float> *rigidTransform) {
            m_eulerTransform = Euler3DTransformType::New();
            //		m_eulerTransform->Register();
//		m_eulerTransform->SetIdentity();
            m_eulerTransform->SetMatrix(rigidTransform->GetMatrix());
            m_eulerTransform->SetTranslation(rigidTransform->GetTranslation());
            m_eulerTransform->SetCenter(rigidTransform->GetCenter());
        }

        PoseTransformation &operator=(const PoseTransformation &rhs) {
            if (this != &rhs) {
                m_eulerTransform = rhs.m_eulerTransform;
            }
            return *this;
        }

        PoseTransformation(const PoseTransformation &orig) {
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
        ShapeModelTransformationView(int id, const PoseTransformation &p, const ShapeTransformation &s)
                : m_shapeTransformation(s), m_poseTransformation(p), m_id(id) {}

        int GetId() const { return m_id; }

        const ShapeTransformation &GetShapeTransformation() const {
            return m_shapeTransformation;
        }

        ShapeModelTransformationView &SetShapeTransformation(const ShapeTransformation &st) {
            m_shapeTransformation = st;
            return *this;
        }

        const PoseTransformation &GetPoseTransformation() const {
            return m_poseTransformation;
        }

        ShapeModelTransformationView &SetPoseTransformation(const PoseTransformation &pt) {
            m_poseTransformation = pt;
            return *this;
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
        TriangleMeshView(int id, const Color color, double opacity, int lineWidth) : m_id(id), m_color(color),
                                                                                     m_opacity(opacity),
                                                                                     m_lineWidth(lineWidth) {}


        int GetId() const { return m_id; }

        Color GetColor() const { return m_color; }

        TriangleMeshView &SetColor(const Color &c) {
            m_color = c;
            return *this;
        }


        double GetOpacity() const { return m_opacity; }


        TriangleMeshView &SetOpacity(double opacity) {
            m_opacity = opacity;
            return *this;
        }

        int GetLineWidth() const {
            return m_lineWidth;
        };

        TriangleMeshView &SetLineWidth(int lineWidth) {
            m_lineWidth = lineWidth;
            return *this;
        }

    private:
        int m_id;
        Color m_color;
        double m_opacity;
        int m_lineWidth;
    };


    class ShapeModelView {
    public:
        ShapeModelView(const TriangleMeshView &triangleMeshView,
                       const ShapeModelTransformationView &shapeModelTransformationView)
                : m_triangleMeshView(triangleMeshView), m_shapeModelTransformationView(shapeModelTransformationView) {}

        TriangleMeshView GetTriangleMeshView() const { return m_triangleMeshView; }

        ShapeModelTransformationView GetShapeModelTransformationView() const { return m_shapeModelTransformationView; }

    private:
        TriangleMeshView m_triangleMeshView;
        ShapeModelTransformationView m_shapeModelTransformationView;
    };


    class ImageView {
    public:
        ImageView(int id, double window, double level, double opacity) : m_id(id), m_window(window), m_level(level),
                                                                         m_opacity(opacity) {}


        int GetId() const { return m_id; }

        double GetWindow() const { return m_window; }

        ImageView &SetWindow(double window) {
            m_window = window;
            return *this;
        }

        double GetLevel() const { return m_level; }

        ImageView &SetLevel(double level) {
            m_level = level;
            return *this;
        }

        double GetOpacity() const { return m_opacity; }


        ImageView &SetOpacity(double opacity) {
            m_opacity = opacity;
            return *this;
        }


    private:
        int m_id;
        double m_window;
        double m_level;
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

        Group createGroup(const std::string &name);

        TriangleMeshView showTriangleMesh(const Group &group, const MeshType *mesh, const std::string &name);

        void showPointCloud(const Group &group, const std::list<PointType> points, const std::string &name);

        ShapeModelView
        showStatisticalShapeModel(const Group &group, const StatisticalModelType *ssm, const std::string &name);


        void
        showLandmark(const Group &group, const PointType &point, const vnl_matrix<double> cov, const std::string &name);

        ImageView showImage(const Group &group, const ImageType *image, const std::string &name);

        void updateShapeModelTransformationView(const ShapeModelTransformationView &smv);

        void updateTriangleMeshView(const TriangleMeshView &tmv);

        void updateImageView(const ImageView &imv);


        void removeGroup(const Group& group);
        void removeTriangleMesh(const TriangleMeshView& tmv);
        void removeImage(const ImageView& imv);
        void removeShapeModelTransformation(const ShapeModelTransformationView& ssmtview);
        void removeShapeModel(const ShapeModelView & ssmview);

    private:

        ui::Group groupToThriftGroup(const Group &group);

        ShapeModelView
        shapeModelViewFromThrift(const ui::ShapeModelView &smvThrift);

        ui::ShapeModelView
        shapeModelViewToThriftShapeModelView(const ShapeModelView &tv);


        ShapeModelTransformationView
        shapeModelTransformationViewFromThrift(const ui::ShapeModelTransformationView &tvthrift);

        ui::ShapeModelTransformationView
        shapeModelTransformationViewToThrift(const ShapeModelTransformationView &tv);


        ui::ImageView
        imageViewToThriftImageView(const ImageView& iv);

        TriangleMeshView triangleMeshViewFromThriftMeshView(ui::TriangleMeshView tmvThrift);
        ui::TriangleMeshView thriftMeshViewFromTriangleMeshView(const TriangleMeshView& tmv);
        ui::TriangleMesh meshToThriftMesh(const MeshType *mesh);


    };

}

#endif //UI_STATISMOUI_H
