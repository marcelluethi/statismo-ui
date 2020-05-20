set(_target_include_dir_suffix /statismo/core)
set(_target_include_dir include/${_target_include_dir_suffix})

file(GLOB _clients *.cpp)

# We need some components to enable ITK IO factory
set(ITK_COMPONENTS
  ITKIOImageBase
  ITKIOMesh
  ITKIOMeta
  ITKIONRRD
  ITKIOPNG
  ITKIOTIFF
  ITKIONIFTI
)
find_package(ITK COMPONENTS ${ITK_COMPONENTS} REQUIRED)
include(${ITK_USE_FILE})

foreach(_c ${_clients})
    get_filename_component(_exe ${_c} NAME_WE)
    add_executable(${_exe} ${_c})
    target_link_libraries(${_exe} statismo_ui)
endforeach()
