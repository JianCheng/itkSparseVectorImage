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
#ifndef __itkSparseVectorImageLinearInterpolateImageFunction_h
#define __itkSparseVectorImageLinearInterpolateImageFunction_h

#include "itkSparseVectorImageInterpolateImageFunction.h"

namespace itk
{

/** 
 * \class SparseVectorImageLinearInterpolateImageFunction
 * \brief Linearly interpolate a sparse vector image at specified positions.
 *
 * SparseVectorImageLinearInterpolateImageFunction linearly interpolates a sparse
 * vector image intensity at non-integer pixel position. This class is templated
 * over the input image type and the coordinate representation type.
 *
 * This function works for N-dimensional images.
 *
 * \warning This function work only for itk::SparseVectorImage. For
 * itk::MySparseImage, use SparseImageLinearInterpolateImageFunction.
 *
 * \author Pei Zhang @ UNC-Chapel Hill
 *
 * \ingroup ImageFunctions ImageInterpolators
 *
 * \ingroup ITKImageFunction
 * 
 */
template<typename TInputImage, typename TCoordRep = double>
class ITK_EXPORT SparseVectorImageLinearInterpolateImageFunction :
  public SparseVectorImageInterpolateImageFunction<TInputImage, TCoordRep>
{
public:
  /** Standard class typedefs. */
  typedef SparseVectorImageLinearInterpolateImageFunction             Self;
  typedef SparseVectorImageInterpolateImageFunction<TInputImage, TCoordRep>
                                                                      Superclass;
  typedef SmartPointer<Self>                                          Pointer;
  typedef SmartPointer<const Self>                                    ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro( Self );

  /** Run-time type information (and related methods). */
  itkTypeMacro( SparseVectorImageLinearInterpolateImageFunction,
                SparseVectorImageInterpolateImageFunction );

  /** Dimension underlying input image. */
  itkStaticConstMacro( ImageDimension, unsigned int, Superclass::ImageDimension );

  /** Input typedefs for the images. */
  typedef typename Superclass::InputImageType                         InputImageType;
  typedef typename Superclass::PixelType                              PixelType;
  typedef typename Superclass::ValueType                              ValueType;
  typedef typename Superclass::RealType                               RealType;

  /** Index typedef support. */
  typedef typename Superclass::IndexType                              IndexType;

  /** ContinuousIndex typedef support. */
  typedef typename Superclass::ContinuousIndexType                    ContinuousIndexType;

  /** Output type is Vector<double,Dimension> */
  typedef typename Superclass::OutputType                             OutputType;

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
      const ContinuousIndexType & index ) const;

protected:
  SparseVectorImageLinearInterpolateImageFunction();
  ~SparseVectorImageLinearInterpolateImageFunction() {}
  void PrintSelf(std::ostream& os, Indent indent) const;

private:
  SparseVectorImageLinearInterpolateImageFunction(const Self&); //purposely not implemented
  void operator=(const Self&);//purposely not implemented

  /** Number of neighbors used in the interpolation */
  static const unsigned long m_Neighbors;
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkSparseVectorImageLinearInterpolateImageFunction.hxx"
#endif

#endif
