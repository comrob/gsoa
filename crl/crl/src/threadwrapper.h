/*
 * File name: threadwrapper.h
 * Date:      2006/09/11 11:39
 * Author:    Jan Faigl
 */

#ifndef __THREADWRAPPER_H__
#define __THREADWRAPPER_H__

namespace crl { namespace concurrent {
   /// ----------------------------------------------------------------------------
   /// @brief Thread wrapper
   /// ----------------------------------------------------------------------------
   template <class T> class CThreadWrapper {
    public:
      typedef T ThreadClass;

      CThreadWrapper(ThreadClass *thd) : _thd(thd) {}
      void operator()()
      {
         _thd->_running = true;
         _thd->_finished = false;
         _thd->threadBody();
         _thd->_running = false;
         _thd->_finished = true;
      }

    private:
      ThreadClass *_thd;
   };

}} // namespace crl::concurrent

#endif

/* end of threadwrapper.h */
