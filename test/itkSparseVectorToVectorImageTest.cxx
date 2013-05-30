#include "itkImageFileWriter.h"
#include "itkImageIOBase.h"
#include "itkVectorImage.h"
#include "itkSparseVectorImage.h"
#include "itkSparseVectorImageFileReader.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"


inline void 
PrintHelpInfo ( char* str )
{
  std::cout << str << ": convert a SparseVectorImage to VectorImage" << std::endl << std::flush;
  std::cout << str << " inputImage outputImage" << std::endl << std::flush;
}

int
itkSparseVectorToVectorImageTest(int argc, char *argv[])
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
  typedef itk::SparseVectorImageFileReader<SparseVectorImageType> ReaderType;

  SparseVectorImageType::Pointer inputImage;
  VectorImageType::Pointer vectorImage = VectorImageType::New();
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
  vectorImage->CopyInformation(inputImage);
  vectorImage->SetRegions(inputImage->GetLargestPossibleRegion());
  vectorImage->Allocate();

  // Iterator for the input image
  itk::ImageRegionConstIterator<SparseVectorImageType> inputIt(inputImage, inputImage->GetLargestPossibleRegion() );
  
  // Iterator for the output image
  itk::ImageRegionIterator<VectorImageType> outputIt(vectorImage, vectorImage->GetLargestPossibleRegion() );

  inputIt.GoToBegin();
  outputIt.GoToBegin();
  
  // Transfer data
  while( !inputIt.IsAtEnd() )
    {
    outputIt.Set(inputIt.Get());
    
    ++inputIt;
    ++outputIt;
    }


  
  // Write Output
  try
    {
    typedef itk::ImageFileWriter<VectorImageType> OutputImageWriterType;
    OutputImageWriterType::Pointer writer = OutputImageWriterType::New(); 
    std::cout << "Writing file: " << _OutputFile << std::endl;
    writer->SetFileName( _OutputFile );
    writer->SetInput( vectorImage );
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
