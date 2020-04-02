cmake_minimum_required(VERSION 3.0)
project(R1)
find_package(VTK REQUIRED)
vtk_module_config(VTK
vtkCommonCore
vtkIOImage
vtkIOXML
vtkInteractionWidgets
vtkRenderingVolumeOpenGL2
)

