/*=========================================================================
 
 Program:   Sparse Vector Image File Writer
 
 Copyright (c) Pew-Thian Yap. All rights reserved.
 See http://www.unc.edu/~ptyap/ for details.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/

#ifndef __itkSparseVectorImageFileWriter_hxx
#define __itkSparseVectorImageFileWriter_hxx

#include "itkSparseVectorImageFileWriter.h"
#include <fstream>

namespace itk
{

//---------------------------------------------------------
template <class TInputImage>
SparseVectorImageFileWriter<TInputImage>
::SparseVectorImageFileWriter()
{
  m_FileName = "";
  m_UseCompression = false;
//  m_UseInputMetaDataDictionary = true;
}

//---------------------------------------------------------
template <class TInputImage>
SparseVectorImageFileWriter<TInputImage>
::~SparseVectorImageFileWriter()
{
}

//---------------------------------------------------------
template <class TInputImage>
void 
SparseVectorImageFileWriter<TInputImage>
::SetInput(const InputImageType *input)
{
  // ProcessObject is not const_correct so this cast is required here.
  this->ProcessObject::SetNthInput(0, 
                                   const_cast<TInputImage *>(input ) );
}


//---------------------------------------------------------
template <class TInputImage>
const typename SparseVectorImageFileWriter<TInputImage>::InputImageType *
SparseVectorImageFileWriter<TInputImage>
::GetInput(void)
{
  if (this->GetNumberOfInputs() < 1)
    {
    return 0;
    }
  
  return static_cast<TInputImage*>
    (this->ProcessObject::GetInput(0));
}
  
//---------------------------------------------------------
template <class TInputImage>
const typename SparseVectorImageFileWriter<TInputImage>::InputImageType *
SparseVectorImageFileWriter<TInputImage>
::GetInput(unsigned int idx)
{
  return static_cast<TInputImage*> (this->ProcessObject::GetInput(idx));
}

//---------------------------------------------------------
template <class TInputImage>
void 
SparseVectorImageFileWriter<TInputImage>
::Write()
{
  const InputImageType * input = this->GetInput();

  itkDebugMacro( <<"Writing an image file" );

  // Make sure input is available
  if ( input == 0 )
    {
    itkExceptionMacro(<< "No input to writer!");
    }

  if ( m_FileName == "" )
    {
    itkExceptionMacro(<<"No key filename specified");
    }

  // write the data
  this->GenerateData();
}


//---------------------------------------------------------
template <class TInputImage>
void 
SparseVectorImageFileWriter<TInputImage>
::GenerateData(void)
{
  itkDebugMacro ( << "SparseVectorImageFileWriter::GenerateData() \n" );

  // Setup - Input Image
  InputImageType * input = const_cast<InputImageType*>(this->GetInput());
  InputImagePixelContainerType * container = input->GetPixelContainer();
  InputImagePixelMapType * pixelMap = container->GetPixelMap();
  InputImagePixelMapIteratorType iterator = pixelMap->begin();
//  InputImageSpacingType::SpacingType inputSpacing = input->GetSpacing();
  
  // Setup - Output Image
  KeyImage = KeyImageType::New();
  ValueImage = ValueImageType::New();
    
  KeyImageType::IndexType startIndex;
  KeyImageType::RegionType region;
  KeyImageType::SizeType size;
  KeyImageType::SpacingType spacing;
  
  startIndex.Fill(0);
  
  if ( pixelMap->size() > 0 )
    {
    size[0] = pixelMap->size();
    }
  else
    {
    size[0] = 1; // Output at least one voxel.
    }
    
  spacing.Fill(1);
  
  region.SetIndex(startIndex);
  region.SetSize(size);

  KeyImage->SetSpacing(spacing);
  KeyImage->SetRegions(region);
  ValueImage->SetSpacing(spacing);
  ValueImage->SetRegions(region);

  KeyImage->Allocate();
  ValueImage->Allocate();
  KeyImage->FillBuffer(static_cast<KeyType>(0));
  ValueImage->FillBuffer(static_cast<InputImagePixelType>(0));  

  typedef itk::ImageRegionIterator< KeyImageType > KeyImageIteratorType;
  typedef itk::ImageRegionIterator< ValueImageType > ValueImageIteratorType;
  
  KeyImageIteratorType KeyImageIterator( KeyImage, KeyImage->GetRequestedRegion() );
  ValueImageIteratorType ValueImageIterator( ValueImage, ValueImage->GetRequestedRegion() );
  
  KeyImageIterator.GoToBegin();
  ValueImageIterator.GoToBegin();

  if ( pixelMap->size() > 0 )
    {
    // Populate Data
    while ( iterator != pixelMap->end() )
      {
  //    std::cout << "first = " << iterator->first << std::endl;
      KeyImageIterator.Set(static_cast<KeyType>(iterator->first));
  //    std::cout << "second = " << iterator->second << std::endl;
      ValueImageIterator.Set(iterator->second);
      ++iterator;
      ++KeyImageIterator;
      ++ValueImageIterator;
      }
    }
  else
    {
    KeyImageIterator.Set(static_cast<KeyType>(0));
    ValueImageIterator.Set(0);
    }
  
  // Process File Names
  std::string BaseFileName = "";
  std::string FileNameExtension;
  std::string DataFileNameExtension = "nrrd";

  std::string::size_type idx;
  idx = std::string(GetFileName()).find('.');

  if (idx != std::string::npos)
    {
    FileNameExtension = std::string(GetFileName()).substr(idx + 1);
    if (FileNameExtension != "spr")
      {
      std::cout << "Renaming extension to .spr" << std::endl;
      FileNameExtension = "spr";
      }
    BaseFileName = std::string(GetFileName()).substr(0, idx);
    }
  std::string KeyFileName = BaseFileName + "_key." + DataFileNameExtension;
  std::string ValueFileName = BaseFileName + "_value." + DataFileNameExtension;
  std::string HeaderFileName = BaseFileName + "." + FileNameExtension;
  
  // Write Files
  KeyImageFileWriter = KeyImageFileWriterType::New();
  ValueImageFileWriter = ValueImageFileWriterType::New();
  
  KeyImageFileWriter->SetFileName(KeyFileName);
  KeyImageFileWriter->SetInput(KeyImage);
  ValueImageFileWriter->SetFileName(ValueFileName);
  ValueImageFileWriter->SetInput(ValueImage);

  KeyImageFileWriter->SetUseCompression(this->m_UseCompression);
  ValueImageFileWriter->SetUseCompression(this->m_UseCompression);
//  KeyImageFileWriter->SetUseInputMetaDataDictionary(this->m_UseInputMetaDataDictionary);
//  ValueImageFileWriter->SetUseInputMetaDataDictionary(this->m_UseInputMetaDataDictionary);  
  
  KeyImageFileWriter->Update();
  ValueImageFileWriter->Update();

  // Write Header
  std::ofstream outfile;
//  std::string HeaderFileName = GetHeaderFileName();
//  std::cout << HeaderFileName << std::endl;
  outfile.open(HeaderFileName.c_str(), std::fstream::out);

  InputImageRegionType outputRegion = input->GetLargestPossibleRegion();
  InputImageSizeType outputSize = outputRegion.GetSize();
  InputImageSpacingType outputSpacing = input->GetSpacing();

  outfile << "NDims = " << input->GetImageDimension() << std::endl;
  outfile << "DimSize = ";
  for (unsigned int d=0; d<input->GetImageDimension(); d++)
    {
    outfile << outputSize[d] << " ";
    }
  outfile << std::endl;
  
  outfile << "ElementSpacing = ";
  for (unsigned int d=0; d<input->GetImageDimension(); d++)
    {
    outfile << outputSpacing[d] << " ";
    }
  outfile << std::endl;

  outfile << "KeyElementDataFile = " << KeyFileName << std::endl;
  outfile << "ValueElementDataFile = " << ValueFileName << std::endl;
  
  outfile.close();
}


//---------------------------------------------------------
template <class TInputImage>
void 
SparseVectorImageFileWriter<TInputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);

  os << indent << "File Name: " 
     << (m_FileName.data() ? m_FileName.data() : "(none)") << std::endl;

  if (m_UseCompression)
    {
    os << indent << "Compression: On\n";
    }
  else
    {
    os << indent << "Compression: Off\n";
    }

//  if (m_UseInputMetaDataDictionary)
//    {
//    os << indent << "UseInputMetaDataDictionary: On\n";
//    }
//  else
//    {
//    os << indent << "UseInputMetaDataDictionary: Off\n";
//    }

}

} // end namespace itk

#endif
