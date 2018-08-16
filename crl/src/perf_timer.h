/*
 * File name: perf_timer.h
 * Date:      2009/02/18 17:36
 * Author:    Jan Faigl
 */

#ifndef __PERF_TIMER_H__
#define __PERF_TIMER_H__

#include <crl/logging.h>
#include <crl/timerN.h>

namespace crl {

class CPerfTimer {
 public:
   CPerfTimer() {}

   CPerfTimer(const std::string &str) { start(str); }

   ~CPerfTimer() { stop(); }

   CPerfTimer &start(const std::string &s)
   {
      str = s;
      t.reset();
      t.start();
      return *this;
   }

   CPerfTimer &restart(const std::string &s)
   {
      if (t.enabled()) {
         stop();
      }
      start(s);
      return *this;
   }

   CPerfTimer &stop(void)
   {
      if (t.enabled()) {
         t.stop();
         INFO(str << " real: " << t.realTime() << " ms cpu: " << t.cpuTime()
                  << " ms usage: " << (100.0 * t.cpuTime() / t.realTime())
                  << " %");
      }
      return *this;
   }

 private:
   std::string str;
   CTimerN t;
};

} // end namespace crl

#endif

/* end of perf_timer.h */
