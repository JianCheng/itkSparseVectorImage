#include "itkImageFileWriter.h"
#include "itkVectorImage.h"
#include "itkSparseVectorImage.h"
#include "itkSparseVectorImageFileReader.h"
#include "itkSparseVectorImageFileWriter.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageFileWriter.h"


inline void 
PrintHelpInfo ( char* str )
{
  std::cout << str << ": convert a VectorImage to SparseVectorImage, then convert it back" << std::endl << std::flush;
  std::cout << str << " inputImage outputImage" << std::endl << std::flush;
}

int
itkVectorAndSparseVectorImageConvertorTest(int argc, char *argv[])
{
  if (argc!=3)
    {
    std::cerr << "No input or No output!" << std::endl;
    PrintHelpInfo(argv[0]);
    return EXIT_FAILURE;
    }

  std::string _InputFile(argv[1]);
  std::string _OutputFile(argv[2]);

  // Define Variables
  typedef float PixelType;
  typedef itk::VectorImage<PixelType, 3> VectorImageType;
  typedef itk::SparseVectorImage<PixelType, 3> SparseVectorImageType;
  typedef itk::ImageFileReader<VectorImageType> ReaderType;

  VectorImageType::Pointer inputImage, outputImage;
  SparseVectorImageType::Pointer sparseImage = SparseVectorImageType::New();
  ReaderType::Pointer reader = ReaderType::New();
  
  // Read input image
  reader->SetFileName(_InputFile);
  try
    {
    std::cout << "Reading file: " << _InputFile << std::endl;
    reader->Update();
    }
  catch (itk::ExceptionObject & err)
    {
    std::cerr << "ExceptionObject caught!" << std::endl;
    std::cerr << err << std::endl;
    return EXIT_FAILURE;
    }

  inputImage = reader->GetOutput();
  
  // Allocate output image
  sparseImage->CopyInformation(inputImage);
  sparseImage->SetRegions(inputImage->GetLargestPossibleRegion());
  sparseImage->Allocate();

  // Iterator for the input image
  itk::ImageRegionConstIterator<VectorImageType> inputIt(inputImage, inputImage->GetLargestPossibleRegion() );
  
  // Iterator for the output image
  itk::ImageRegionIterator<SparseVectorImageType> sparseIt(sparseImage, sparseImage->GetLargestPossibleRegion() );

  inputIt.GoToBegin();
  sparseIt.GoToBegin();
  
  // Transfer data
  while( !inputIt.IsAtEnd() )
    {
    sparseIt.Set(inputIt.Get());
    
    ++inputIt;
    ++sparseIt;
    }


  // transfer SparseVectorImage to VectorImage
  outputImage = VectorImageType::New();
  outputImage->CopyInformation(sparseImage);
  outputImage->SetRegions(sparseImage->GetLargestPossibleRegion());
  outputImage->Allocate();

  // Iterator for the output image
  itk::ImageRegionIterator<VectorImageType> outputIt(outputImage, outputImage->GetLargestPossibleRegion() );

  outputIt.GoToBegin();
  sparseIt.GoToBegin();
  
  // Transfer data
  while( !outputIt.IsAtEnd() )
    {
    outputIt.Set(sparseIt.Get());
    
    ++sparseIt;
    ++outputIt;
    }

  
  // Write Output
  try
    {
    typedef itk::ImageFileWriter<VectorImageType> OutputImageWriterType;
    OutputImageWriterType::Pointer writer = OutputImageWriterType::New(); 
    std::cout << "Writing file: " << _OutputFile << std::endl;
    writer->SetFileName( _OutputFile );
    writer->SetInput( outputImage );
    writer->Update();
    }
  catch ( itk::ExceptionObject & err )
    {
    std::cerr << "ExceptionObject caught!" << std::endl;
    std::cerr << err << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;  
}



