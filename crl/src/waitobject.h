/*
 * File name: waitobject.h
 * Date:      2007/05/03 21:56
 * Author:    Jan Faigl
 */

#ifndef __BOOSTWAITOBJECT_H__
#define __BOOSTWAITOBJECT_H__

#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>

namespace crl { namespace concurrent {

   class CWaitObject {
    public:
      typedef boost::mutex::scoped_lock ScopedLock;
      typedef boost::mutex Mutex;
      typedef boost::condition Condition;

      CWaitObject();
      ~CWaitObject();

      void notify(void);
      void wait(void);

    private:
      unsigned int n;
      Mutex mtx;
      Condition cond;
   };

}} // namespace crl::concurrent

#endif

/* end of waitobject.h */
