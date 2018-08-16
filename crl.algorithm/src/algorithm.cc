/*
 * File name: crl_algorithm.cc
 * Date:      2010/03/25 15:21
 * Author:    Jan Faigl
 */

#include <sstream>
#include <vector>

#include <sys/utsname.h>

#include <crl/file_utils.h>
#include <crl/logging.h>

#include <crl/alg/text_result_log.h>

#include <crl/alg/algorithm.h>

using namespace crl;

/// ----------------------------------------------------------------------------
/// Class CAlgorithm

/// - static method ------------------------------------------------------------
crl::CConfig &CAlgorithm::getConfig(CConfig &config)
{
   config.add<std::string>(
       "output", "output directory to store particular results and outputs",
       "./");
   config.add<std::string>(
       "results", "result log file, it will be placed in output directory",
       "results.log"); // TODO .db for sqlite, .txt text logs
   config.add<std::string>(
       "info",
       "information file, it will be placed in particular experiment directory",
       "info.txt");
   config.add<std::string>(
       "settings",
       "store configurations in boost::program_options config file format ",
       "settings.txt");
   config.add<std::string>(
       "result-canvas-output",
       "file name for final canvas output (eps,png,pdf,svg) are supported");
   config.add<std::string>("result-canvas-suffixes",
                           "comman separated list of exptensis e.g. png,pdf.  "
                           "if specified  several result images are created, "
                           "with particular suffix to the resultCanvasOutput");
   config.add<std::string>("name", "name used in result log as user "
                                   "identification if not set a default values "
                                   "(cities) is used");
   config.add<int>("iteration",
                   "set particular interation, otherwise all interations "
                   "(batch) are performed",
                   -1);
   config.add<int>("batch",
                   "number of iterations from 0 to batch (not included) ", -1);
   config.add<bool>("continue",
                    "in batch mode, partial results are loaded and checked, "
                    "only missing iterations are computed ",
                    false);
   config.add<bool>("save-results",
                    "disable/enable save results,configs and so on", true);
   config.add<bool>("save-visual",
                    "disable/enable save visualization results, canvas be",
                    true);
   config.add<bool>("verbose-result-log",
                    "disable/enable printing results log into logger", false);
   return config;
}

/// ----------------------------------------------------------------------------
CAlgorithm::CAlgorithm(crl::CConfig &cfg, const std::string iterI)
    : ITER(iterI), canvas(0), config(cfg)
{
   iter = config.get<int>("iteration");
   batchNbr = config.get<int>("batch");
   batchContinue = config.get<bool>("continue");
   saveResults = config.get<bool>("save-results");
   output = config.get<std::string>("output");
   name = config.get<std::string>("name");
   resultCanvasOutput = config.get<std::string>("result-canvas-output");
   saveVisual = config.get<bool>("save-visual");
   verboseLog = config.get<bool>("verbose-result-log");
}

/// ----------------------------------------------------------------------------
CAlgorithm::~CAlgorithm() {}

/// ----------------------------------------------------------------------------
void CAlgorithm::solve(void)
{
   defineResultLog();
   if (batchNbr != -1) {
      DEBUG("Batch mode enabled, required number of iterations is "
            << batchNbr);
      batch();
   } else {
      DEBUG("Single iteration enabled " << iter);
      tLoad.reset().start();
      load();
      tLoad.stop();
      tInit.reset().start();
      initialize();
      tInit.stop();
      after_init();
      tSolve.reset().start();
      iterate(iter);
      tSolve.stop();
      tRelease.reset().start();
      release();
      tRelease.stop();
      tSave.reset().start();
      save();
      tSave.stop();
      appendToLog();
   }
}

/// - protected method ---------------------------------------------------------
void CAlgorithm::batch(void)
{
   tLoad.reset().start();
   load();
   tLoad.stop();
   tInit.reset().start();
   initialize();
   tInit.stop();
   after_init();

   if (batchContinue) {
      MissingIntervals iters;
      loadResults();
      getMissingIters(iters);
      for (MissingIntervals::iterator it = iters.begin(); it != iters.end();
           it++) {
         DEBUG("MissingInterval <" << it->from << ", " << it->to << ")");
         for (iter = it->from; iter < it->to; iter++) {
            tSolve.reset().start();
            iterate(iter);
            tSolve.stop();
            tSave.reset().start();
            save();
            tSave.stop();
            appendToLog();
         }
      } // end for loop
   } else {
      for (iter = (iter < 0 ? 0 : iter); iter < batchNbr; iter++) {
         tSolve.reset().start();
         iterate(iter);
         tSolve.stop();
         tSave.reset().start();
         save();
         tSave.stop();
         appendToLog();
      } // end for loop
   }
   tRelease.reset().start();
   release();
   tRelease.stop();
}

