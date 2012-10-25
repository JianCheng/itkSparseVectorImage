
/* This class was posted to the ITK users list by Karthik Krishnan
 * http://www.itk.org/pipermail/insight-users/2008-July/026542.html */

#ifndef __itkSparseVectorImageInterpolateImageFunction_h
#define __itkSparseVectorImageInterpolateImageFunction_h

#include "itkImageFunction.h"
#include "itkVariableLengthVector.h"
#include "itkPoint.h"

namespace itk
{

template <
      class TInputImage,
      class TCoordRep = float,
      class TPixelType = typename TInputImage::PixelType
      >
      class ITK_EXPORT SparseVectorImageInterpolateImageFunction :
        public ImageFunction<
        TInputImage,
        typename TInputImage::PixelType,
        TCoordRep >
{
public:
  /** Dimension underlying input image. */
  itkStaticConstMacro(ImageDimension, unsigned int,
    TInputImage::ImageDimension);

  /** Standard class typedefs. */
  typedef SparseVectorImageInterpolateImageFunction Self;
//  typedef ImageFunction<TInputImage,
//          VariableLengthVector<double >, TCoordRep > Superclass;
  typedef ImageFunction<TInputImage,
          typename TInputImage::PixelType, TCoordRep > Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro(SparseVectorImageInterpolateImageFunction, ImageFunction);

  /** InputImageType typedef support. */
  typedef typename Superclass::InputImageType          InputImageType;
  typedef typename InputImageType::PixelType           PixelType;
//  typedef typename PixelType::ValueType       ValueType;
  typedef typename InputImageType::PixelType           ValueType;
  typedef typename NumericTraits<ValueType>::RealType  RealType;
  typedef typename InputImageType::RegionType          RegionType;
  typedef typename InputImageType::SizeType            SizeType;

  /** Point typedef support. */
  typedef typename Superclass::PointType PointType;

  /** Index typedef support. */
  typedef typename Superclass::IndexType IndexType;

  /** ContinuousIndex typedef support. */
  typedef typename Superclass::ContinuousIndexType ContinuousIndexType;

  /** Output type is FixedArray<RealType,Dimension>. */
  typedef typename Superclass::OutputType OutputType;
  
  /** CoordRep typedef support. */
  typedef TCoordRep CoordRepType;

  /** Convenient typedefs for vector output */
  typedef VariableLengthVector<RealType>     VectorOutputType;
  typedef Index<ImageDimension - 1>          VectorOutputIndexType;
  typedef Point<TCoordRep,ImageDimension-1>  VectorOutputPointType;
  typedef ContinuousIndex<TCoordRep,ImageDimension-1>
                                             VectorOutputContinuousIndexType;

  /** Returns the interpolated image intensity at a
    * specified point position. No bounds checking is done.
    * The point is assume to lie within the image buffer.
    * ImageFunction::IsInsideBuffer() can be used to check bounds before
    * calling the method. */
  virtual OutputType Evaluate( const PointType& point ) const
    {
    ContinuousIndexType index;
    this->GetInputImage()->TransformPhysicalPointToContinuousIndex( 
      point, index );
    return ( this->EvaluateAtContinuousIndex( index ) );
    }

 // for VectorImage

  virtual VectorOutputType VectorEvaluate( const VectorOutputPointType& point ) const    
    {
    PointType augmentedPoint;
    ContinuousIndexType augmentedIndex;
    VectorOutputContinuousIndexType index;

    for ( unsigned int dim=0; dim<ImageDimension-1; dim++ )
      {
      augmentedPoint[dim+1] = point[dim];
      }
    augmentedPoint[0] = 0;

//    OutputContinuousIndexType index;
    //std::cout << "augmentedPoint before = " << augmentedPoint << std::endl; 
    this->GetInputImage()->TransformPhysicalPointToContinuousIndex( augmentedPoint, augmentedIndex );
    //std::cout << "augmentedPoint after = " << augmentedPoint << std::endl; 
   
    for ( unsigned int dim=0; dim<ImageDimension-1; dim++ )
      {
      index[dim] = augmentedIndex[dim+1];
      }

    return ( this->VectorEvaluateAtContinuousIndex( index ) );
    }

  /** Interpolate the image at a continuous index position
    *
    * Returns the interpolated image intensity at a
    * specified index position. No bounds checking is done.
    * The point is assume to lie within the image buffer.
    *
    * Subclasses must override this method.
    *
    * ImageFunction::IsInsideBuffer() can be used to check bounds before
    * calling the method. */
  virtual OutputType EvaluateAtContinuousIndex(
    const ContinuousIndexType & index ) const = 0;

  // for VectorImage
  virtual VectorOutputType VectorEvaluateAtContinuousIndex( 
    const VectorOutputContinuousIndexType & index ) const = 0;

  /** Interpolate the image at an index position.
    * Simply returns the image value at the
    * specified index position. No bounds checking is done.
    * The point is assume to lie within the image buffer.
    *
    * ImageFunction::IsInsideBuffer() can be used to check bounds before
    * calling the method. */
  virtual OutputType EvaluateAtIndex( const IndexType & index ) const
    {
    PixelType input = this->GetInputImage()->GetPixel( index );
    OutputType output = static_cast<OutputType>( input );
    return ( output );
    }

  virtual VectorOutputType VectorEvaluateAtIndex( const VectorOutputIndexType & index ) const
    {
    RegionType region = this->GetInputImage()->GetLargestPossibleRegion();
    SizeType size = region.GetSize();
    unsigned int VectorDimension = size[0];

    IndexType augmentedIndex;
    for ( unsigned int dim=0; dim<ImageDimension-1; dim++ )
      {
      augmentedIndex[dim+1] = index[dim];
      }

    VectorOutputType output;
    output.Fill( 0 );
    
    for ( unsigned int k=0; k<VectorDimension; k++ )
      {
      augmentedIndex[0] = k;
      PixelType input = this->GetInputImage()->GetPixel( augmentedIndex );
      output[k] = static_cast<RealType>( input );
      }
      return ( output );
    }


protected:
  SparseVectorImageInterpolateImageFunction() {}
  ~SparseVectorImageInterpolateImageFunction() {}
  void PrintSelf(std::ostream& os, Indent indent) const
    { Superclass::PrintSelf( os, indent ); }

private:
  SparseVectorImageInterpolateImageFunction(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented

};

} // end namespace itk

#endif
