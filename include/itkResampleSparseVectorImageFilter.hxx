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
#ifndef __itkResampleSparseVectorImageFilter_hxx
#define __itkResampleSparseVectorImageFilter_hxx

#include "itkResampleSparseVectorImageFilter.h"
#include "itkObjectFactory.h"
#include "itkIdentityTransform.h"
#include "itkProgressReporter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkImageLinearIteratorWithIndex.h"
#include "itkSpecialCoordinatesImage.h"
#include "itkDefaultConvertPixelTraits.h"

namespace itk
{
/**
 * Initialize new instance
 */
template< class TInputImage,
          class TOutputImage,
          class TInterpolatorPrecisionType >
ResampleSparseVectorImageFilter< TInputImage, TOutputImage, TInterpolatorPrecisionType >
::ResampleSparseVectorImageFilter()
{
  m_OutputOrigin.Fill(0.0);
  m_OutputSpacing.Fill(1.0);
  m_OutputDirection.SetIdentity();

  m_Size.Fill(0);
  m_OutputStartIndex.Fill(0);

  m_Transform =
    IdentityTransform< TInterpolatorPrecisionType, ImageDimension >::New();

  m_Interpolator = dynamic_cast< InterpolatorType * >
                   ( LinearInterpolatorType::New().GetPointer() );

  m_DefaultPixelValue
    = NumericTraits<PixelType>::ZeroValue( m_DefaultPixelValue );
}

/**
 * Print out a description of self
 *
 * \todo Add details about this class
 */
template< class TInputImage,
          class TOutputImage,
          class TInterpolatorPrecisionType >
void
ResampleSparseVectorImageFilter< TInputImage, TOutputImage, TInterpolatorPrecisionType >
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "DefaultPixelValue: "
     << static_cast< typename NumericTraits< PixelType >::PrintType >
  ( m_DefaultPixelValue )
     << std::endl;
  os << indent << "Size: " << m_Size << std::endl;
  os << indent << "OutputStartIndex: " << m_OutputStartIndex << std::endl;
  os << indent << "OutputSpacing: " << m_OutputSpacing << std::endl;
  os << indent << "OutputOrigin: " << m_OutputOrigin << std::endl;
  os << indent << "OutputDirection: " << m_OutputDirection << std::endl;
  os << indent << "Transform: " << m_Transform.GetPointer() << std::endl;
  os << indent << "Interpolator: " << m_Interpolator.GetPointer() << std::endl;
  return;
}

/**
 * Set the output image spacing.
 */
template< class TInputImage,
          class TOutputImage,
          class TInterpolatorPrecisionType >
void
ResampleSparseVectorImageFilter< TInputImage, TOutputImage, TInterpolatorPrecisionType >
::SetOutputSpacing(const double *spacing)
{
  SpacingType s(spacing);

  this->SetOutputSpacing(s);
}

/**
 * Set the output image origin.
 */
template< class TInputImage,
          class TOutputImage,
          class TInterpolatorPrecisionType >
void
ResampleSparseVectorImageFilter< TInputImage, TOutputImage, TInterpolatorPrecisionType >
::SetOutputOrigin(const double *origin)
{
  OriginPointType p(origin);

  this->SetOutputOrigin(p);
}

/**
 * Set up state of filter before multi-threading.
 * InterpolatorType::SetInputImage is not thread-safe and hence
 * has to be set up before ThreadedGenerateData
 */
template< class TInputImage,
          class TOutputImage,
          class TInterpolatorPrecisionType >
void
ResampleSparseVectorImageFilter< TInputImage, TOutputImage, TInterpolatorPrecisionType >
::BeforeThreadedGenerateData()
{
  if ( !m_Transform )
    {
    itkExceptionMacro(<< "Transform not set");
    }

  if ( !m_Interpolator )
    {
    itkExceptionMacro(<< "Interpolator not set");
    }

  // Connect input image to interpolator
  m_Interpolator->SetInputImage( this->GetInput() );

  unsigned int nComponents
    = DefaultConvertPixelTraits<PixelType>::GetNumberOfComponents(
      m_DefaultPixelValue );

  if (nComponents == 0)
    {
    PixelComponentType zeroComponent
      = NumericTraits<PixelComponentType>::ZeroValue( zeroComponent );
    nComponents = this->GetInput()->GetNumberOfComponentsPerPixel();
    NumericTraits<PixelType>::SetLength(m_DefaultPixelValue, nComponents );
    for (unsigned int n=0; n<nComponents; n++)
      {
      PixelConvertType::SetNthComponent( n, m_DefaultPixelValue,
                                         zeroComponent );
      }
    }

  unsigned int nbOfThreads = this->GetNumberOfThreads();
  if ( !m_IntermediateImages.empty() )
    m_IntermediateImages.clear();
  m_IntermediateImages.resize( nbOfThreads );
  for ( unsigned int i = 0; i < nbOfThreads; ++i )
    m_IntermediateImages[i] = NULL;

  if ( !m_SplitRegions.empty() )
    m_SplitRegions.clear();
  m_SplitRegions.resize( nbOfThreads );
}

