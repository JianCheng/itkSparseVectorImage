itk_module_test()
set(ITKSparseVectorImageTests
  itkVectorToSparseVectorImageTest.cxx
  itkSparseVectorToVectorImageTest.cxx
  itkVectorAndSparseVectorImageConvertorTest.cxx
)

CreateTestDriver(ITKSparseVectorImage  "${ITKSparseVectorImage-Test_LIBRARIES}" "${ITKSparseVectorImageTests}")

# Test I/O convertor
itk_add_test( NAME itkVectorToSparseVectorImageTest
  COMMAND ITKSparseVectorImageTestDriver
  --compare DATA{Input/testSparseVectorImage_Sparse_key.nrrd} ${ITK_TEST_OUTPUT_DIR}/testSparseVectorImage_VectorToSparseOutput_key.nrrd
  --compare DATA{Input/testSparseVectorImage_Sparse_value.nrrd} ${ITK_TEST_OUTPUT_DIR}/testSparseVectorImage_VectorToSparseOutput_value.nrrd
  itkVectorToSparseVectorImageTest DATA{Input/testSparseVectorImage_Vector.nii.gz} ${ITK_TEST_OUTPUT_DIR}/testSparseVectorImage_VectorToSparseOutput.spr
  )

itk_add_test( NAME itkSparseVectorToVectorImageTest
  COMMAND ITKSparseVectorImageTestDriver
  --compare DATA{Input/testSparseVectorImage_Vector.nii.gz} ${ITK_TEST_OUTPUT_DIR}/testSparseVectorImage_SparseToVectorOutput.nii.gz
  itkSparseVectorToVectorImageTest DATA{Input/testSparseVectorImage_Sparse.spr} ${ITK_TEST_OUTPUT_DIR}/testSparseVectorImage_SparseToVectorOutput.nii.gz 
  )

itk_add_test( NAME itkVectorAndSparseVectorImageConvertorTest
  COMMAND ITKSparseVectorImageTestDriver
  --compare DATA{Input/testSparseVectorImage_Vector.nii.gz} ${ITK_TEST_OUTPUT_DIR}/testSparseVectorImage_VectorOutput.nii.gz
  itkVectorAndSparseVectorImageConvertorTest DATA{Input/testSparseVectorImage_Vector.nii.gz} ${ITK_TEST_OUTPUT_DIR}/testSparseVectorImage_VectorOutput.nii.gz
  )

