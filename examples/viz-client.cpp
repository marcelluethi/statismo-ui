// This autogenerated skeleton file illustrates how to build a server.
// You should copy it to another filename to avoid overwriting it.

#include "StatismoUI.h"

#include "statismo/ITK/itkIO.h"
#include "statismo/ITK/itkStandardMeshRepresenter.h"
#include "statismo/ITK/itkStatisticalModel.h"

#include <itkMesh.h>
#include <itkImage.h>
#include <itkImageFileReader.h>

#include <iostream>


int
main(int argc, char ** argv)
{
  StatismoUI::StatismoUI ui;
  StatismoUI::Group      g = ui.createGroup("viz-client");

  using MeshType = itk::Mesh<float, 3>;
  using ImageType = itk::Image<short, 3>;

  using RepresenterType = itk::StandardMeshRepresenter<float, 3>;
  using StatisticalModelType = itk::StatisticalModel<MeshType>;

  auto representer = RepresenterType::New();
  auto model = itk::StatismoIO<MeshType>::LoadStatisticalModel(representer, "../data/knee_gp_model.h5");

  auto meanMesh = model->DrawSample();

  auto tv = ui.showTriangleMesh(g, meanMesh, "example mesh");
  tv.SetColor(StatismoUI::Color(1.0, 1.0, 255.0)).SetOpacity(0.8).SetLineWidth(5);
  ui.updateTriangleMeshView(tv);

  /*
  // Image example
  auto reader = itk::ImageFileReader<ImageType>::New();
  reader->SetFileName("/tmp/test.nii");
  reader->Update();
  ImageType::Pointer    img = reader->GetOutput();
  StatismoUI::ImageView imageView = ui.showImage(g, img, "example image");
  imageView.SetWindow(1115).SetLevel(1200).SetOpacity(0.5);
  ui.updateImageView(imageView);
  */
  //

  auto ssmView = ui.showStatisticalShapeModel(g, model, "example model");

  vnl_vector<float> newCoeffs(ssmView.GetShapeModelTransformationView().GetShapeTransformation().GetCoefficients());
  for (unsigned i = 0; i < newCoeffs.size(); ++i)
  {
    newCoeffs[i] += 1;
  }
  auto euler = itk::Euler3DTransform<float>::New();
  euler->SetIdentity();

  itk::Point<float> p;
  p.SetElement(0, 10);
  p.SetElement(1, 5);
  p.SetElement(2, 55);
  euler->SetCenter(p);
  // euler->SetRotation(0.1, 0.1, 0.3);
  itk::Vector<float> t(3);
  t[0] = 0;
  t[1] = 0;
  t[2] = 0;
  euler->SetRotation(0, 0, 1.5);
  euler->SetTranslation(t);
  auto nv = ssmView.GetShapeModelTransformationView();

  nv.SetPoseTransformation(StatismoUI::PoseTransformation(*euler))
    .SetShapeTransformation(StatismoUI::ShapeTransformation(std::move(newCoeffs)));
  ui.updateShapeModelTransformationView(nv);

  auto tmv = ssmView.GetTriangleMeshView();
  tmv.SetColor(StatismoUI::Color(255, 0, 0));
  ui.updateTriangleMeshView(tmv);

  ui.removeTriangleMesh(tmv);
  return 0;
}
