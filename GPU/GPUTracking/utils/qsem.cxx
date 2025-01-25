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

/// \file qsem.cxx
/// \author David Rohr

#include <cerrno>
#include <cstdio>

#include "qsem.h"

#ifndef STD_OUT
#define STD_OUT stdout
#endif

qSem::qSem(int32_t num)
{
  max = num;
  if (sem_init(&sem, 0, num)) {
    fprintf(STD_OUT, "Error initializing semaphore");
  }
}

qSem::~qSem()
{
  if (sem_destroy(&sem)) {
    fprintf(STD_OUT, "Error destroying semaphore");
  }
}

int32_t qSem::Lock()
{
  int32_t retVal;
  if ((retVal = sem_wait(&sem))) {
    fprintf(STD_OUT, "Error locking semaphore");
  }
  return (retVal);
}

int32_t qSem::Unlock()
{
  int32_t retVal;
  if ((retVal = sem_post(&sem))) {
    fprintf(STD_OUT, "Error unlocking semaphire");
  }
  return (retVal);
}

int32_t qSem::Trylock()
{
  int32_t retVal = sem_trywait(&sem);
  if (retVal) {
    if (errno == EAGAIN) {
      return (EBUSY);
    }
    return (-1);
  }
  return (0);
}

#ifndef _WIN32
int32_t qSem::Query()
{
  int32_t value;
  if (sem_getvalue(&sem, &value) != 0) {
    value = -1;
  }
  return (value);
}
#endif
