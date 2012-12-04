/*=========================================================================
 
 Program:   Sparse Vector Image Linear Interpolate Image Function
 
 Copyright (c) Pew-Thian Yap. All rights reserved.
 See http://www.unc.edu/~ptyap/ for details.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/

#ifndef __itkSparseVectorImageLinearInterpolateImageFunction_hxx
#define __itkSparseVectorImageLinearInterpolateImageFunction_hxx

#include "itkSparseVectorImageLinearInterpolateImageFunction.h"

#include "vnl/vnl_math.h"

namespace itk
{

/**
 * Define the number of neighbors
 */
template<class TInputImage, class TCoordRep>
  const unsigned long SparseVectorImageLinearInterpolateImageFunction<
      TInputImage, TCoordRep>::m_Neighbors = 1 << TInputImage::ImageDimension;

/**
 * Constructor
 */
template<class TInputImage, class TCoordRep>
SparseVectorImageLinearInterpolateImageFunction<TInputImage, TCoordRep>::SparseVectorImageLinearInterpolateImageFunction()
{

}

/**
 * PrintSelf
 */
template<class TInputImage, class TCoordRep>
void
SparseVectorImageLinearInterpolateImageFunction<TInputImage, TCoordRep>::PrintSelf(
    std::ostream& os, Indent indent ) const
{
  this->Superclass::PrintSelf( os, indent );
}

/**
 * Evaluate at image index position - vector version
 */
template<class TInputImage, class TCoordRep>
typename SparseVectorImageLinearInterpolateImageFunction<TInputImage,
    TCoordRep>::VectorOutputType
SparseVectorImageLinearInterpolateImageFunction<TInputImage, TCoordRep>::VectorEvaluateAtContinuousIndex(
    const VectorOutputContinuousIndexType& index ) const
{

  unsigned int dim; // index over dimension

  /**
   * Compute base index = closet index below point
   * Compute distance from point to base index
   */
  signed long baseIndex[ImageDimension];
  double distance[ImageDimension];

  for ( dim = 0; dim < ImageDimension-1; dim++ )
    {
    baseIndex[dim+1] = (long) vcl_floor( index[dim] );
    distance[dim+1] = index[dim] - double( baseIndex[dim+1] );
    }
  baseIndex[0] = 0;
  distance[0] = 0;

  RegionType region = this->GetInputImage()->GetLargestPossibleRegion();
  SizeType size = region.GetSize();
  unsigned int vectorDimension = size[0];

  /**
   * Interpolated value is the weight some of each of the surrounding
   * neighbors. The weight for each neighbour is the fraction overlap
   * of the neighbor pixel with respect to a pixel centered on point.
   */
  //OutputType output = static_cast<OutputType>( 0 );
  VectorOutputType output;
  output.SetSize( vectorDimension );
  output.Fill( 0.0 );

  PixelType input;

  RealType totalOverlap = 0.0;

  for ( unsigned int counter = 0; counter < m_Neighbors/2; counter++ )
    {

    double overlap = 1.0; // fraction overlap
    unsigned int upper = counter; // each bit indicates upper/lower neighbour
    IndexType neighIndex;

    // get neighbor index and overlap fraction
    for ( dim = 0; dim < ImageDimension-1; dim++ )
      {

      if ( upper & 1 )
        {
        neighIndex[dim+1] = baseIndex[dim+1] + 1;
        overlap *= distance[dim+1];
        }
      else
        {
        neighIndex[dim+1] = baseIndex[dim+1];
        overlap *= 1.0 - distance[dim+1];
        }

      upper >>= 1;

      }
    neighIndex[0] = 0;

    // get neighbor value only if overlap is not zero
    if ( overlap )
      {
      if ( this->IsInsideBuffer( neighIndex ) )
        {
        for ( unsigned int k = 0; k < vectorDimension; k++ )
          {
          neighIndex[0] = k;
          input = this->GetInputImage()->GetPixel( neighIndex );
          output[k] += overlap * static_cast<RealType>( input );
          }
        }
      totalOverlap += overlap;
      }

    if ( totalOverlap == 1.0 )
      {
      // finished
      break;
      }

    }

  return ( output );
}

/**
 * Evaluate at image index position
 */
template<class TInputImage, class TCoordRep>
typename SparseVectorImageLinearInterpolateImageFunction<TInputImage,
    TCoordRep>::OutputType
SparseVectorImageLinearInterpolateImageFunction<TInputImage, TCoordRep>::EvaluateAtContinuousIndex(
    const ContinuousIndexType& index ) const
{

  unsigned int dim; // index over dimension

  /**
   * Compute base index = closet index below point
   * Compute distance from point to base index
   */
  signed long baseIndex[ImageDimension];
  double distance[ImageDimension];

  for ( dim = 0; dim < ImageDimension; dim++ )
    {
    baseIndex[dim] = (long) vcl_floor( index[dim] );
    distance[dim] = index[dim] - double( baseIndex[dim] );
    }

  /**
   * Interpolated value is the weight some of each of the surrounding
   * neighbors. The weight for each neighbour is the fraction overlap
   * of the neighbor pixel with respect to a pixel centered on point.
   */
  OutputType output = static_cast<OutputType>( 0 );
//	output.SetSize(this->GetInputImage()->GetNumberOfComponentsPerPixel());
//	output.Fill( 0.0 );

  PixelType input;

  RealType totalOverlap = 0.0;

  for ( unsigned int counter = 0; counter < m_Neighbors; counter++ )
    {

    double overlap = 1.0; // fraction overlap
    unsigned int upper = counter; // each bit indicates upper/lower neighbour
    IndexType neighIndex;

    // get neighbor index and overlap fraction
    for ( dim = 0; dim < ImageDimension; dim++ )
      {

      if ( upper & 1 )
        {
        neighIndex[dim] = baseIndex[dim] + 1;
        overlap *= distance[dim];
        }
      else
        {
        neighIndex[dim] = baseIndex[dim];
        overlap *= 1.0 - distance[dim];
        }

      upper >>= 1;

      }

    // get neighbor value only if overlap is not zero
    if ( overlap )
      {
      if ( this->IsInsideBuffer( neighIndex ) )
        {
        input = this->GetInputImage()->GetPixel( neighIndex );

//      for(unsigned int k = 0; k < Dimension; k++ )
        //for(unsigned int k = 0; k < input.GetSize(); k++ )
        //{
        output += overlap * static_cast<RealType>( input );
        //}
        }
      totalOverlap += overlap;
      }

    if ( totalOverlap == 1.0 )
      {
      // finished
      break;
      }

    }

  return ( output );
}

} // end namespace itk

#endif
