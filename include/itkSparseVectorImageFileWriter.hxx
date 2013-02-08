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
  m_UseCompression = true;
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
  m_KeyImage = KeyImageType::New();
  m_ValueImage = ValueImageType::New();
    
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

  m_KeyImage->SetSpacing(spacing);
  m_KeyImage->SetRegions(region);
  m_ValueImage->SetSpacing(spacing);
  m_ValueImage->SetRegions(region);

  m_KeyImage->Allocate();
  m_ValueImage->Allocate();
  m_KeyImage->FillBuffer(static_cast<KeyType>(0));
  m_ValueImage->FillBuffer(static_cast<InputImagePixelType>(0));

  typedef itk::ImageRegionIterator< KeyImageType > KeyImageIteratorType;
  typedef itk::ImageRegionIterator< ValueImageType > ValueImageIteratorType;
  
  KeyImageIteratorType keyImageIterator( m_KeyImage, m_KeyImage->GetRequestedRegion() );
  ValueImageIteratorType valueImageIterator( m_ValueImage, m_ValueImage->GetRequestedRegion() );
  
  keyImageIterator.GoToBegin();
  valueImageIterator.GoToBegin();

  if ( pixelMap->size() > 0 )
    {
    // Populate Data
    while ( iterator != pixelMap->end() )
      {
  //    std::cout << "first = " << iterator->first << std::endl;
      keyImageIterator.Set(static_cast<KeyType>(iterator->first));
  //    std::cout << "second = " << iterator->second << std::endl;
      valueImageIterator.Set(iterator->second);
      ++iterator;
      ++keyImageIterator;
      ++valueImageIterator;
      }
    }
  else
    {
    keyImageIterator.Set(static_cast<KeyType>(0));
    valueImageIterator.Set(0);
    }
  
  // Process File Names
  std::string baseFileName = "";
  std::string fileNameExtension;
  std::string dataFileNameExtension = "nrrd";

  std::string::size_type idx;
  idx = std::string(GetFileName()).find('.');

  if (idx != std::string::npos)
    {
    fileNameExtension = std::string(GetFileName()).substr(idx + 1);
    if (fileNameExtension != "spr")
      {
      std::cout << "Renaming extension to .spr" << std::endl;
      fileNameExtension = "spr";
      }
    baseFileName = std::string(GetFileName()).substr(0, idx);
    }
  std::string keyFileName = baseFileName + "_key." + dataFileNameExtension;
  std::string valueFileName = baseFileName + "_value." + dataFileNameExtension;
  std::string headerFileName = baseFileName + "." + fileNameExtension;
  
  // Write Files
  m_KeyImageFileWriter = KeyImageFileWriterType::New();
  m_ValueImageFileWriter = ValueImageFileWriterType::New();
  
  m_KeyImageFileWriter->SetFileName(keyFileName);
  m_KeyImageFileWriter->SetInput(m_KeyImage);
  m_ValueImageFileWriter->SetFileName(valueFileName);
  m_ValueImageFileWriter->SetInput(m_ValueImage);

  m_KeyImageFileWriter->SetUseCompression(this->m_UseCompression);
  m_ValueImageFileWriter->SetUseCompression(this->m_UseCompression);
//  m_KeyImageFileWriter->SetUseInputMetaDataDictionary(this->m_UseInputMetaDataDictionary);
//  m_ValueImageFileWriter->SetUseInputMetaDataDictionary(this->m_UseInputMetaDataDictionary);  
  
  m_KeyImageFileWriter->Update();
  m_ValueImageFileWriter->Update();

  // Write Header
  std::ofstream outfile;
//  std::string HeaderFileName = GetHeaderFileName();
//  std::cout << HeaderFileName << std::endl;
  outfile.open(headerFileName.c_str(), std::fstream::out);

  InputImageRegionType outputRegion = input->GetLargestPossibleRegion();
  InputImageSizeType outputSize = outputRegion.GetSize();
  InputImageSpacingType outputSpacing = input->GetSpacing();

  outfile << "NDims = " << input->GetImageDimension() + 1 << std::endl;
  outfile << "DimSize = ";
  outfile << input->GetNumberOfComponentsPerPixel() << " ";
  for (unsigned int d=0; d<input->GetImageDimension(); d++)
    {
    outfile << outputSize[d] << " ";
    }
  outfile << std::endl;
  
  outfile << "ElementSpacing = ";
  outfile << "1" << " ";
  for (unsigned int d=0; d<input->GetImageDimension(); d++)
    {
    outfile << outputSpacing[d] << " ";
    }
  outfile << std::endl;

  outfile << "KeyElementDataFile = " << keyFileName << std::endl;
  outfile << "ValueElementDataFile = " << valueFileName << std::endl;
  
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
