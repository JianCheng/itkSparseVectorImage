/*=========================================================================
 
 Program:   Sparse Vector Image File Reader
 
 Copyright (c) Pew-Thian Yap. All rights reserved.
 See http://www.unc.edu/~ptyap/ for details.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/

#ifndef __itkSparseVectorImageFileReader_hxx
#define __itkSparseVectorImageFileReader_hxx

#include "itkSparseVectorImageFileReader.h"
#include "itkImageRegionIterator.h"

namespace itk
{

template <class TOutputImage>
SparseVectorImageFileReader<TOutputImage>
::SparseVectorImageFileReader()
{
  m_FileName = "";
  m_UseStreaming = true;
  m_ImageIO = 0;
}

template <class TOutputImage>
SparseVectorImageFileReader<TOutputImage>
::~SparseVectorImageFileReader()
{
}

template <class TOutputImage>
void SparseVectorImageFileReader<TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  if (m_ImageIO)
    {
    os << indent << "ImageIO: \n";
    m_ImageIO->Print(os, indent.GetNextIndent());
    }
  else
    {
    os << indent << "ImageIO: (null)" << "\n";
    }
  
  os << indent << "m_FileName: " << m_FileName << "\n";
  os << indent << "m_UseStreaming: " << m_UseStreaming << "\n";
}

template <class TOutputImage>
bool SparseVectorImageFileReader<TOutputImage>
::GetFilePath(const char *_fName, char *_fPath)
{
  long i;

  size_t l = strlen(_fName);

  for(i=(long)l-1; i>=0; i--)
    if(_fName[i] == '\\' || _fName[i] == '/')
      break;

    if(i >= 0 && (_fName[i] == '/' || _fName[i] == '\\'))
      {
      strcpy(_fPath, _fName);
      _fPath[i+1] = '\0';
      return true;
      }
    else
      {
      _fPath[0] = '\0';
      return false;
      }
}

template <class TOutputImage>
void SparseVectorImageFileReader<TOutputImage>
::GenerateData()
{
  itkDebugMacro ( << "SparseVectorImageFileReader::GenerateData() \n" );

  std::string KeyFileName;
  std::string ValueFileName;
  char tempLine[256];
  
  // Read Header
  std::ifstream infile;
  infile.open(m_FileName.c_str(), std::fstream::in);

  if ( !infile.is_open() )
    {
    itkExceptionMacro( << "Cannot open file: " << m_FileName );
    }
      
  bool usePath = false;
  char pathName[255];
  usePath = this->GetFilePath( m_FileName.c_str(), pathName );


  std::string line, extractedLine;

  OutputImageRegionType outputRegion;
  OutputImageSizeType outputSize;
  OutputImageSpacingType outputSpacing;
  OutputImageIndexType outputStartIndex;
  OutputImagePointType outputOrigin;
  OutputImageDirectionType outputDirection;

  unsigned int ndims = 0;
  unsigned int numberOfComponentsPerPixel = 0;
  
  // This section should be rewritten to support generic image dimension
  char * pch;
  unsigned int idx;
  
  while (!infile.eof())
    {
    if (infile.getline(tempLine, 256))
      {
      line = std::string(tempLine);

      if (line.find("NDims") != std::string::npos)
        {
          extractedLine = line.substr(line.find("=") + 1);
          pch = strtok(const_cast<char*>(extractedLine.c_str())," ");
          if (pch != NULL)
          {
            ndims = atoi(pch);
//            pch = strtok (NULL, " ");
          }      
//          sscanf(extractedLine.c_str(), "%d", &ndims); 
        }

      if (line.find("DimSize") != std::string::npos)
        {
          extractedLine = line.substr(line.find("=") + 1);
//          sscanf(extractedLine.c_str(), "%ld %ld %ld %ld", &outputSize[0], &outputSize[1],
//              &outputSize[2], &outputSize[3]); 
          idx = 0;
          pch = strtok (const_cast<char*>(extractedLine.c_str())," ");
          while (pch != NULL)
          {
            if ( idx == 0 )
              {
              numberOfComponentsPerPixel = atoi(pch);
              }
            else
              {
              outputSize[idx-1] = atoi(pch);
              }
            idx++;
            pch = strtok (NULL, " ");
          }
        }

      if (line.find("ElementSpacing") != std::string::npos)
        {
          extractedLine = line.substr(line.find("=") + 1);
//          sscanf(extractedLine.c_str(), "%lf %lf %lf %lf", &outputSpacing[0], &outputSpacing[1],
//              &outputSpacing[2], &outputSpacing[3]);
          idx = 0;
          pch = strtok (const_cast<char*>(extractedLine.c_str())," ");
          while (pch != NULL)
          {
            if ( idx > 0 )
              {
              outputSpacing[idx-1] = atof(pch);
              }
            idx++;
            pch = strtok (NULL, " ");
          }
        }

      if (line.find("KeyElementDataFile") != std::string::npos)
        {
          extractedLine = line.substr(line.find("=") + 1);

          const size_t beginStr = extractedLine.find_first_not_of(" \t");
          if (beginStr == std::string::npos)
          {
              // no content
              std::cerr << "No key file specified!";
          }
          const size_t endStr = extractedLine.find_last_not_of(" \t");
          const size_t range = endStr - beginStr + 1;
          KeyFileName = extractedLine.substr(beginStr, range);

          if ( usePath )
            {
            KeyFileName = std::string(pathName) + KeyFileName;
            }

//          sscanf(extractedLine.c_str(), "%s", tempLine);
//          KeyFileName = extractedLine;
        }

      if (line.find("ValueElementDataFile") != std::string::npos)
         {
           extractedLine = line.substr(line.find("=") + 1);

           const size_t beginStr = extractedLine.find_first_not_of(" \t");
           if (beginStr == std::string::npos)
           {
               // no content
               std::cerr << "No value file specified!";
           }
           const size_t endStr = extractedLine.find_last_not_of(" \t");
           const size_t range = endStr - beginStr + 1;
           ValueFileName = extractedLine.substr(beginStr, range);
           
          if ( usePath )
            {
            ValueFileName = std::string(pathName) + ValueFileName;
            }

//           sscanf(extractedLine.c_str(), "%s", tempLine);
//           ValueFileName = extractedLine;
         }
      }
    else
      {
      break;
      }

    }
  infile.close();
   
  // Setup - Output Image
  OutputImageType * output = this->GetOutput();
  outputStartIndex.Fill(0);
  outputRegion.SetIndex(outputStartIndex);
  outputRegion.SetSize(outputSize);  
  output->SetRegions(outputRegion);
  output->SetSpacing(outputSpacing);
  outputOrigin.Fill(0);
  output->SetOrigin(outputOrigin);
  outputDirection.SetIdentity();
  output->SetDirection(outputDirection);
  output->SetNumberOfComponentsPerPixel(numberOfComponentsPerPixel);
  output->Allocate();

  OutputImagePixelType outputPixel;
  outputPixel.SetSize(numberOfComponentsPerPixel);
  outputPixel.Fill(0);
  
  output->FillBuffer(outputPixel);

  OutputImagePixelContainerType * container = output->GetPixelContainer();
  OutputImagePixelMapType * pixelMap = container->GetPixelMap();
//  OutputImagePixelMapIteratorType iterator = pixelMap->begin();
  
  KeyImageFileReader = KeyImageFileReaderType::New();
  ValueImageFileReader = ValueImageFileReaderType::New();
  
  KeyImageFileReader->SetFileName(KeyFileName);
  ValueImageFileReader->SetFileName(ValueFileName);
  
  KeyImageFileReader->Update();
  ValueImageFileReader->Update();
  
  KeyImage = KeyImageFileReader->GetOutput();
  ValueImage = ValueImageFileReader->GetOutput();
  
  m_ImageIO = ValueImageFileReader->GetImageIO();
  
  typedef ImageRegionIterator< KeyImageType > KeyImageIteratorType;
  typedef ImageRegionIterator< ValueImageType > ValueImageIteratorType;
  
  KeyImageIteratorType KeyImageIterator( KeyImage, KeyImage->GetRequestedRegion() );
  ValueImageIteratorType ValueImageIterator( ValueImage, ValueImage->GetRequestedRegion() );
  
  KeyImageIterator.GoToBegin();
  ValueImageIterator.GoToBegin();

  // Populate Data
  while ( !KeyImageIterator.IsAtEnd() )
    {
    pixelMap->operator[](KeyImageIterator.Get()) = ValueImageIterator.Get();
//    ++iterator;
    ++KeyImageIterator;
    ++ValueImageIterator;
    }
  
}


} //namespace ITK

#endif
