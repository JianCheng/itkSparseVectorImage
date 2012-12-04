/*=========================================================================
 
 Program:   Sparse Vector Image
 
 Copyright (c) Pew-Thian Yap. All rights reserved.
 See http://www.unc.edu/~ptyap/ for details.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/

#ifndef _itkSparseVectorImage_hxx
#define _itkSparseVectorImage_hxx

#include "itkSparseVectorImage.h"
#include "itkProcessObject.h"

namespace itk
{

template<class TPixel, unsigned int VImageDimension>
SparseVectorImage<TPixel, VImageDimension>
::SparseVectorImage()
{
  m_Container = PixelContainer::New();
}


template<class TPixel, unsigned int VImageDimension>
void
SparseVectorImage<TPixel, VImageDimension>
::Allocate()
{
  this->ComputeOffsetTable();
  m_Container->Reserve( );
}


template<class TPixel, unsigned int VImageDimension>
void
SparseVectorImage<TPixel, VImageDimension>
::Initialize()
{
  //
  // We don't modify ourselves because the "ReleaseData" methods depend upon
  // no modification when initialized.
  //

  // Call the superclass which should initialize the BufferedRegion ivar.
  Superclass::Initialize();

  // Replace the handle to the container. This is the safest thing to do,
  // since the same container can be shared by multiple images (e.g.
  // Grafted outputs and in place filters).
  m_Container = PixelContainer::New();
}


template<class TPixel, unsigned int VImageDimension>
void
SparseVectorImage<TPixel, VImageDimension>
::FillBuffer(const PixelType& value)
{
  m_FillBufferValue = value;
  m_Container->GetPixelMap()->clear();
}


template<class TPixel, unsigned int VImageDimension>
void
SparseVectorImage<TPixel, VImageDimension>
::SetPixelContainer(PixelContainer *container)
{
   if (m_Container != container)
     {
     m_Container = container;
     this->Modified();
     }
}


template<class TPixel, unsigned int VImageDimension>
void
SparseVectorImage<TPixel, VImageDimension>
::Graft(const DataObject *data)
{
  // call the superclass' implementation
  Superclass::Graft( data );

  if ( data )
    {
    // Attempt to cast data to an Image
    const Self *imgData;

    try
      {
      imgData = dynamic_cast< const Self *>( data );
      }
    catch( ... )
      {
      return;
      }

    // Copy from SliceContiguousImage< TPixel, dim >
    if ( imgData )
      {
      // Now copy anything remaining that is needed
      this->SetPixelContainer( const_cast< PixelContainer *>
                                    (imgData->GetPixelContainer()) );
      }
    else 
      {
      // pointer could not be cast back down
      itkExceptionMacro( << "itk::SliceContiguousImage::Graft() cannot cast "
                         << typeid(data).name() << " to "
                         << typeid(const Self *).name() );
      }
    }
}

template< class TPixel, unsigned int VImageDimension >
unsigned int
SparseVectorImage< TPixel, VImageDimension >
::GetNumberOfComponentsPerPixel() const
{
  return this->m_VectorLength;
}

template< class TPixel, unsigned int VImageDimension >
void
SparseVectorImage< TPixel, VImageDimension >
::SetNumberOfComponentsPerPixel(unsigned int n)
{
  this->SetVectorLength( static_cast< VectorLengthType >( n ) );
}
  
template<class TPixel, unsigned int VImageDimension>
void
SparseVectorImage<TPixel, VImageDimension>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  os << indent << "PixelContainer: " << std::endl;
  m_Container->Print(os, indent.GetNextIndent());
// m_Origin and m_Spacing are printed in the Superclass
}

} // end namespace itk

#endif
