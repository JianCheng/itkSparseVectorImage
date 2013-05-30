itk_module_test()
set(ITKSparseVectorImageTests
  itkVectorToSparseVectorImageTest.cxx
)

CreateTestDriver(ITKSparseVectorImage  "${ITKSparseVectorImage-Test_LIBRARIES}" "${ITKSparseVectorImageTests}")

# Test I/O using itk::Image
itk_add_test( NAME ITKVectorToSparseVectorImageTest
  COMMAND ITKSparseVectorImageTestDriver
  --compare DATA{Input/testSparseVectorImage_Vector.nii.gz} ${ITK_TEST_OUTPUT_DIR}/testSparseVectorImage_VectorOutput.nii.gz
  itkVectorToSparseVectorImageTest DATA{Input/testSparseVectorImage_Vector.nii.gz} ${ITK_TEST_OUTPUT_DIR}/testSparseVectorImage_VectorOutput.nii.gz )

