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

/// \file strtag.h
/// \author David Rohr

#ifndef STRTAG_H
#define STRTAG_H

#include <stdexcept>
#include <string>

template <class T = uint64_t>
constexpr T qStr2Tag(const char* str)
{
  if (strlen(str) != sizeof(T)) {
    throw std::runtime_error("Invalid tag length");
  }
  T tmp;
  for (uint32_t i = 0; i < sizeof(T); i++) {
    ((char*)&tmp)[i] = str[i];
  }
  return tmp;
}

template <class T>
std::string qTag2Str(const T tag)
{
  T str[2];
  str[0] = tag;
  str[1] = 0;
  return std::string((const char*)str);
}

#endif