/// - protected method ---------------------------------------------------------
void CAlgorithm::updateResultRecordTimes(void)
{
   long t[3] = {0, 0, 0};
   tLoad.addTime(t);
   tInit.addTime(t);
   tSolve.addTime(t);
   tSave.addTime(t);

   resultLog << CResultLog::SetColumnValue("RTIME", crl::string_format(t[0]))
             << CResultLog::SetColumnValue("CTIME", crl::string_format(t[1]))
             << CResultLog::SetColumnValue("UTIME", crl::string_format(t[2]));
}

/// - protected method ---------------------------------------------------------
void CAlgorithm::appendToLog(void)
{ // append last record
   std::string dir;
   std::string log = getOutputPath(config.get<std::string>("results"), dir);
   assert_io(createDirectory(dir), "Can not create directory '" + dir + "'");
   std::fstream out(log.c_str(), std::ios::app | std::ios::out);
   CTextLog t(resultLog);
   t.printRecord(resultLog.last(), out);
   if (verboseLog) {
      std::stringstream str;
      t.printRecord(resultLog.last(), str, false);
      INFO("result log: " << str.str());
   }
   out.close();
   if (!resultLog) {
      WARN("Error in resultLog:" << resultLog.getError());
      resultLog.clear();
   }
}

/// - protected method ---------------------------------------------------------
void CAlgorithm::saveCanvas(void)
{
   if (canvas && saveVisual && !resultCanvasOutput.empty()) {
      std::string dir;
      std::string suffixes(config.get<std::string>("result-canvas-suffixes"));
      if (!suffixes.empty()) {
         std::string::size_type cur = 0;
         std::string::size_type next;
         do {
            next = suffixes.find(',', cur);
            const std::string &ext = suffixes.substr(cur, next - cur);
            if (!ext.empty()) {
               std::string file =
                   getOutputIterPath(resultCanvasOutput + "." + ext, dir);
               assert_io(createDirectory(dir),
                         "Can not create file in path'" + dir + "'");
               canvas->redraw();
               assert_io(canvas->save(file),
                         "Can create output canvas file '" + file + "'");
            }
            cur = next + 1;
         } while (next != std::string::npos);
      } else {
         std::string file = getOutputIterPath(resultCanvasOutput, dir);
         assert_io(createDirectory(dir),
                   "Can not create file in path'" + dir + "'");
         canvas->redraw();
         assert_io(canvas->save(file),
                   "Can create output canvas file '" + file + "'");
      }
   }
}

/// - protected method ---------------------------------------------------------
void CAlgorithm::saveInfo(const std::string &file) throw()
{
   std::ostream *out;
   std::ofstream *ofs = 0;
   std::stringstream *ss = 0;
   if (saveResults) {
      assert_io(createDirectory(getDirname(file)),
                "Can not create file in path'" + file + "'");
      ofs = new std::ofstream(file.c_str());
      out = ofs;
   } else {
      ss = new std::stringstream();
      out = ss;
   }
   *out << "# program information " << std::endl;
   *out << "VERSION: " << getVersion() << std::endl;
   *out << "REVISION: " << getRevision() << std::endl;
   struct utsname n;
   if (uname(&n) == 0) {
      *out << std::endl << "# host information " << std::endl;
      *out << "PLATFORM:  " << n.sysname << " " << n.release << std::endl;
      *out << "VERSION: " << n.version << " " << std::endl;
      *out << "HOST: " << n.nodename << std::endl;
      *out << "MACHINE: " << n.machine << std::endl;
   }
   {
      char data[26];
      const time_t seconds = time(NULL);
      *out << std::endl << "# time information " << std::endl;
      *out << "DATE: " << ctime_r(&seconds, &data[0]) << std::endl;
   }

   *out << "# detail performance log" << std::endl;
   *out << "LOAD_TIME_CPU: " << tLoad.cpuTime() << std::endl;
   *out << "LOAD_TIME_USR: " << tLoad.userTime() << std::endl;
   *out << "LOAD_TIME_SYS: " << tLoad.sysTime() << std::endl;
   *out << "INIT_TIME_CPU: " << tInit.cpuTime() << std::endl;
   *out << "INIT_TIME_USR: " << tInit.userTime() << std::endl;
   *out << "INIT_TIME_SYS: " << tInit.sysTime() << std::endl;
   *out << "SOLVE_TIME_CPU: " << tSolve.cpuTime() << std::endl;
   *out << "SOLVE_TIME_USR: " << tSolve.userTime() << std::endl;
   *out << "SOLVE_TIME_SYS: " << tSolve.sysTime() << std::endl;
   *out << "SAVE_TIME_CPU: " << tSave.cpuTime() << std::endl;
   *out << "SAVE_TIME_USR: " << tSave.userTime() << std::endl;
   *out << "SAVE_TIME_SYS: " << tSave.sysTime() << std::endl;
   if (batchNbr < 0) { // save release times only for non-batched mode
      *out << "RELEASE_TIME_CPU: " << tRelease.cpuTime() << std::endl;
      *out << "RELEASE_TIME_USR: " << tRelease.userTime() << std::endl;
      *out << "RELEASE_TIME_SYS: " << tRelease.sysTime() << std::endl;
   }
   *out << std::endl << "# log results" << std::endl;
   CResultLog::RecordIterator record = resultLog.last();
   if (record != resultLog.records_end()) {
      result::StringArray::iterator col = resultLog.column_names_begin();
      for (result::StringArray::iterator v = record->begin();
           v != record->end(); col++, v++) {
         *out << *col << ":" << *v << std::endl;
      } // end for loop
   }
   if (ofs) {
      assert_io(not ofs->fail(), "Can not save info file '" + file + "'");
      ofs->close();
      delete ofs;
   } else {
      INFO(ss->str());
      delete ss;
   }
}