/**
 * Set up state of filter after multi-threading.
 */
template< class TInputImage,
          class TOutputImage,
          class TInterpolatorPrecisionType >
void
ResampleSparseVectorImageFilter< TInputImage, TOutputImage, TInterpolatorPrecisionType >
::AfterThreadedGenerateData()
{
  // Disconnect input image from the interpolator
  m_Interpolator->SetInputImage(NULL);

  // Get output pointer
  OutputImagePointer outputPtr = this->GetOutput();

  // Stack the intermediate images to form the output image
  for ( unsigned int threadId = 0; threadId < m_IntermediateImages.size(); ++threadId )
    {
    if ( m_IntermediateImages[threadId] )
      {
      typedef ImageRegionIteratorWithIndex< TOutputImage > OutputIterator;

      OutputIterator tmpIt(m_IntermediateImages[threadId], m_SplitRegions[threadId]);
      OutputIterator outIt(outputPtr, m_SplitRegions[threadId]);

      while( !outIt.IsAtEnd() )
        {
        outIt.Set( tmpIt.Get() );
        ++tmpIt;
        ++outIt;
        }
      }
    }
}

/**
 * ThreadedGenerateData
 */
template< class TInputImage,
          class TOutputImage,
          class TInterpolatorPrecisionType >
void
ResampleSparseVectorImageFilter< TInputImage, TOutputImage, TInterpolatorPrecisionType >
::ThreadedGenerateData(const OutputImageRegionType & outputRegionForThread,
                       ThreadIdType threadId)
{
  m_SplitRegions[threadId] = outputRegionForThread;

  // Clone the output image
  OutputImagePointer outputPtr = this->GetOutput();

  m_IntermediateImages[threadId] = OutputImageType::New();
  m_IntermediateImages[threadId]->CopyInformation( outputPtr );
  m_IntermediateImages[threadId]->SetRequestedRegion( outputPtr->GetRequestedRegion() );
  m_IntermediateImages[threadId]->SetBufferedRegion( outputPtr->GetBufferedRegion() );
  m_IntermediateImages[threadId]->Allocate();

  this->NonlinearThreadedGenerateData(outputRegionForThread, threadId);
}

/**
 * Cast from interpolotor output to pixel type
 */
template< class TInputImage,
          class TOutputImage,
          class TInterpolatorPrecisionType >
typename ResampleSparseVectorImageFilter< TInputImage, TOutputImage, TInterpolatorPrecisionType >
::PixelType
ResampleSparseVectorImageFilter< TInputImage, TOutputImage, TInterpolatorPrecisionType >
::CastPixelWithBoundsChecking(const InterpolatorOutputType value,
                              const ComponentType minComponent,
                              const ComponentType maxComponent ) const
{
  const unsigned int nComponents = InterpolatorConvertType::GetNumberOfComponents(value);
  PixelType outputValue;
  NumericTraits<PixelType>::SetLength( outputValue, nComponents );

  for (unsigned int n=0; n<nComponents; n++)
    {
    ComponentType component = InterpolatorConvertType::GetNthComponent( n, value );

    if ( component < minComponent )
      {
      PixelConvertType::SetNthComponent( n, outputValue, static_cast<PixelComponentType>( minComponent ) );
      }
    else if ( component > maxComponent )
      {
      PixelConvertType::SetNthComponent( n, outputValue, static_cast<PixelComponentType>( maxComponent ) );
      }
    else
      {
      PixelConvertType::SetNthComponent(n, outputValue,
                                        static_cast<PixelComponentType>( component ) );
      }
    }

  return outputValue;
}

/**
 * NonlinearThreadedGenerateData
 */
template< class TInputImage,
          class TOutputImage,
          class TInterpolatorPrecisionType >
