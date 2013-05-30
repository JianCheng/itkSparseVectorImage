/*=========================================================================
 
 Program:   Sparse Vector Image Pixel Accessor
 
 Copyright (c) Pew-Thian Yap. All rights reserved.
 See http://www.unc.edu/~ptyap/ for details.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/

#ifndef __itkSparseVectorImagePixelAccessor_h
#define __itkSparseVectorImagePixelAccessor_h

#include "itkMacro.h"
#include "itkVariableLengthVector.h"


namespace itk
{

/** \class SparseVectorImagePixelAccessor
 * \brief Give access to partial aspects of a type
 *
 * SparseVectorImagePixelAccessor is specifically meant to provide SparseVectorImage
 * with the same \c DefaultPixelAccessor interface that
 * DefaultPixelAccessor provides to Image.
 *
 * SparseVectorImagePixelAccessor is templated over an internal type and an
 * external type representation. This class encapsulates a
 * customized convertion between the internal and external
 * type representations.
 *
 * \ingroup ITKSparseVectorImage 
 *
 */
template <class TType, class TPixelMapType>
class ITK_EXPORT SparseVectorImagePixelAccessor
{
public:

 /** External typedef. It defines the external aspect
   * that this class will exhibit. */
  typedef VariableLengthVector<TType> ExternalType;

  /** Internal typedef. It defines the internal real
   * representation of data. */
//  typedef VariableLengthVector<TType> InternalType;
  typedef TType InternalType;

  /** Typedef for pixel map. */
  typedef TPixelMapType PixelMapType;

  typedef unsigned int VectorLengthType;

  /** Set output using the value in input */
  inline void Set(InternalType & output, const ExternalType & input, const unsigned long offset ) const
    {
    unsigned long trueOffset = m_VectorLength * offset;
    
    for ( VectorLengthType i = 0; i < m_VectorLength; i++ )
      {
      if ( input[i] != 0 )
        {
        m_PixelMap->operator[](trueOffset + i) = input[i];
        }
      }
    }

  /** Get the value from input */
  inline ExternalType Get( const InternalType & begin, const unsigned long offset ) const
    {
    unsigned long trueOffset = m_VectorLength * offset;

    ExternalType pixel;
    pixel.SetSize(m_VectorLength);
    
    for ( VectorLengthType i = 0; i < m_VectorLength; i++ )
      {
      typename PixelMapType::const_iterator it = m_PixelMap->find( trueOffset + i );
      
      if ( it == m_PixelMap->end() )
        {
        pixel[i] = m_FillBufferValue[i];
        }
      else
        {
        pixel[i] = m_PixelMap->operator[](trueOffset + i);
        }
      }
    
    return pixel;
    }

  /** Set the length of each vector in the VectorImage */
  void SetVectorLength(VectorLengthType length)
    {
    m_VectorLength = length;
    }
  
  /** Get Vector lengths */
  VectorLengthType GetVectorLength() const { return m_VectorLength; }
  
  SparseVectorImagePixelAccessor() : m_VectorLength(0) {}

   /** Constructor to initialize slices and image size at construction time */
   SparseVectorImagePixelAccessor( PixelMapType* pixelMap,
     ExternalType fillBufferValue, VectorLengthType length)
     {
     m_PixelMap = pixelMap;
     m_FillBufferValue = fillBufferValue;
     m_VectorLength = length;
     }

  virtual ~SparseVectorImagePixelAccessor() {};

private:
  PixelMapType* m_PixelMap;
  ExternalType m_FillBufferValue;
  VectorLengthType m_VectorLength;
};

} // end namespace itk


#endif
