/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef __itkShrinkSparseVectorImageFilter_hxx
#define __itkShrinkSparseVectorImageFilter_hxx

#include "itkShrinkSparseVectorImageFilter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkContinuousIndex.h"
#include "itkObjectFactory.h"
#include "itkProgressReporter.h"

namespace itk
{
/**
 *
 */
template<class TInputImage, class TOutputImage>
ShrinkSparseVectorImageFilter< TInputImage, TOutputImage >
::ShrinkSparseVectorImageFilter()
{
  for ( unsigned int i = 0; i < InputImageDimension; ++i )
  {
    m_ShrinkFactors[i] = 1.0;
    m_MinSize[i] = 5;
  }
}

/**
 *
 */
template<class TInputImage, class TOutputImage>
void
ShrinkSparseVectorImageFilter<TInputImage, TOutputImage>
::PrintSelf( std::ostream & os, Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Shrink Factor: ";
  for ( unsigned int i = 0; i < InputImageDimension; ++i )
  {
    os << m_ShrinkFactors[i] << " ";
  }
  os << std::endl;

  os << indent << "Minimum Size: ";
  for ( unsigned int i = 0; i < InputImageDimension; ++i )
  {
    os << m_MinSize[i] << " ";
  }
  os << std::endl;
}

/**
 *
 */
template<class TInputImage, class TOutputImage>
void
ShrinkSparseVectorImageFilter<TInputImage, TOutputImage>
::SetShrinkFactors( RealType factor )
{
  if ( factor < 1.0 )  factor = 1.0;

  for ( unsigned int i = 0; i < InputImageDimension; ++i )
    m_ShrinkFactors[i] = factor;

  this->Modified();
}

/**
 *
 */
template<class TInputImage, class TOutputImage>
void
ShrinkSparseVectorImageFilter<TInputImage, TOutputImage>
::SetShrinkFactor( unsigned int i, RealType factor )
{
  if( i>=InputImageDimension )
  {
    itkExceptionMacro( "SetShrinkFactor(): invalid i." );
  }

  if ( factor < 1.0 )  factor = 1.0;

  m_ShrinkFactors[i] = factor;

  this->Modified();
}


/**
 *
 */
template<class TInputImage, class TOutputImage>
void
ShrinkSparseVectorImageFilter<TInputImage, TOutputImage>
::SetMinSize( unsigned int width )
{
  if ( width == 0 )  width = 5;

  for ( unsigned int i = 0; i < InputImageDimension; ++i )
    m_MinSize[i] = width;

  this->Modified();
}

/**
 *
 */
template<class TInputImage, class TOutputImage>
void
ShrinkSparseVectorImageFilter<TInputImage, TOutputImage>
::SetMinSize( unsigned int i, unsigned int width )
{
  if( i>=InputImageDimension )
  {
    itkExceptionMacro( "SetShrinkFactor(): invalid i." );
  }

  if ( width == 0 )  width = 5;

  m_MinSize[i] = width;

  this->Modified();
}

/**
 *
 */
template<class TInputImage, class TOutputImage>
void
ShrinkSparseVectorImageFilter<TInputImage, TOutputImage>
::GenerateOutputInformation()
{
  // Call the superclass' implementation of this method
  Superclass::GenerateOutputInformation();

  // Get pointers to the input and output
  InputImageConstPointer inputPtr  = this->GetInput();
  OutputImagePointer     outputPtr = this->GetOutput();

  if ( !inputPtr || !outputPtr )
  {
    return;
  }

  const typename InputImageType::SpacingType &inputSpacing = inputPtr->GetSpacing();
  const typename InputImageType::SizeType &inputSize = inputPtr->GetLargestPossibleRegion().GetSize();
  const typename InputImageType::IndexType &inputStartIndex = inputPtr->GetLargestPossibleRegion().GetIndex();

  typename OutputImageType::SpacingType outputSpacing;
  typename OutputImageType::SizeType outputSize;
  typename OutputImageType::IndexType outputStartIndex;

  for ( unsigned int i = 0; i < InputImageDimension; ++i )
  {
    // Check if the scaling factor is too large, so as to
    // guarantee the minimum size of the output image
    RealType maxFactor = (RealType)inputSize[i] / (RealType)m_MinSize[i];
    if ( m_ShrinkFactors[i] > maxFactor )
    {
      std::cerr << "Warning: the scaling factor " << m_ShrinkFactors[i]
                << " is too large! Use " << maxFactor << " instead."
                << std::endl;
      m_ShrinkFactors[i] = maxFactor;
      this->Modified();
    }

    // Compute the size of the output image
    outputSize[i] = static_cast<SizeValueType>(
      vcl_floor( (RealType)inputSize[i] / m_ShrinkFactors[i] ) );

    // Set the correct spacing
    outputSpacing[i] = inputSpacing[i] * inputSize[i] / outputSize[i];

    // Set the index of the output image to 0, this is not critical as
    // the origin will be shifted later. Other values may be used instead.
    outputStartIndex[i] = 0;

    //outputStartIndex[i] = static_cast<IndexValueType>(
    //  vcl_ceil( (RealType)inputStartIndex[i] / m_ShrinkFactors[i] ) );
  }

  outputPtr->SetSpacing( outputSpacing );

  // Compute origin offset
  // The physical center's of the input and output should be the same
  ContinuousIndex<RealType, InputImageDimension> inputCenterIndex;
  ContinuousIndex<RealType, InputImageDimension> outputCenterIndex;
  for ( unsigned int i = 0; i < InputImageDimension; ++i )
  {
    inputCenterIndex[i] = inputStartIndex[i] + ( inputSize[i] - 1 ) / 2.0;
    outputCenterIndex[i] = outputStartIndex[i] + ( outputSize[i] - 1 ) / 2.0;
  }

  typename OutputImageType::PointType inputCenterPoint;
  typename OutputImageType::PointType outputCenterPoint;
  inputPtr->TransformContinuousIndexToPhysicalPoint( inputCenterIndex, inputCenterPoint );
  outputPtr->TransformContinuousIndexToPhysicalPoint( outputCenterIndex, outputCenterPoint );

  typename OutputImageType::PointType outputOrigin = outputPtr->GetOrigin();
  outputOrigin = outputOrigin + ( inputCenterPoint - outputCenterPoint );
  outputPtr->SetOrigin( outputOrigin );

  // Set region
  typename OutputImageType::RegionType outputLargestPossibleRegion;
  outputLargestPossibleRegion.SetSize( outputSize );
  outputLargestPossibleRegion.SetIndex( outputStartIndex );

  outputPtr->SetLargestPossibleRegion( outputLargestPossibleRegion );
}

/**
 *
 */
template<class TInputImage, class TOutputImage>
void
ShrinkSparseVectorImageFilter<TInputImage, TOutputImage>
::GenerateData()
{
  // Get pointers to the input and output
  InputImageConstPointer inputPtr  = this->GetInput();
  OutputImagePointer     outputPtr = this->GetOutput();

  ResampleFilterPointer filterPtr = ResampleFilterType::New();
  filterPtr->SetInput( inputPtr );
  filterPtr->SetSize( outputPtr->GetLargestPossibleRegion().GetSize() );
  filterPtr->SetOutputStartIndex( outputPtr->GetLargestPossibleRegion().GetIndex() );
  filterPtr->SetOutputSpacing( outputPtr->GetSpacing() );
  filterPtr->SetOutputOrigin( outputPtr->GetOrigin() );
  filterPtr->Update();
  this->GraftOutput( filterPtr->GetOutput() );
}

} // end namespace itk

#endif
