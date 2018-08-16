/*
 * File name: thread.cc
 * Date:      2006/09/11 11:51
 * Author:    Jan Faigl
 */

#include <crl/thread.h>

using namespace crl::concurrent;

/// ----------------------------------------------------------------------------
/// Class CThread
/// ----------------------------------------------------------------------------
CThread::CThread()
{
   _wrapper = 0;
   _thrd = 0;
}

/// ----------------------------------------------------------------------------
CThread::~CThread()
{
   if (isRunning()) {
      join();
   }
   if (_thrd) {
      delete _thrd;
   }
   if (_wrapper) {
      delete _wrapper;
   }
}

/// ----------------------------------------------------------------------------
bool CThread::isRunning(void)
{
   bool ret = false;
   if (_wrapper) {
      ret = _running;
   }
   return ret;
}

/// ----------------------------------------------------------------------------
bool CThread::isFinished(void)
{
   bool ret = false;
   if (_wrapper) {
      ret = _finished;
   }
   return ret;
}

/// ----------------------------------------------------------------------------
void CThread::join(void)
{
   if (isRunning()) {
      _thrd->join(); // can not call join if thread is terminated ???
   }
}

/// ----------------------------------------------------------------------------
void CThread::yield(void)
{
   if (_thrd) {
      _thrd->yield();
   }
}

/// ----------------------------------------------------------------------------
void CThread::execute(void)
{
   _wrapper = new CThreadWrapper<CThread>(this);
   _thrd = new boost::thread(*_wrapper);
}

/// ----------------------------------------------------------------------------
/// Private methods
/// ----------------------------------------------------------------------------
void CThread::terminate(void) { stop(); }

/* end of thread.cc */
