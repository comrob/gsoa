/*
 * File name: algorithm.h
 * Date:      2010/03/25 15:20
 * Author:    Jan Faigl
 */

#ifndef __ALGORITHM_H__
#define __ALGORITHM_H__

#include <string>

#include <crl/config.h>
#include <crl/timerN.h>
#include <crl/gui/canvas.h>

#include <crl/alg/result_log.h>

namespace crl {

class CAlgorithm {
 public:
   static CConfig &getConfig(CConfig &config);
   typedef crl::gui::CCanvasBase Canvas;

   CAlgorithm(crl::CConfig &cfg, const std::string iter = "ITER");
   ~CAlgorithm();

   void setCanvas(crl::gui::CCanvasBase *c) { canvas = c; }

   virtual std::string getVersion(void) = 0;
   virtual std::string getRevision(void) = 0;

   /// ----------------------------------------------------------------------------
   /// @brief solve it will call particular iteration or invoke batch
   ///        in single iteration mode following methods are called
   ///          1. load
   ///          2. initialize
   ///          2a. after_init
   ///          3. iterate
   ///          3++. release - place to release resource allocated in initialize
   ///          4. save - called only if save-results is true
   ///          5. appendToLog
   ///
   ///        in batch mode following methods are called
   ///          1. load
   ///          2. initialize
   ///          2a. after_init
   ///           - for each requested iteration
   ///                3. iterate
   ///                4. save - called only if save-results is true
   ///                5. appendToLog
   ///          6. release - place to release resource allocated in initialize
   ///
   ///          these methods are defined in derived class, following methods
   ///          can be called,
   ///            defineResultLog   - define common columns in resultLog
   ///            fillResultRecord  - fill first common columns in resultLog
   ///            saveInfo          - save env setup and overall settings and
   ///            stats saveSettings      - save config settings saveCanvas
   ///            - handle canvas save to result-canvas-output into
   ///                                approriate outputiterpath
   ///                                particular visualization must be called in
   ///                                solve derived class
   /// ----------------------------------------------------------------------------
   virtual void solve(void);

 protected:
   virtual void batch(void);

   /// ----------------------------------------------------------------------------
   /// @brief load
   /// ----------------------------------------------------------------------------
   virtual void load(void) = 0;

   /// ----------------------------------------------------------------------------
   /// @brief initialize before first iteration, resp. before batch loop
   /// ----------------------------------------------------------------------------
   virtual void initialize(void) = 0;

   virtual void after_init(void) = 0;

   virtual void iterate(int iter) = 0;
   virtual void save(void) = 0;
   virtual void release(void) = 0;

   /// ----------------------------------------------------------------------------
   /// @brief defineResultLog virtual method to add and set result log,
   ///        eg. method column
   /// ----------------------------------------------------------------------------
   virtual void defineResultLog(void) = 0;

   /// ----------------------------------------------------------------------------
   /// @brief updateResultRecordTimes propage current timers values to the
   /// result
   ///                                log
   /// ----------------------------------------------------------------------------
   void updateResultRecordTimes(void);

   /// ----------------------------------------------------------------------------
   /// @brief appendToLog
   /// ----------------------------------------------------------------------------
   virtual void appendToLog(void);

   /// ----------------------------------------------------------------------------
   /// @brief saveCanvas
   /// ----------------------------------------------------------------------------
   virtual void saveCanvas(void);

   /// ----------------------------------------------------------------------------
   /// @brief saveInfo
   ///
   /// @param file
   /// @throw crl::io_error
   /// ----------------------------------------------------------------------------
   virtual void saveInfo(const std::string &file) throw();

   /// ----------------------------------------------------------------------------
   /// @brief saveSettings
   ///
   /// @param file
   /// @throw crl::io_error
   /// ----------------------------------------------------------------------------
   virtual void saveSettings(const std::string &file) throw();

   /// ----------------------------------------------------------------------------
   /// @brief getOutputPath
   ///
   /// @param filename
   ///
   /// @return
   /// ----------------------------------------------------------------------------
   std::string getOutputPath(const std::string filename, std::string &dir);

   /// ----------------------------------------------------------------------------
   /// @brief getOutputIterPath
   ///
   /// @param filename
   ///
   /// @return
   /// ----------------------------------------------------------------------------
   std::string getOutputIterPath(const std::string filename, std::string &dir);

 private:
   struct IterInterval {
      int from;
      int to;
      IterInterval(int f, int t) : from(f), to(t) {}
   };

   typedef std::vector<IterInterval> MissingIntervals;

   void loadResults(void);
   void getMissingIters(MissingIntervals &iters);

 protected:
   const std::string ITER;
   crl::gui::CCanvasBase *canvas;
   crl::CConfig &config;

   typedef crl::CTimerN Timer;

   int iter;
   int batchNbr;
   bool batchContinue;
   bool saveResults;
   bool saveVisual;
   bool verboseLog;
   std::string output;
   std::string citiesFilename;
   std::string resultCanvasOutput;
   std::string name;

   CResultLog resultLog;

   Timer tLoad;
   Timer tInit;
   Timer tSolve;
   Timer tSave;
   Timer tRelease;
};

} // end namespace crl

#endif

/* end of algorithm.h */
