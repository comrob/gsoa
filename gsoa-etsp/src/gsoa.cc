/*
 * File name: gsoa.cc
 * Date:      2016/12/07 08:32
 * Author:    Jan Faigl
 */

#include <limits>

#include <boost/foreach.hpp>

#include <crl/random.h>
#include <crl/logging.h>
#include <crl/assert.h>
#include <crl/file_utils.h>

#include <crl/gui/shape.h>
#include <crl/gui/shapes.h>

#include "gsoa.h"

#include "gsoa_learning.h"

#include "route_path_utils.h"

#include "canvasview_coords.h"
#include "canvasview_gsoa.h"

#define foreach BOOST_FOREACH

using namespace gsoa;

typedef std::vector<int> IntVector;

using namespace crl;
using namespace crl::gui;

/// ----------------------------------------------------------------------------
static void createPermutation(int number, IntVector &permutation) 
{
   permutation.clear();
   for (int i = 0; i < number; i++) {
      permutation.push_back(i);
   }
}

/// ----------------------------------------------------------------------------
static void permute(IntVector &permutation) 
{
   int k, tmp;
   for (int i = permutation.size(); i > 0; --i) {
      k = crl::CRandom::random() % i;
      tmp = permutation[i - 1];
      permutation[i - 1] = permutation[k];
      permutation[k] = tmp;
   }
}

/// - static method ------------------------------------------------------------
crl::CConfig& CGSOA::getConfig(crl::CConfig &config) 
{
   // basic properties not included in the crl_algorithm
   Base::getConfig(config);
   config.add<bool>("save-info", "disable/enable save info", true);
   config.add<bool>("save-settings", "disable/enable save settings", true);

   config.add<std::string>("result-path", "file name for the final found path (ring) as sequence of points",
         "path");
   config.add<std::string>("pic-dir", "relative directory in result directory to store pictures from each iteration");
   config.add<std::string>("pic-ext",
         "extension of pic, eps, png, pdf, svg (supported by particular gui renderer rendered",
         "png");
   config.add<bool>("save-pic", "enable/disable saving pictures (after each refine)");
   // 
   // GSOA - learning paramters 
   Schema::getConfig(config);
   config.add<bool>("2opt-post", "enable 2opt postprocessing of the found path", false);
   //
   // Problem specification
   config.add<std::string>("problem", "Problem file");
   config.add<std::string>("method", "Specify method in the result log", "gsoa");
   config.add<std::string>("ring-init-coords", "Filename with coords to initialize the ring of nodes. If given ring is initialized using the coords in the file", "");
   //
   // Info reporting
   config.add<double>("ref-sol-len", "Specify reference solution const for reporting relative length RLENGTH in the results log (must be > 0)", 0.0); 
   //
   // Gui properties
   config.add<std::string>("draw-shape-targets", "Shape of the target", Shape::CITY());
   config.add<std::string>("draw-shape-neurons", "Shape of the neurons", Shape::NEURON());
   config.add<std::string>("draw-shape-path", "Shape of the path", Shape::RED_LINE());
   config.add<std::string>("draw-shape-ring", "Shape of the ring", Shape::GREEN_BOLD_LINE());
   config.add<std::string>("draw-shape-path-nodes", "Shape of the path nodes", Shape::MAP_VERTEX());
   config.add<bool>("draw-ring-iter", "enable/disable drawing ring at each iteration", false);
   config.add<bool>("draw-ring", "Enable/Disable drawing ring in the final shoot", true);
   config.add<bool>("draw-path", "Enable/Disable drawing ring in the final shoot", true);
   config.add<bool>("draw-path-nodes", "enable/disable drawing path vertices(nodes)", true);
   return config;
}

