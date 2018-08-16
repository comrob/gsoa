/*
 * File name: waitobject.cc
 * Date:      2007/05/03 21:57
 * Author:    Jan Faigl
 */

#include <crl/waitobject.h>

using namespace crl::concurrent;

/// ----------------------------------------------------------------------------
/// Class CWaitObject
/// ----------------------------------------------------------------------------
CWaitObject::CWaitObject() { n = 0; }

/// ----------------------------------------------------------------------------
CWaitObject::~CWaitObject() {}

/// ----------------------------------------------------------------------------
void CWaitObject::notify(void)
{
   ScopedLock lk(mtx);
   n++;
   cond.notify_one();
}

/// ----------------------------------------------------------------------------
void CWaitObject::wait(void)
{
   ScopedLock lk(mtx);
   if (n == 0) {
      cond.wait(lk);
      n--;
   } else {
      n--;
   }
}

/* end of waitobject.cc */
