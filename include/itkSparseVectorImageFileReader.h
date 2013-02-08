/*=========================================================================
 
 Program:   Sparse Vector Image File Reader
 
 Copyright (c) Pew-Thian Yap. All rights reserved.
 See http://www.unc.edu/~ptyap/ for details.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/

#ifndef __itkSparseVectorImageFileReader_h
#define __itkSparseVectorImageFileReader_h

//#include "itkProcessObject.h"
#include "itkExceptionObject.h"
#include "itkImage.h"
#include "itkImageSource.h"
#include "itkImageIOBase.h"
#include "itkImageFileReader.h"

namespace itk
{

/** \class ImageFileReaderException
 *
 * \brief Base exception class for IO conflicts.
 */
class SparseVectorImageFileReaderException : public ExceptionObject 
{
public:
  /** Run-time information. */
  itkTypeMacro( SparseVectorImageFileReaderException, ExceptionObject );

  /** Constructor. */
  SparseVectorImageFileReaderException(const char *file, unsigned int line, 
                           const char* message = "Error in IO",
                           const char* loc = "Unknown") :
    ExceptionObject(file, line, message, loc)
  {
  }

  /** Constructor. */
  SparseVectorImageFileReaderException(const std::string &file, unsigned int line, 
                           const char* message = "Error in IO",
                           const char* loc = "Unknown") : 
    ExceptionObject(file, line, message, loc)
  {
  }
};


/** \class SparseVectorImageFileReader
 * \brief Reads sparse image data from key and value files.
 *
 */
/** \class SparseVectorImageFileReader
 */
template <class TOutputImage>
class ITK_EXPORT SparseVectorImageFileReader : public ImageSource<TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef SparseVectorImageFileReader   Self;
  typedef ImageSource<TOutputImage>     Superclass;
  typedef SmartPointer<Self>            Pointer;
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(SparseVectorImageFileReader, ImageSource);

  /** Some convenient typedefs. */
  typedef TOutputImage  OutputImageType;
  typedef typename OutputImageType::Pointer                       OutputImagePointer;
  typedef typename OutputImageType::SizeType                      OutputImageSizeType;
  typedef typename OutputImageType::SpacingType                   OutputImageSpacingType;
  typedef typename OutputImageType::IndexType                     OutputImageIndexType;
  typedef typename OutputImageType::PointType                     OutputImagePointType;
  typedef typename OutputImageType::DirectionType                 OutputImageDirectionType;
  typedef typename OutputImageType::RegionType                    OutputImageRegionType;
  typedef typename OutputImageType::PixelType                     OutputImagePixelType;
  typedef typename OutputImageType::InternalPixelType             OutputImageInternalPixelType;
  typedef typename OutputImageType::PixelContainer                OutputImagePixelContainerType;
  typedef typename OutputImageType::PixelContainer::Pointer       OutputImagePixelContainerPointerType;  
  typedef typename OutputImageType::PixelContainer::PixelMapType  OutputImagePixelMapType;
  typedef typename OutputImagePixelMapType::iterator              OutputImagePixelMapIteratorType;
  
  /** Specify the files to read. This is forwarded to the IO instance. */
  itkSetStringMacro(FileName);
  itkGetStringMacro(FileName);
  
  /** Set the stream On or Off */
  itkSetMacro(UseStreaming,bool);
  itkGetConstReferenceMacro(UseStreaming,bool);
  itkBooleanMacro(UseStreaming);
  
  /** Set/Get the ImageIO helper class */
  itkGetObjectMacro(ImageIO,ImageIOBase);

protected:
  SparseVectorImageFileReader();
  ~SparseVectorImageFileReader();
  void PrintSelf(std::ostream& os, Indent indent) const;
  
  /** Convert a block of pixels from one type to another. */
//  void DoConvertBuffer(void* buffer, size_t numberOfPixels);

  /** Test whether the given filename exist and it is readable, this
    * is intended to be called before attempting to use  ImageIO
    * classes for actually reading the file. If the file doesn't exist
    * or it is not readable, and exception with an approriate message
    * will be thrown. */
//  void TestFileExistanceAndReadability();

  typedef Image<unsigned long, 1> KeyImageType;
  typedef Image<OutputImageInternalPixelType, 1> ValueImageType;
      
  typename KeyImageType::Pointer m_KeyImage;
  typename ValueImageType::Pointer m_ValueImage;

  typedef itk::ImageFileReader<KeyImageType> KeyImageFileReaderType;
  typedef itk::ImageFileReader<ValueImageType> ValueImageFileReaderType;
  
  typename KeyImageFileReaderType::Pointer m_KeyImageFileReader;
  typename ValueImageFileReaderType::Pointer m_ValueImageFileReader;

  /** Does the real work. */
  virtual void GenerateData();

//  ImageIOBase::Pointer m_ImageIO;
//  bool                 m_UserSpecifiedImageIO; // keep track whether the
                                               // ImageIO is user specified 

//  std::string m_KeyFileName; // The key file to be read
//  std::string m_ValueFileName; // The value file to be read
  std::string m_FileName;
  
  bool m_UseStreaming;
  
  ImageIOBase::Pointer m_ImageIO;

  bool GetFilePath(const char *_fName, char *_fPath);
  
private:
  SparseVectorImageFileReader(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  
  std::string m_ExceptionMessage;  

  // The region that the ImageIO class will return when we ask to
  // produce the requested region.
//  ImageIORegion m_ActualIORegion; 
};


} //namespace ITK

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkSparseVectorImageFileReader.hxx"
#endif

#endif // __itkSparseSparseVectorImageFileReader_h
