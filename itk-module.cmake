set(DOCUMENTATION "TODO")

itk_module(ITKSparseVectorImage
  DEPENDS
    ITKIOImageBase
    ITKImageFunction
  TEST_DEPENDS
    ITKTestKernel
  DESCRIPTION
    "${DOCUMENTATION}"
)
