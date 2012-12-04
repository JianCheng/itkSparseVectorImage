/*=========================================================================
 
 Program:   Sparse Vector Image Nearest Neighbor Interpolate Image Function
 
 Copyright (c) Pew-Thian Yap. All rights reserved.
 See http://www.unc.edu/~ptyap/ for details.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/

#ifndef __itkSparseVectorImageNearestNeighborInterpolateImageFunction_h
#define __itkSparseVectorImageNearestNeighborInterpolateImageFunction_h

#include "itkSparseVectorImageInterpolateImageFunction.h"

namespace itk
{

/** 
 * \class SparseVectorImageNearestNeighborInterpolateImageFunction
 * \brief Nearest-neighbor interpolate a sparse vector image at specified positions.
 *
 * SparseVectorImageNearestNeighborInterpolateImageFunction linearly interpolates a vector
 * image intensity non-integer pixel position. This class is templated
 * over the input image type and the coordinate representation type.
 *
 * This function works for N-dimensional images.
 *
 * \warning This function work only for sparse images. For
 * scalar images use LinearInterpolateImageFunction.
 *
 * \ingroup ImageFunctions ImageInterpolators
 * 
 */
template<class TInputImage, class TCoordRep = double>
class ITK_EXPORT SparseVectorImageNearestNeighborInterpolateImageFunction : public SparseVectorImageInterpolateImageFunction<
    TInputImage, TCoordRep>
{
public:
  /** Standard class typedefs. */
  typedef SparseVectorImageNearestNeighborInterpolateImageFunction Self;
  typedef SparseVectorImageInterpolateImageFunction<TInputImage, TCoordRep> Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(SparseVectorImageNearestNeighborInterpolateImageFunction,
      SparseVectorImageInterpolateImageFunction);

  /** InputImageType typedef support. */
  typedef typename Superclass::InputImageType InputImageType;
  typedef typename Superclass::PixelType PixelType;
  typedef typename Superclass::ValueType ValueType;
  typedef typename Superclass::RealType RealType;
  typedef typename Superclass::RegionType RegionType;
  typedef typename Superclass::SizeType SizeType;

  /** Grab the vector dimension from the superclass. */
//  itkStaticConstMacro(Dimension, unsigned int,
//                       Superclass::Dimension);
  /** Dimension underlying input image. */
  itkStaticConstMacro(ImageDimension, unsigned int,Superclass::ImageDimension);

  /** Point typedef support. */
  typedef typename Superclass::PointType PointType;
  
  /** Index typedef support. */
  typedef typename Superclass::IndexType IndexType;

  /** ContinuousIndex typedef support. */
  typedef typename Superclass::ContinuousIndexType ContinuousIndexType;

  /** Output type is Vector<double,Dimension> */
  typedef typename Superclass::OutputType OutputType;

  /** Convenient typedefs for vector output */
  typedef typename Superclass::VectorOutputType VectorOutputType;
  typedef typename Superclass::VectorOutputIndexType VectorOutputIndexType;
  typedef typename Superclass::VectorOutputPointType VectorOutputPointType;
  typedef typename Superclass::VectorOutputContinuousIndexType VectorOutputContinuousIndexType;

  virtual VectorOutputType VectorEvaluate( const VectorOutputPointType& point ) const
  {
  PointType augmentedPoint;
  ContinuousIndexType augmentedIndex;
  VectorOutputIndexType index;

  for ( unsigned int dim=0; dim<ImageDimension-1; dim++ )
    {
    augmentedPoint[dim+1] = point[dim];
    }
  augmentedPoint[0] = 0;
  
  this->GetInputImage()->TransformPhysicalPointToContinuousIndex( augmentedPoint, augmentedIndex );

  IndexType nnIndex;
//  this->ConvertContinuousIndexToNearestIndex(augmentedIndex, nnIndex);
  nnIndex.CopyWithRound(augmentedIndex);

  for ( unsigned int dim=0; dim<ImageDimension-1; dim++ )
    {
    index[dim] = nnIndex[dim+1];
    }
  
  VectorOutputType output = this->VectorEvaluateAtIndex( index );
  
  return ( output );
  }

  virtual OutputType Evaluate( const PointType& point ) const
    {
    ContinuousIndexType index;
    
    this->GetInputImage()->TransformPhysicalPointToContinuousIndex(
      point, index );
    
    IndexType nnIndex;
//    this->ConvertContinuousIndexToNearestIndex(cindex, nnIndex);
    nnIndex.CopyWithRound(index);
    
    return ( this->EvaluateAtIndex( nnIndex ) );
    }
  
  virtual OutputType EvaluateAtContinuousIndex(
    const ContinuousIndexType & index ) const
    {
    IndexType nnIndex;
    nnIndex.CopyWithRound(index);
//    this->ConvertContinuousIndexToNearestIndex(index, nnIndex);
    return ( this->EvaluateAtIndex(nnIndex) );
    }

  virtual VectorOutputType VectorEvaluateAtContinuousIndex(
      const VectorOutputContinuousIndexType & index ) const
    {
    VectorOutputIndexType nnIndex;
    nnIndex.CopyWithRound(index);
    return ( this->VectorEvaluateAtIndex( nnIndex ) );
    }

protected:
  SparseVectorImageNearestNeighborInterpolateImageFunction() {};
  ~SparseVectorImageNearestNeighborInterpolateImageFunction()
    {};

private:
  SparseVectorImageNearestNeighborInterpolateImageFunction(const Self&); //purposely not implemented
  void operator=(const Self&);//purposely not implemented

};

} // end namespace itk

#endif
