//**************************************************************************\
//* This file is property of and copyright by the ALICE Project            *\
//* ALICE Experiment at CERN, All rights reserved.                         *\
//*                                                                        *\
//* Primary Authors: Matthias Richter <Matthias.Richter@ift.uib.no>        *\
//*                  for The ALICE HLT Project.                            *\
//*                                                                        *\
//* Permission to use, copy, modify and distribute this software and its   *\
//* documentation strictly for non-commercial purposes is hereby granted   *\
//* without fee, provided that the above copyright notice appears in all   *\
//* copies and that both the copyright notice and this permission notice   *\
//* appear in the supporting documentation. The authors make no claims     *\
//* about the suitability of this software for any purpose. It is          *\
//* provided "as is" without express or implied warranty.                  *\
//**************************************************************************

/// \file GPUROOTSMatrixFwd.h
/// \author Matteo Concas

#ifndef GPUROOTSMATRIXFWD_H
#define GPUROOTSMATRIXFWD_H

// Standalone forward declarations for Svector / SMatrix / etc.
// To be used on GPU where ROOT is not available.

#include "GPUCommonDef.h"

namespace ROOT
{
namespace Math
{
template <typename T, uint32_t N>
class SVector;
template <class T, uint32_t D1, uint32_t D2, class R>
class SMatrix;
template <class T, uint32_t D>
class MatRepSym;
template <class T, uint32_t D1, uint32_t D2>
class MatRepStd;
} // namespace Math
} // namespace ROOT

namespace o2
{
namespace math_utils
{

namespace detail
{
template <typename T, uint32_t N>
class SVectorGPU;
template <class T, uint32_t D1, uint32_t D2, class R>
class SMatrixGPU;
template <class T, uint32_t D>
class MatRepSymGPU;
template <class T, uint32_t D1, uint32_t D2>
class MatRepStdGPU;
} // namespace detail

#if !defined(GPUCA_STANDALONE) && !defined(GPUCA_GPUCODE) && !defined(GPUCOMMONRTYPES_H_ACTIVE)
template <typename T, uint32_t N>
using SVector = ROOT::Math::SVector<T, N>;
template <class T, uint32_t D1, uint32_t D2, class R>
using SMatrix = ROOT::Math::SMatrix<T, D1, D2, R>;
template <class T, uint32_t D>
using MatRepSym = ROOT::Math::MatRepSym<T, D>;
template <class T, uint32_t D1, uint32_t D2 = D1>
using MatRepStd = ROOT::Math::MatRepStd<T, D1, D2>;
#else
template <typename T, uint32_t N>
using SVector = detail::SVectorGPU<T, N>;
template <class T, uint32_t D1, uint32_t D2 = D1, class R = detail::MatRepStdGPU<T, D1, D2>>
using SMatrix = detail::SMatrixGPU<T, D1, D2, R>;
template <class T, uint32_t D>
using MatRepSym = detail::MatRepSymGPU<T, D>;
template <class T, uint32_t D1, uint32_t D2 = D1>
using MatRepStd = detail::MatRepStdGPU<T, D1, D2>;
#endif

} // namespace math_utils
} // namespace o2

#endif
