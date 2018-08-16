/*
 * File name: proc_signal.cc
 * Date:      2008/10/06 07:50
 * Author:    Jan Faigl
 */

#include <signal.h>
#include <unistd.h>

#include <crl/assert.h>
#include <crl/proc_signal.h>

using namespace crl;

/// ----------------------------------------------------------------------------
/// Class CProcSignal
/// ----------------------------------------------------------------------------
CProcSignal::SignalHandlerArray CProcSignal::signals[MAX_SIGNAL];

/// ----------------------------------------------------------------------------
void CProcSignal::addHandler(int sig, CSignalHandler *handler)
{
   assert_argument(sig > 0 && sig < MAX_SIGNAL, "Signal number out of range");
   SignalHandlerArray &handlers(signals[sig]);
   handlers.push_back(handler);
   if (handlers.size() == 1) {
      signal(sig, CProcSignal::catch_sig);
   }
}

/// ----------------------------------------------------------------------------
void CProcSignal::delHandler(int sig, CSignalHandler *handler)
{
   assert_argument(sig > 0 && sig < MAX_SIGNAL, "Signal number out of range");
   SignalHandlerArray &handlers(signals[sig]);
   for (SignalHandlerArray::iterator it = handlers.begin();
        it != handlers.end(); it++) {
      if (*it == handler) {
         handlers.erase(it);
         break;
      }
   }
}

/// ----------------------------------------------------------------------------
/// private methods
/// ----------------------------------------------------------------------------
void CProcSignal::catch_sig(int sig_num)
{
   SignalHandlerArray &handlers(signals[sig_num]);
   for (SignalHandlerArray::iterator it = handlers.begin();
        it != handlers.end(); it++) {
      (**it)(sig_num);
   }
   if (handlers.size() > 0) {
      signal(sig_num, CProcSignal::catch_sig);
   }
}

/* end of signal.cc */
