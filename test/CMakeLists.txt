itk_module_test()
set(ITKSparseVectorImageTests
  itkVectorToSparseVectorImageTest.cxx
  itkSparseVectorToVectorImageTest.cxx
  itkVectorAndSparseVectorImageConvertorTest.cxx
)

CreateTestDriver(ITKSparseVectorImage  "${ITKSparseVectorImage-Test_LIBRARIES}" "${ITKSparseVectorImageTests}")

# Test I/O convertor
itk_add_test( NAME ITKVectorToSparseVectorImageTest
  COMMAND ITKSparseVectorImageTestDriver
  itkVectorToSparseVectorImageTest DATA{Input/testSparseVectorImage_Vector.nii.gz} ${ITK_TEST_OUTPUT_DIR}/testSparseVectorImage_VectorToSparseOutput.spr
  )

itk_add_test( NAME ITKSparseVectorToVectorImageTest
  COMMAND ITKSparseVectorImageTestDriver
  itkSparseVectorToVectorImageTest ${ITK_TEST_OUTPUT_DIR}/testSparseVectorImage_VectorToSparseOutput.spr ${ITK_TEST_OUTPUT_DIR}/testSparseVectorImage_SparseToVectorOutput.nii.gz 
  )

itk_add_test( NAME ITKVectorAndSparseVectorImageConvertorTest
  COMMAND ITKSparseVectorImageTestDriver
  --compare DATA{Input/testSparseVectorImage_Vector.nii.gz} ${ITK_TEST_OUTPUT_DIR}/testSparseVectorImage_VectorOutput.nii.gz
  itkVectorAndSparseVectorImageConvertorTest DATA{Input/testSparseVectorImage_Vector.nii.gz} ${ITK_TEST_OUTPUT_DIR}/testSparseVectorImage_VectorOutput.nii.gz
  )

