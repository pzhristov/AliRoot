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

/// \file qsem.h
/// \author David Rohr

#ifndef QSEM_H
#define QSEM_H

#ifdef _WIN32
#include "pthread_mutex_win32_wrapper.h"
#else
#include <semaphore.h>
#endif

class qSem
{
 public:
  qSem(int32_t num = 1);
  ~qSem();

  int32_t Lock();
  int32_t Unlock();
  int32_t Trylock();
  int32_t Query();

 private:
  int32_t max;
  sem_t sem;
};

class qSignal
{
 private:
  qSem sem;

 public:
  qSignal() : sem(0) {}
  void Wait() { sem.Lock(); }
  void Signal() { sem.Unlock(); }
};

#endif
