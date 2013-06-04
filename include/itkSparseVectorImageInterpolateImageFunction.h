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
#ifndef __itkSparseVectorImageInterpolateImageFunction_h
#define __itkSparseVectorImageInterpolateImageFunction_h

#include "itkImageFunction.h"
#include "itkVariableLengthVector.h"

namespace itk
{

/** \class SparseVectorImageInterpolateImageFunction
 * \brief Base class for all itk::SparseVectorImage interpolaters
 *
 * SparseVectorImageInterpolateImageFunction is the base for all ImageFunctions
 * that interpolates image with sparse vector pixel types. This function outputs
 * a return value of type VariableLengthVector<double>.
 *
 * This class is templated input image type and the coordinate
 * representation type.
 *
 * \warning This hierarchy of functions work only for itk::SparseVectorImage.
 * For itk::MySparseImage use SparseImageInterpolateImageFunction.
 *
 * \author Pei Zhang @ UNC-Chapel Hill
 *
 * \sa InterpolateImageFunction
 * \ingroup ImageFunctions ImageInterpolators
 * \ingroup ITKImageFunction
 * \ingroup ITKSparseVectorImage
 *
 */
template<typename TInputImage, typename TCoordRep = double>
class ITK_EXPORT SparseVectorImageInterpolateImageFunction :
  public ImageFunction<TInputImage,
                       VariableLengthVector<typename NumericTraits<typename TInputImage::ValueType>::RealType>,
                       TCoordRep>
{
public:
  /** Standard class typedefs. */
  typedef SparseVectorImageInterpolateImageFunction                   Self;
  typedef ImageFunction<TInputImage,
                        VariableLengthVector<typename NumericTraits<typename TInputImage::ValueType>::RealType>,
                        TCoordRep>                                    Superclass;
  typedef SmartPointer<Self>                                          Pointer;
  typedef SmartPointer<const Self>                                    ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro( SparseVectorImageInterpolateImageFunction, ImageFunction );

  /** ImageDimension constants */
  itkStaticConstMacro( ImageDimension, unsigned int, TInputImage::ImageDimension );

  /** Input typedefs for the images. */
  typedef typename Superclass::InputImageType                         InputImageType;
  typedef typename InputImageType::PixelType                          PixelType;
  typedef typename InputImageType::ValueType                          ValueType;
  typedef typename NumericTraits<ValueType>::RealType                 RealType;

  /** Point typedef support. */
  typedef typename Superclass::PointType                              PointType;

  /** Index typedef support. */
  typedef typename Superclass::IndexType                              IndexType;

  /** ContinuousIndex typedef support. */
  typedef typename Superclass::ContinuousIndexType                    ContinuousIndexType;

  /** Output type is VariableLengthVector<RealType>. */
  typedef typename Superclass::OutputType                             OutputType;
  
  /** CoordRep typedef support. */
  typedef TCoordRep                                                   CoordRepType;

  /** Interpolate the image at a point position
   *
   * Returns the interpolated image intensity at a
   * specified point position. No bounds checking is done.
   * The point is assume to lie within the image buffer.
   *
   * ImageFunction::IsInsideBuffer() can be used to check bounds before
   * calling the method. */
  virtual OutputType Evaluate( const PointType& point ) const
  {
    ContinuousIndexType index;

    this->GetInputImage()->TransformPhysicalPointToContinuousIndex(point, index);
    return ( this->EvaluateAtContinuousIndex(index) );
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

  /** Interpolate the image at an index position.
   *
   * Simply returns the image value at the
   * specified index position. No bounds checking is done.
   * The point is assume to lie within the image buffer.
   *
   * ImageFunction::IsInsideBuffer() can be used to check bounds before
   * calling the method. */
  virtual OutputType EvaluateAtIndex( const IndexType& index ) const
  {
    PixelType input = this->GetInputImage()->GetPixel( index );
    OutputType output( input.GetSize() );
    for( unsigned int k = 0; k < input.GetSize(); k++ )
    {
      output[k] = static_cast<RealType>( input[k] );
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
