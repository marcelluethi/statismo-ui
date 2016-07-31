// This autogenerated skeleton file illustrates how to build a server.
// You should copy it to another filename to avoid overwriting it.

#include <iostream>
#include "itkMesh.h"
#include "itkStatismoIO.h"
#include "itkStandardMeshRepresenter.h"
#include "itkStatisticalModel.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "ScalismoUI.h"

int main(int argc, char **argv) {



  typedef itk::Mesh<float, 3> MeshType;
  typedef itk::Image<short, 3> ImageType;

  typedef itk::StandardMeshRepresenter<float, 3> RepresenterType;

  RepresenterType::Pointer representer = RepresenterType::New();

  itk::StatisticalModel<MeshType>::Pointer model = itk::StatismoIO<MeshType>::LoadStatisticalModel(representer, "/tmp/facemodel.h5");

  MeshType::Pointer meanMesh = model->DrawSample();

//  typedef itk::ImageFileReader<ImageType> ReaderType;
//  typename ReaderType::Pointer reader = ReaderType::New();
//  reader->SetFileName("/tmp/vsd-0003.nii");
//  reader->Update();
//  ImageType::Pointer image = reader->GetOutput();

  ScalismoUI ui;
  Group g = ui.createGroup("a newly created group");
//  ui.showImage(g, image, "abc");
//
//  ui.showTriangleMesh(g, meanMesh, "a mesh");

  const ShapeModelTransformationView& v = ui.showStatisticalShapeModel(g, model, "aModel");

  vnl_vector<float> newCoeffs(v.GetShapeTransformation().GetCoefficients());
  for (unsigned i = 0; i < newCoeffs.size(); ++i) {
    newCoeffs[i] += 1;
  }
  itk::Euler3DTransform<float>::Pointer euler = itk::Euler3DTransform<float>::New();
  euler->SetIdentity();
  euler->SetRotation(0.01, 1.0, 2.0);
  itk::Vector<float> t(3); t[0] = 10; t[1] = 20; t[2] = 30;
  euler->SetTranslation(t);
  ShapeModelTransformationView nv = v.SetShapeTransformation(v.GetShapeTransformation().SetShapeTransformation(newCoeffs)).SetPoseTransformation(PoseTransformation(euler));
  ui.updateShapeModelTransformationView(nv);
  return 0;
}