/// - protected method ---------------------------------------------------------
void CAlgorithm::saveSettings(const std::string &file) throw()
{
   if (saveResults) {
      assert_io(createDirectory(getDirname(file)),
                "Can not create file in path'" + file + "'");
      std::ofstream out(file.c_str());
      config.print(out);
      if (out) {
         assert_io(not out.fail(), "Can not save info file '" + file + "'");
      }
      out.close();
   }
}

/// - protected method ---------------------------------------------------------
std::string CAlgorithm::getOutputPath(const std::string filename,
                                      std::string &dir)
{
   if (!output.empty()) {
      dir = (output[output.size() - 1] == '/') ? output : output + "/";
      return dir + filename;
   } else {
      dir = "./";
      return filename;
   }
}

/// - protected method ---------------------------------------------------------
std::string CAlgorithm::getOutputIterPath(const std::string filename,
                                          std::string &dir)
{
   std::string iter_sep = iter >= 0 ? crl::string_format<int>(iter) + "/" : "/";
   if (output.size() > 0) {
      dir = (output[output.size() - 1] == '/' and iter_sep[0] == '/')
                ? output + iter_sep
                : output + "/" + iter_sep;
      return dir + filename;
   } else {
      dir = iter_sep;
      return dir + filename;
   }
}

/// - private method -----------------------------------------------------------
void CAlgorithm::loadResults(void)
{
   std::string dir;
   DEBUG("loadResults");
   std::string f = getOutputPath(config.get<std::string>("results"), dir);
   if (isFile(f)) {
      std::ifstream in(f.c_str());
      CTextLog(resultLog).load(in);
   } else {
      WARN("Results log file '" << f << "' has not been found");
   }
}

/// - private method -----------------------------------------------------------
void CAlgorithm::getMissingIters(MissingIntervals &iters)
{
   int i = iter < 0 ? 0 : iter;
   int t;
   int c = resultLog.getColumnIndex(ITER);
   std::vector<int> saveIters;
   if (c != -1) {
      for (CResultLog::RecordIterator it = resultLog.records_begin();
           it != resultLog.records_end(); it++) {
         saveIters.push_back(resultLog.getColumnValueAs<int>(it, c));
      }
      std::sort(saveIters.begin(), saveIters.end());
      std::vector<int>::iterator cur = saveIters.begin();
      while (i < batchNbr && cur != saveIters.end()) {
         if (*cur > i) { /// s to e is missing
            t = *cur < batchNbr ? *cur : batchNbr;
            iters.push_back(IterInterval(i, t));
            i = *cur + 1;
	 } else {
            i = *cur + 1;
         }
         cur++;
      } // end while
      if (i < batchNbr) {
         iters.push_back(IterInterval(i, batchNbr));
      }
   } else {
      WARN("Can not get ITER column from resultLog");
   }
}

/* end of crl_algorithm.cc */