/// - constructor --------------------------------------------------------------
CGSOA::CGSOA(crl::CConfig &config) : Base(config, "TRIAL"),
   SAVE_RESULTS(config.get<bool>("save-results")),
   SAVE_SETTINGS(config.get<bool>("save-settings")),
   SAVE_INFO(config.get<bool>("save-info")),
   DRAW_RING_ITER(config.get<bool>("draw-ring-iter")),
   DRAW_RING_ENABLE(config.get<bool>("draw-ring")),
   SAVE_PIC(config.get<bool>("save-pic")),
   REF_SOL_LEN(config.get<double>("ref-sol-len"))
{
   shapeTargets.setShape(config.get<std::string>("draw-shape-targets"));
   shapeNeurons.setShape(config.get<std::string>("draw-shape-neurons"));
   shapePath.setShape(config.get<std::string>("draw-shape-path"));
   shapeRing.setShape(config.get<std::string>("draw-shape-ring"));
   shapePathNodes.setShape(config.get<std::string>("draw-shape-path-nodes"));
   method = config.get<std::string>("method");
   if (!schema) {
      schema = new Schema(config);
   }
   const std::string fname = config.get<std::string>("problem");
   { // load problem
      std::ifstream in(fname);
      Coords pt;
      while (in >> pt) {
	 targets.push_back(new STarget(targets.size(), pt));
      }
   }
   if (name.size() == 0) {
      std::string n = getBasename(fname);
      size_t i = n.rfind(".txt");
      if (i != std::string::npos) {
	 name = n.erase(i, 4);
      }
   }
   ring = new CRing(targets);

   const std::string finit = config.get<std::string>("ring-init-coords");
   if (not finit.empty()) {
      assert_io(crl::checkPath(finit) && crl::isFile(finit), "Cannot open file " + finit + " to read ring-init-coords");
      std::ifstream in(finit);
      Coords pt;
      while (in >> pt) {
	 initCoords.push_back(pt);
      }
      INFO("Read " << initCoords.size() << " coords from \"" + finit + "\" as ring-init-coords");
      initRingBasename = crl::getBasename(finit);
   }
}

/// - destructor ---------------------------------------------------------------
CGSOA::~CGSOA()
{
   delete ring;
   foreach(STarget *target, targets) {
      delete target;
   }
}

/// - public method ------------------------------------------------------------
std::string CGSOA::getVersion(void) 
{
   return "GSOA ETSP 1.0";
}

/// - public method ------------------------------------------------------------
std::string CGSOA::getRevision(void) 
{
   return "$Id: gsoa.cc 3500 2023-05-15 19:54:24Z jf $";
}

/// - public method ------------------------------------------------------------
void CGSOA::solve(void) 
{
   crl::CRandom::randomize();
   Base::solve();
}

/// - protected method ---------------------------------------------------------
void CGSOA::load(void)
{
   if (canvas) {
      *canvas 
         << canvas::AREA << targets << canvas::END
         << "targets" << shapeTargets << canvas::POINT 
         << targets
         << canvas::REDRAW
         ;
   }
   DEBUG("GSOA::load -- done");
}

/// - protected method ---------------------------------------------------------
void CGSOA::initialize(void)
{
   permutation.clear();
   foreach(const STarget *target, targets) {
      permutation.push_back(target->label);
   }
}

/// - protected method ---------------------------------------------------------
void CGSOA::after_init(void) 
{
   //  tLoad.append(loadTimer);
   //  tInit.append(initTimer);
}