void
ResampleSparseVectorImageFilter< TInputImage, TOutputImage, TInterpolatorPrecisionType >
::NonlinearThreadedGenerateData(const OutputImageRegionType &
                                outputRegionForThread,
                                ThreadIdType threadId)
{
  // Get the intermediate image pointers
  OutputImagePointer outputPtr = m_IntermediateImages[threadId];

  // Get this input pointers
  InputImageConstPointer inputPtr = this->GetInput();

  // Create an iterator that will walk the output region for this thread.
  typedef ImageRegionIteratorWithIndex< TOutputImage > OutputIterator;
  OutputIterator outIt(outputPtr, outputRegionForThread);

  // Define a few indices that will be used to translate from an input pixel
  // to an output pixel
  PointType outputPoint;         // Coordinates of current output pixel
  PointType inputPoint;          // Coordinates of current input pixel

  ContinuousInputIndexType inputIndex;

  // Support for progress methods/callbacks
  ProgressReporter progress( this,
                             threadId,
                             outputRegionForThread.GetNumberOfPixels() );

  // Min/max values of the output pixel type AND these values
  // represented as the output type of the interpolator
  const PixelComponentType minValue =  NumericTraits< PixelComponentType >::NonpositiveMin();
  const PixelComponentType maxValue =  NumericTraits< PixelComponentType >::max();

  typedef typename InterpolatorType::OutputType OutputType;
  const ComponentType minOutputValue = static_cast< ComponentType >( minValue );
  const ComponentType maxOutputValue = static_cast< ComponentType >( maxValue );

  // Walk the output region
  outIt.GoToBegin();

  while ( !outIt.IsAtEnd() )
    {
    // Determine the index of the current output pixel
    outputPtr->TransformIndexToPhysicalPoint(outIt.GetIndex(), outputPoint);

    // Compute corresponding input pixel position
    inputPoint = this->m_Transform->TransformPoint(outputPoint);
    inputPtr->TransformPhysicalPointToContinuousIndex(inputPoint, inputIndex);

    PixelType        pixval;
    OutputType       value;
    // Evaluate input at right position and copy to the output
    if ( m_Interpolator->IsInsideBuffer(inputIndex) )
      {
      value = m_Interpolator ->EvaluateAtContinuousIndex(inputIndex);
      pixval = this->CastPixelWithBoundsChecking( value, minOutputValue, maxOutputValue );
      outIt.Set(pixval);
      }
    else
      {
      outIt.Set( m_DefaultPixelValue ); // default background value
      }

    progress.CompletedPixel();
    ++outIt;
    }

  return;
}

/**
 * Inform pipeline of necessary input image region
 *
 * Determining the actual input region is non-trivial, especially
 * when we cannot assume anything about the transform being used.
 * So we do the easy thing and request the entire input image.
 */
template< class TInputImage,
          class TOutputImage,
          class TInterpolatorPrecisionType >
void
ResampleSparseVectorImageFilter< TInputImage, TOutputImage, TInterpolatorPrecisionType >
::GenerateInputRequestedRegion()
{
  // call the superclass's implementation of this method
  Superclass::GenerateInputRequestedRegion();

  if ( !this->GetInput() )
    {
    return;
    }

  // get pointers to the input and output
  InputImagePointer inputPtr  =
    const_cast< TInputImage * >( this->GetInput() );

  // Request the entire input image
  inputPtr->SetRequestedRegionToLargestPossibleRegion();

  return;
}

/**
 * Inform pipeline of required output region
 */
template< class TInputImage,
          class TOutputImage,
          class TInterpolatorPrecisionType >
void
ResampleSparseVectorImageFilter< TInputImage, TOutputImage, TInterpolatorPrecisionType >
::GenerateOutputInformation()
{
  // call the superclass' implementation of this method
  Superclass::GenerateOutputInformation();

  // get pointers to the input and output
  OutputImagePointer outputPtr = this->GetOutput();
  if ( !outputPtr )
    {
    return;
    }

  typename TOutputImage::RegionType outputLargestPossibleRegion;
  outputLargestPossibleRegion.SetSize(m_Size);
  outputLargestPossibleRegion.SetIndex(m_OutputStartIndex);
  outputPtr->SetLargestPossibleRegion(outputLargestPossibleRegion);

  outputPtr->SetSpacing(m_OutputSpacing);
  outputPtr->SetOrigin(m_OutputOrigin);
  outputPtr->SetDirection(m_OutputDirection);

  return;
}

/**
 * Verify if any of the components has been modified.
 */
template< class TInputImage,
          class TOutputImage,
          class TInterpolatorPrecisionType >
ModifiedTimeType
ResampleSparseVectorImageFilter< TInputImage, TOutputImage, TInterpolatorPrecisionType >
::GetMTime(void) const
{
  ModifiedTimeType latestTime = Object::GetMTime();

  if ( m_Transform )
    {
    if ( latestTime < m_Transform->GetMTime() )
      {
      latestTime = m_Transform->GetMTime();
      }
    }

  if ( m_Interpolator )
    {
    if ( latestTime < m_Interpolator->GetMTime() )
      {
      latestTime = m_Interpolator->GetMTime();
      }
    }

  return latestTime;
}
} // end namespace itk

#endif
