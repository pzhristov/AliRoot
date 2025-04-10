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

/// \file threadserver.h
/// \author David Rohr

#ifndef THREADSERVER_H
#define THREADSERVER_H

#ifdef _WIN32
#include "pthread_mutex_win32_wrapper.h"
#include "sched_affinity_win32_wrapper.h"
#else
#include <pthread.h>
#include <sched.h>
#endif
#include "qsem.h"

class qThreadServerException
{
};

template <class S, class T>
class qThreadCls;

class qThreadParam
{
  template <class S, class T>
  friend class qThreadCls;

 public:
  qThreadParam()
  {
    for (int32_t i = 0; i < 2; i++) {
      threadMutex[i].Lock();
    }
    terminate = false;
    pinCPU = -1;
  }

  ~qThreadParam()
  {
    for (int32_t i = 0; i < 2; i++) {
      threadMutex[i].Unlock();
    }
  }

  bool WaitForTask()
  {
    threadMutex[1].Unlock();
    threadMutex[0].Lock();
    return (!terminate);
  }

  int32_t threadNum;

 protected:
  int32_t pinCPU;
  qSem threadMutex[2];
  volatile bool terminate;
};

template <class S>
class qThreadParamCls : public qThreadParam
{
  template <class SS, class TT>
  friend class qThreadCls;

 private:
  S* pCls;
  void (S::*pFunc)(void*);
};

template <class S, class T>
static void* qThreadWrapperCls(void* arg);

template <class S, class T>
class qThreadCls
{
 public:
  qThreadCls() { started = false; };
  qThreadCls(S* pCls, void (S::*pFunc)(T*), int32_t threadNum = 0, int32_t pinCPU = -1) : threadParam()
  {
    started = false;
    SpawnThread(pCls, pFunc, threadNum, pinCPU);
  }

  void SpawnThread(S* pCls, void (S::*pFunc)(T*), int32_t threadNum = 0, int32_t pinCPU = -1, bool wait = true)
  {
    qThreadParamCls<S>& XthreadParam = *((qThreadParamCls<S>*)&this->threadParam);

    XthreadParam.pCls = pCls;
    XthreadParam.pFunc = (void (S::*)(void*))pFunc;
    XthreadParam.threadNum = threadNum;
    XthreadParam.pinCPU = pinCPU;
    pthread_t thr;
    pthread_create(&thr, nullptr, (void* (*)(void*)) & qThreadWrapperCls, &XthreadParam);
    if (wait) {
      WaitForSpawn();
    }
    started = true;
  }

  void WaitForSpawn() { threadParam.threadMutex[1].Lock(); }

  ~qThreadCls()
  {
    if (started) {
      End();
    }
  }

  void End()
  {
    qThreadParamCls<S>& XthreadParam = *((qThreadParamCls<S>*)&this->threadParam);

    XthreadParam.terminate = true;
    XthreadParam.threadMutex[0].Unlock();
    XthreadParam.threadMutex[1].Lock();
    started = false;
  }

  void Start() { threadParam.threadMutex[0].Unlock(); }

  void Sync() { threadParam.threadMutex[1].Lock(); }

 private:
  bool started;
  T threadParam;

  static void* qThreadWrapperCls(T* arg);
};

template <class S, class T>
void* qThreadCls<S, T>::qThreadWrapperCls(T* arg)
{
  qThreadParamCls<S>* const arg_A = (qThreadParamCls<S>*)arg;
  if (arg_A->pinCPU != -1) {
    cpu_set_t tmp_mask;
    CPU_ZERO(&tmp_mask);
    CPU_SET(arg_A->pinCPU, &tmp_mask);
    sched_setaffinity(0, sizeof(tmp_mask), &tmp_mask);
  }

  void (S::*pFunc)(T*) = (void (S::*)(T*))arg_A->pFunc;
  (arg_A->pCls->*pFunc)(arg);

  arg_A->threadMutex[1].Unlock();
  pthread_exit(nullptr);
  return (nullptr);
}

template <class S, class T>
class qThreadClsArray
{
 public:
  qThreadClsArray()
  {
    pArray = nullptr;
    nThreadsRunning = 0;
  }
  qThreadClsArray(int32_t n, S* pCls, void (S::*pFunc)(T*), int32_t threadNumOffset = 0, int32_t* pinCPU = nullptr)
  {
    pArray = nullptr;
    nThreadsRunning = 0;
    SetNumberOfThreads(n, pCls, pFunc, threadNumOffset, pinCPU);
  }

  void SetNumberOfThreads(int32_t n, S* pCls, void (S::*pFunc)(T*), int32_t threadNumOffset = 0, int32_t* pinCPU = nullptr)
  {
    if (nThreadsRunning) {
      fprintf(STD_OUT, "Threads already started\n");
      throw(qThreadServerException());
    }
    pArray = new qThreadCls<S, T>[n];
    nThreadsRunning = n;
    for (int32_t i = 0; i < n; i++) {
      pArray[i].SpawnThread(pCls, pFunc, threadNumOffset + i, pinCPU == nullptr ? -1 : pinCPU[i], false);
    }
    for (int32_t i = 0; i < n; i++) {
      pArray[i].WaitForSpawn();
    }
  }

  ~qThreadClsArray()
  {
    if (nThreadsRunning) {
      EndThreads();
    }
  }

  void EndThreads()
  {
    delete[] pArray;
    nThreadsRunning = 0;
  }

  void Start()
  {
    for (int32_t i = 0; i < nThreadsRunning; i++) {
      pArray[i].Start();
    }
  }

  void Sync()
  {
    for (int32_t i = 0; i < nThreadsRunning; i++) {
      pArray[i].Sync();
    }
  }

 private:
  qThreadCls<S, T>* pArray;
  int32_t nThreadsRunning;
};

#endif