/// - protected method ---------------------------------------------------------
void CGSOA::iterate(int iter) 
{
   crl::CRandom::randomize();
   finalSolution.clear();
   int finalBestSolutionStep;
   TargetPtrVector allTargets; 
   if (initCoords.empty()) {
      ring->initialize_neurons(targets[0]->coords);
   } else {
      ring->initialize_neurons(initCoords);
   }
   foreach(STarget *target, targets) {
      target->selectedWinner = 0;
      target->stepWinnerSelected = -1;
   }
   schema->G = schema->INIT_LEARNING_GAIN;
   schema->mi = config.get<double>("learning-rate");
   const double MAX_ERROR = config.get<double>("termination-error");
   const int MAX_STEPS = config.get<int>("termination-max-steps");
   const bool TERM_CHANGE = config.get<bool>("termination-change");

   if (canvas) {
      *canvas << canvas::CLEAR << "path" << "path";
   }
   double error = 2 * MAX_ERROR;
   int step = 0;
   CoordsVector solution;
   CoordsVector bestSolution;
   int bestSolutionStep = -1;
   double bestSolutionLength = std::numeric_limits<double>::max();
   const bool BEST_SOLUTION = config.get<bool>("best-solution");
   bool term = false;
   IntVector routes[2];
   int routeCur = 0;
   int routePrev = 1;
   while (!((error < MAX_ERROR)) && (step < MAX_STEPS) && not term) { //perform adaptation step
      error = refine(step, error);
      DEBUG("Step: " << step << " G: " << schema->G);
      if (BEST_SOLUTION) {
         getSolution(step, solution); //collect solution
         const double len = get_path_length(solution);
         if (len < bestSolutionLength) {
            bestSolution = solution;
            bestSolutionStep = step;
            bestSolutionLength = len;
         }
      }
      if (TERM_CHANGE) {
         ring->get_ring_route(step, routes[routeCur]);
         if (routes[routeCur].size() == routes[routePrev].size()) {
            term = true;
            for (int i = 0; i < routes[routeCur].size(); ++i) {
               if (routes[routeCur][i] != routes[routePrev][i]) {
                  term = false;
                  break;
               }
            }
         }
         routePrev = (routePrev + 1)%2;
         routeCur = (routeCur + 1)%2;
      }
      schema->G = schema->G * (1 - schema->GAIN_DECREASING_RATE * (step + 1));
      step++;
   } //end step loop
   tSolve.stop();

   double length;
   if (BEST_SOLUTION) {
      finalSolution = bestSolution;
      length = bestSolutionLength;
      finalBestSolutionStep = bestSolutionStep;
   } else {
      getSolution(step - 1, finalSolution); //collect solution
      length = get_path_length(finalSolution);
      finalBestSolutionStep = step - 1;
   }

   if (config.get<bool>("2opt-post")) {
      two_opt(finalSolution);
      double twoOptLength = get_path_length(finalSolution);
      DEBUG("Length: " << length << " after 2opt: " << twoOptLength);
      length = twoOptLength;
   }
   fillResultRecord(iter);
   resultLog
      << length // 
      << step
      << finalBestSolutionStep
      << (initCoords.empty() ? "NONE" : initRingBasename)
      << schema->INIT_LEARNING_GAIN;
   if (REF_SOL_LEN) {
      resultLog << (length / REF_SOL_LEN);
   }
   resultLog << crl::result::endrec;
   DEBUG("Best solution with the length: " << bestSolutionLength << " found in: " << bestSolutionStep << " steps");
}

/// - protected method ---------------------------------------------------------
double CGSOA::refine(int step, double errorMax)
{
   double errorToGoal = errorMax;
   double error = 0.0;
   permute(permutation);
   schema->updateExp(targets.size(), step);
   for (IntVector::iterator i = permutation.begin(); i != permutation.end(); i++) {
      STarget *target = targets[*i];
      SNeuron *prevWinner = target->stepWinnerSelected == step - 1 ? target->selectedWinner : 0;
      int r = 0;
      SWinnerSelection* winner = ring->selectWinner(step, target, errorToGoal);
      if (winner and winner->hasWinner) {
         ring->adapt(step); 
         if (error < errorToGoal) {
            error = errorToGoal; //update error
         }
      }
   } //end permutation of all targets
   ring->regenerate(step);
   if (canvas and DRAW_RING_ITER) {
      drawRing(step);
      if (SAVE_PIC) {
         savePic(step, true);
      }
   }
   return error; // return largest error to city
}

/// - protected method ---------------------------------------------------------
void CGSOA::save(void) 
{
   std::string dir;
   updateResultRecordTimes(); //update timers as load and initilization is outside class
   if (SAVE_SETTINGS) {
      saveSettings(getOutputIterPath(config.get<std::string>("settings"), dir));
   }
   if (SAVE_INFO) {
      saveInfo(getOutputIterPath(config.get<std::string>("info"), dir));
   }
   if (SAVE_RESULTS) {
      std::string file = getOutputIterPath(config.get<std::string>("result-path"), dir);
      assert_io(createDirectory(dir), "Cannot create file in the path'" + file + "'");

      const int i = 0;
      std::stringstream ss;
      ss << file << "-" << std::setw(2) << std::setfill('0') << i << ".txt";
      std::ofstream ofs(ss.str());
      assert_io(not ofs.fail(), "Cannot create path '" + ss.str() + "'");
      ofs << std::setprecision(14);
      foreach(const Coords &pt, finalSolution) {
         ofs << pt.x << " " << pt.y << std::endl;
      }
      assert_io(not ofs.fail(), "Error occur during path saving");
      ofs.close();
   }
   if (canvas) { // map must be set
      *canvas << canvas::CLEAR << "ring";
      if (config.get<bool>("draw-path")) {
         drawPath();
      } else if (DRAW_RING_ENABLE) {
         drawRing(-1);
      }
      saveCanvas();
   }
}

