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
#ifndef __itkShrinkSparseVectorImageFilter_h
#define __itkShrinkSparseVectorImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkResampleSparseVectorImageFilter.h"

namespace itk
{
/** \class ShrinkSparseVectorImageFilter
 * \brief Reduce the size of an object of itk::SparseVectorImage by a real factor
 * in each dimension.
 *
 * This is adapted from itk::ShrinkImageFilter but with more practical 
 * extensions. To be more specific, we 
 *   \li \c avoid to shrink the input image to a single voxel
 *   \li \c use more sophisticated resampling technique
 *
 * \author Pei Zhang @ UNC-Chapel Hill
 * 
 * \ingroup GeometricTransform Streamed
 * \ingroup ITKImageGrid
 * \ingroup ITKSparseVectorImage
 */
template<class TInputImage, class TOutputImage>
class ITK_EXPORT ShrinkSparseVectorImageFilter:
  public ImageToImageFilter<TInputImage, TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef ShrinkSparseVectorImageFilter                             Self;
  typedef ImageToImageFilter<TInputImage, TOutputImage>             Superclass;
  typedef SmartPointer<Self>                                        Pointer;
  typedef SmartPointer<const Self>                                  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro( Self );

  /** 
   * Extract some information from the image types.  Dimensionality
   * of the two images is assumed to be the same. 
   */
  itkStaticConstMacro( InputImageDimension, unsigned int, TInputImage::ImageDimension );
  itkStaticConstMacro( OutputImageDimension, unsigned int, TOutputImage::ImageDimension );

  /** Run-time type information (and related methods) */
  itkTypeMacro( ShrinkSparseVectorImageFilter, ImageToImageFilter );

  /** Image typedef support */
  typedef TInputImage                                               InputImageType;
  typedef TOutputImage                                              OutputImageType;
  typedef typename InputImageType::Pointer                          InputImagePointer;
  typedef typename OutputImageType::Pointer                         OutputImagePointer;
  typedef typename InputImageType::ConstPointer                     InputImageConstPointer;

  typedef typename InputImageType::PixelType                        InputPixelType;
  typedef typename InputImageType::IndexType                        InputIndexType;
  typedef typename OutputImageType::PixelType                       OutputPixelType;
  typedef typename NumericTraits<OutputPixelType>::ElementRealType  RealType;

  typedef FixedArray<RealType, InputImageDimension>                 ShrinkFactorsType;
  typedef FixedArray<unsigned int, InputImageDimension>             MinSizeType;

  typedef ResampleSparseVectorImageFilter<TInputImage, TOutputImage>
                                                                    ResampleFilterType;
  typedef typename ResampleFilterType::Pointer                      ResampleFilterPointer;

  /** 
   * Set the shrinkage factors. Values are clamped to
   * a minimum value of 1.0. Default is 1.0 for all dimensions.
   */
  itkSetMacro( ShrinkFactors, ShrinkFactorsType );

  void SetShrinkFactors( RealType );

  void SetShrinkFactor( unsigned int, RealType );

  /** Get the shrink factors. */
  itkGetConstReferenceMacro ( ShrinkFactors, ShrinkFactorsType );

  /** Set the minimum size of the shrinked image. */
  itkSetMacro( MinSize, MinSizeType );

  void SetMinSize( unsigned int );

  void SetMinSize( unsigned int, unsigned int );

  /** Get the minimum size of the shrinked image. */
  itkGetConstReferenceMacro ( MinSize, MinSizeType );

#ifdef ITK_USE_CONCEPT_CHECKING
  /** Begin concept checking */
  itkConceptMacro( ImageDimensionCheck,
    ( Concept::SameDimension<itkGetStaticConstMacro( InputImageDimension ),
                             itkGetStaticConstMacro( OutputImageDimension )> ) );
  /** End concept checking */
#endif

protected:
  ShrinkSparseVectorImageFilter();

  virtual ~ShrinkSparseVectorImageFilter() {}

  virtual void GenerateOutputInformation();

  virtual void GenerateData();

  void PrintSelf( std::ostream&, Indent ) const;


private:
  ShrinkSparseVectorImageFilter( const Self& ); //purposely not implemented
  void operator=( const Self& ); //purposely not implemented

  /** Shrinkage factors along all directions. */
  ShrinkFactorsType                                                 m_ShrinkFactors;

  /** Minimum size of the shrinked image. Default set to 5 voxels. */
  MinSizeType                                                       m_MinSize;
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkShrinkSparseVectorImageFilter.hxx"
#endif

#endif
