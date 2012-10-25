/*=========================================================================

 Program:   Insight Segmentation & Registration Toolkit
 Module:    $RCSfile: itkSparseVectorImageLinearInterpolateImageFunction.h,v $
 Language:  C++
 Date:      $Date$
 Version:   $Revision$

 Copyright (c) Insight Software Consortium. All rights reserved.
 See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.

 =========================================================================*/
#ifndef __itkSparseVectorImageLinearInterpolateImageFunction_h
#define __itkSparseVectorImageLinearInterpolateImageFunction_h

#include "itkSparseVectorImageInterpolateImageFunction.h"

namespace itk
{

/** 
 * \class SparseVectorImageLinearInterpolateImageFunction
 * \brief Linearly interpolate a sparse vector image at specified positions.
 *
 * SparseVectorImageLinearInterpolateImageFunction linearly interpolates a vector
 * image intensity non-integer pixel position. This class is templated
 * over the input image type and the coordinate representation type.
 *
 * This function works for N-dimensional images.
 *
 * \warning This function work only for Vector images. For
 * scalar images use LinearInterpolateImageFunction.
 *
 * \ingroup ImageFunctions ImageInterpolators
 * 
 */
template<class TInputImage, class TCoordRep = double>
class ITK_EXPORT SparseVectorImageLinearInterpolateImageFunction : public SparseVectorImageInterpolateImageFunction<
    TInputImage, TCoordRep>
{
public:
  /** Standard class typedefs. */
  typedef SparseVectorImageLinearInterpolateImageFunction Self;
  typedef SparseVectorImageInterpolateImageFunction<TInputImage, TCoordRep> Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self)
  ;

  /** Run-time type information (and related methods). */
  itkTypeMacro(SparseVectorImageLinearInterpolateImageFunction,
      SparseVectorImageInterpolateImageFunction)
  ;

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

  /** Evaluate the function at a ContinuousIndex position
   *
   * Returns the linearly interpolated image intensity at a 
   * specified point position. No bounds checking is done.
   * The point is assume to lie within the image buffer.
   *
   * ImageFunction::IsInsideBuffer() can be used to check bounds before
   * calling the method. */
  virtual OutputType EvaluateAtContinuousIndex(
      const ContinuousIndexType & index ) const;

  virtual VectorOutputType VectorEvaluateAtContinuousIndex(
      const VectorOutputContinuousIndexType & index ) const;

protected:
  SparseVectorImageLinearInterpolateImageFunction();
  ~SparseVectorImageLinearInterpolateImageFunction()
    {};
  void PrintSelf(std::ostream& os, Indent indent) const;

private:
  SparseVectorImageLinearInterpolateImageFunction(const Self&); //purposely not implemented
  void operator=(const Self&);//purposely not implemented

  /** Number of neighbors used in the interpolation */
  static const unsigned long m_Neighbors;

};

} // end namespace itk

// Define instantiation macro for this template.
#define ITK_TEMPLATE_SparseVectorImageLinearInterpolateImageFunction(_, EXPORT, x, y) namespace itk { \
  _(2(class EXPORT SparseVectorImageLinearInterpolateImageFunction< ITK_TEMPLATE_2 x >)) \
  namespace Templates { typedef SparseVectorImageLinearInterpolateImageFunction< ITK_TEMPLATE_2 x > \
                                                  SparseVectorImageLinearInterpolateImageFunction##y; } \
  }

#if ITK_TEMPLATE_EXPLICIT
# include "Templates/itkSparseVectorImageLinearInterpolateImageFunction+-.h"
#endif

#if ITK_TEMPLATE_TXX
# include "itkSparseVectorImageLinearInterpolateImageFunction.hxx"
#endif

#endif