/// - protected method ---------------------------------------------------------
void CGSOA::release(void) 
{
}

/// - protected method ---------------------------------------------------------
void CGSOA::defineResultLog(void) 
{
   static bool resultLogInitialized = false;
   if (!resultLogInitialized) {
      resultLog << result::newcol << "NAME";
      resultLog << result::newcol << "METHOD";
      resultLog << result::newcol << "TRIAL";
      resultLog << result::newcol << "RTIME";
      resultLog << result::newcol << "CTIME";
      resultLog << result::newcol << "UTIME";
      resultLog << result::newcol << "LENGTH"; 
      resultLog << result::newcol << "STEPS";
      resultLog << result::newcol << "SOLUTION_STEP";
      resultLog << result::newcol << "INIT_RING";
      resultLog << result::newcol << "INIT_GAIN";
      if (REF_SOL_LEN > 0) {
	 resultLog << result::newcol << "RLENGTH";
      }
      resultLogInitialized = true;
   }
}

/// - protected method ---------------------------------------------------------
void CGSOA::fillResultRecord(int trial) 
{
   resultLog << result::newrec << name << method << trial;
   long t[3] = {0l, 0l, 0l};
   tLoad.addTime(t);
   tInit.addTime(t);
   tSolve.addTime(t);
   tSave.addTime(t);
   resultLog << t[0] << t[1] << t[2];
}

/// - private method -----------------------------------------------------------
void CGSOA::drawPath(void)
{
   if (canvas) {
      *canvas
         << canvas::CLEAR << "path" << "path"
         << CShape(config.get<std::string>("draw-shape-path"))
         << canvas::LINESTRING
         << finalSolution
         << finalSolution.front()
         << canvas::END;

      if (config.get<bool>("draw-path-nodes")) {
         *canvas << canvas::POINT << shapePathNodes << finalSolution;
      } //end draw-path-nodes
   } //end if canvas
}

/// - private method -----------------------------------------------------------
void CGSOA::drawRing(int step)
{
   if (canvas) {
      *canvas << canvas::CLEAR << "ring" << "ring";
      if (DRAW_RING_ENABLE) {
         *canvas 
            << canvas::LINESTRING << shapeRing
            << ring << ring->begin()
            << canvas::END;
      } //end ring
   } //end canvas
}

/// - private method -----------------------------------------------------------
void CGSOA::savePic(int step, bool detail, const std::string &dir_suffix)
{
   static int lastStep = step;
   static int i = 0;
   if (lastStep != step) {
      i = 0;
   }
   if (canvas) {
      canvas->redraw();
      std::string dir;
      std::string file = getOutputIterPath(config.get<std::string>("pic-dir") + dir_suffix, dir);
      assert_io(createDirectory(file), "Cannot create file in path '" + file + "'");
      std::stringstream ss;
      ss << file << "/" << "iter-" << std::setw(3) << std::setfill('0') << step;
      ss << "-" << std::setw(4) << std::setfill('0') << i;

      std::string suffixes(config.get<std::string>("pic-ext"));
      if (!suffixes.empty()) {
         std::string::size_type cur = 0;
         std::string::size_type next;
         do {
            next = suffixes.find(',', cur);
            const std::string &ext = suffixes.substr(cur, next - cur);
            if (!ext.empty()) {
               assert_io(canvas->save(ss.str() + "." + ext), "Cannot create output canvas file '" + file + "'");
            }
            cur = next + 1;
         } while (next != std::string::npos);
      } else {
         ss << "." << config.get<std::string>("pic-ext");
         assert_io(canvas->save(ss.str()), "Cannot create output canvas file '" + ss.str() + "'");
      }
   }
   lastStep = step;
   i++;
}

/// - private method -----------------------------------------------------------
void CGSOA::getSolution(int step, CoordsVector &solution) const
{
   ring->get_ring_path(step, solution);
}

/* end of gsoa.cc */
