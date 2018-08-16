/*
 * File name: algorithm_base.cc
 * Date:      2010/03/30 18:18
 * Author:    Jan Faigl
 */

#include <crl/file_utils.h>
#include <crl/logging.h>
#include <crl/timerN.h>

#include <crl/alg/algorithm_base.h>

using namespace crl::alg;

/// ----------------------------------------------------------------------------
/// @brief addTime
///
/// @param long[]
/// @param
/// @param timer
/// ----------------------------------------------------------------------------
void addTime(long (&t)[3], crl::CTimerN &timer)
{
   t[0] += timer.realTime();
   t[1] += timer.cpuTime();
   t[2] += timer.userTime();
}

/// ----------------------------------------------------------------------------
/// Class CAlgorithmBase

/// - static method ------------------------------------------------------------
crl::CConfig &CAlgorithmBase::getConfig(CConfig &config)
{
   crl::CAlgorithm::getConfig(config);
   config.add<std::string>("method", "method of the problem", "");
   config.add<std::string>("id", "id of the problem", "");
   return config;
}

/// ----------------------------------------------------------------------------
CAlgorithmBase::CAlgorithmBase(crl::CConfig &config)
    : crl::CAlgorithm(config, "TRIAL")
{
}

/// ----------------------------------------------------------------------------
CAlgorithmBase::~CAlgorithmBase() {}

/// ----------------------------------------------------------------------------
std::string CAlgorithmBase::getVersion(void) { return "CAlgorithmBase 0.1"; }

/// ----------------------------------------------------------------------------
std::string CAlgorithmBase::getRevision(void)
{
   return "$Id: algorithm_base.cc 1776 2010-03-30 16:50:29Z honza $";
}

/// - protected method ---------------------------------------------------------
void CAlgorithmBase::load(void)
{
   // to be done
}

/// - protected method ---------------------------------------------------------
void CAlgorithmBase::initialize(void)
{
   // to be done
}

/// - protected method ---------------------------------------------------------
void CAlgorithmBase::iterate(int iter)
{
   /* an example
   tSolve.stop();
   fillResultRecord(iter, 100.0 - getPercent(uncoveredArea, freespaceArea),
   100.0 - getPercent(distUncoveredArea, freespaceArea)); if (canvas) { *canvas
   << canvas::CLEAR << "coverage" << "coverage"; *canvas << canvas::DEPTH << 50;
      CShape
   shape("pen_color:black;fill_color:lightyellow3;pen_alpha:1;fill_alpha:1;width:1;radius:1;zoom:0;fill:1;outline:1;depth:50;");
      CShape
   shape2("pen_color:black;fill_color:white;pen_alpha:1;fill_alpha:1;width:1;radius:1;zoom:0;fill:1;outline:1;depth:45;");
      *canvas << canvas::POINT << Shape::CITY << FillColor("skyblue1") <<
   Depth(40) << sensors << Shape::MAP_VERTEX << FillColor("red") << Depth(35) <<
   newSensors;
   }
   */
}

/// - protected method ---------------------------------------------------------
void CAlgorithmBase::save(void)
{
   /* an example
      std::string dir;
      tLoad = loadTimer; //update timers as load and initilization is outside
      class tInit = initTimer; //
      saveInfo(getOutputIterPath(config.get<std::string>("info"), dir));
      saveSettings(getOutputIterPath(config.get<std::string>("settings"), dir));

      if (saveResults) {
         std::string file =
      getOutputIterPath(config.get<std::string>("sensors"), dir);
         assert_io(createDirectory(dir), "Can not create file in path'" + file +
      "'"); std::ofstream ofs(file.c_str()); foreach(const Coords& pt,
      newSensors) { ofs << pt.x << " " << pt.y << std::endl;
         }
         assert_io(ofs, "Error occur during route saving");
         ofs.close();
      }
      saveCanvas();
      */
}

/// - protected method ---------------------------------------------------------
void CAlgorithmBase::release(void)
{
   // to be done
}

/// - protected method ---------------------------------------------------------
void CAlgorithmBase::defineResultLog(void)
{
   /* an example
   static bool resultLogInitialized = false;
   if (!resultLogInitialized) {
      resultLog << result::newcol << "NAME"
         << result::newcol << "METHOD"
         << result::newcol << "VIS"
         << result::newcol << "ITER"
         << result::newcol << "RTIME"
         << result::newcol << "CTIME"
         << result::newcol << "UTIME"
         << result::newcol << "ID"
         << result::newcol << "ERROR_VARIANCE"
         << result::newcol << "REPEATABILITY_VARIANCE"
         << result::newcol << "VIS_RED"
         << result::newcol << "COVERAGE"
         << result::newcol << "DISTURBANCE_COVERAGE"
         << result::newcol << "COVERAGE_CHANGE";
      resultLogInitialized = true;
   }
   */
}

/// - protected method ---------------------------------------------------------
void CAlgorithmBase::fillResultRecord(int iter, double coverage,
                                      double distCoverage)
{
   /* an example
   long t[3] = {0, 0, 0};
   addTime(t, tLoad); //add all times to result record
   addTime(t, tInit);
   addTime(t, tSolve);
   addTime(t, tSave);
   resultLog << result::newrec
      << config.get<std::string>("name") << config.get<std::string>("method")
      << visibility << iter
      << t[0] << t[1] << t[2]
      << config.get<std::string>("id")
      << posVar << repVar << visRed
      << coverage << distCoverage << getPercent(distCoverage, coverage);
   resultLog << result::endrec;
   */
}

/* end of algorithm_base.cc */
