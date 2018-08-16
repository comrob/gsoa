/*
 * File name: proc_signal.h
 * Date:      2008/10/06 07:50
 * Author:    Jan Faigl
 */

#ifndef __PROC_SIGNAL_H__
#define __PROC_SIGNAL_H__

#include <vector>

namespace crl {

class CSignalHandler {
 public:
   virtual ~CSignalHandler() {}
   virtual void operator()(int sig_num) = 0;
};

class CProcSignal {
   enum { MAX_SIGNAL = 33 };
   typedef std::vector<CSignalHandler *> SignalHandlerArray;

 public:
   static void addHandler(int sig, CSignalHandler *handler);
   static void delHandler(int sig, CSignalHandler *handler);

 private:
   static void catch_sig(int sig_num);

 private:
   static SignalHandlerArray signals[MAX_SIGNAL];
};

} // end namespace crl

#endif

/* end of proc_signal.h */
