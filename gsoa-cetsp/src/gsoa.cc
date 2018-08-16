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
   // crl::CRandom::randomize();
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
   // basic properties not included in the crl algorithm
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
   config.add<double>("radius-decrease", "Distance to decreased the radius to ensure the waypoint is in the neighbourhood", 0.1);
   config.add<bool>("in-neigh-waypoint", "enable/disable determining in neighborhood waypoint", false);
   //
   // Problem specification
   config.add<std::string>("problem", "Problem file");
   config.add<int>("depot-idx", "If >= 0, the particular goal is considered as the depot with the radius 0", -1);
   config.add<bool>("variable-radius", "If enabled, the input file is considered as x y radius ", false);
   config.add<double>("communication-radius", "Radius within other sensors can be read, disabled if <= 0", 0);
   config.add<std::string>("method", "Specify method in the result log", "gsoa");
   //
   // Gui properties
   config.add<std::string>("draw-shape-targets", "Shape of the target", Shape::CITY());
   config.add<std::string>("draw-shape-neurons", "Shape of the neurons", Shape::NEURON());

   config.add<std::string>("draw-shape-path", "Shape of the path", Shape::RED_LINE());
   config.add<std::string>("draw-shape-ring", "Shape of the ring", Shape::GREEN_BOLD_LINE());
   config.add<std::string>("draw-shape-communication-radius", "Shape of the communication radius for highlight coverage", Shape::POLYGON_FILL());
   config.add<std::string>("draw-shape-path-nodes", "Shape of the path nodes", Shape::MAP_VERTEX());
   config.add<std::string>("draw-shape-depot", "Shape of the depot node", Shape::DEPOT());
   config.add<std::string>("draw-shape-tour-represented-by-ring", "Shape of the tour represented by the ring", Shape::RED_LINE());

   config.add<bool>("draw-ring-iter", "enable/disable drawing ring at each iteration", false);
   config.add<bool>("draw-ring", "Enable/Disable drawing ring in the final shoot", true);
   config.add<bool>("draw-ring-nodes", "Enable/disable drawing ring nodes", true);
   config.add<bool>("draw-tour-represented-by-ring", "Enable/disable drawing tour represented by the ring", false);
   config.add<bool>("draw-path", "Enable/Disable drawing ring in the final shoot", true);
   config.add<bool>("draw-path-nodes", "enable/disable drawing path vertices(nodes)", true);
   config.add<bool>("draw-communication-radius", "enable/disable drawing radius of the selected targets", false);
   config.add<bool>("draw-depot", "enable/disable drawing depot", false);
   config.add<bool>("disable-area-radius", "enable/disable computing area from the radius", false);
   config.add<double>("add-x", "add the value to the coords for computing the canvas area", 0.0);
   config.add<double>("add-y", "add the value to the coords for computing the canvas area", 0.0);
   return config;
}

/// - constructor --------------------------------------------------------------
CGSOA::CGSOA(crl::CConfig &config) : Base(config, "TRIAL"),
   DEPOT_IDX(config.get<int>("depot-idx")),
   VARIABLE_RADIUS(config.get<bool>("variable-radius")),
   SAVE_RESULTS(config.get<bool>("save-results")),
   SAVE_SETTINGS(config.get<bool>("save-settings")),
   SAVE_INFO(config.get<bool>("save-info")),
   DRAW_RING_ITER(config.get<bool>("draw-ring-iter")),
   DRAW_RING_ENABLE(config.get<bool>("draw-ring")),
   DRAW_RING_NODES(config.get<bool>("draw-ring-nodes")),
   DRAW_TOUR_REPRESENTED_BY_RING(config.get<bool>("draw-tour-represented-by-ring")),
   SAVE_PIC(config.get<bool>("save-pic")),
   IN_NEIGH_WAYPOINT(config.get<bool>("in-neigh-waypoint"))
{
   shapeTargets.setShape(config.get<std::string>("draw-shape-targets"));
   shapeNeurons.setShape(config.get<std::string>("draw-shape-neurons"));
   shapePath.setShape(config.get<std::string>("draw-shape-path"));
   shapeRing.setShape(config.get<std::string>("draw-shape-ring"));
   shapeCommRadius.setShape(config.get<std::string>("draw-shape-communication-radius"));
   shapePathNodes.setShape(config.get<std::string>("draw-shape-path-nodes"));
   shapeDepot.setShape(config.get<std::string>("draw-shape-depot"));
   shapeTourRepresentedByRing.setShape(config.get<std::string>("draw-shape-tour-represented-by-ring"));

   method = config.get<std::string>("method");

   if (!schema) {
      schema = new Schema(config);
   }

   const std::string fname = config.get<std::string>("problem");
   if (VARIABLE_RADIUS) { // load problem
      std::ifstream in(fname);
      Coords pt;
      double r;
      while (in >> pt >> r) {
         targets.push_back(new STarget(targets.size(), pt, DEPOT_IDX >= 0 && targets.size() == DEPOT_IDX ? 0.0 : r));
      }
   } else { // load problem
      std::ifstream in(fname);
      Coords pt;
      while (in >> pt) {
         targets.push_back(new STarget(targets.size(), pt, (DEPOT_IDX >= 0 && targets.size() == DEPOT_IDX) ? 0.0 : schema->COMM_RADIUS));
      }
   }
   if (name.size() == 0) {
      std::string n = getBasename(fname);
      size_t i = n.rfind(".txt");
      if (i != std::string::npos) {
         name = n.erase(i, 4);
      }
   }
   ring = new CRing(targets, config.get<double>("radius-decrease"), IN_NEIGH_WAYPOINT);
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
   return "GSOA CETSP 1.0";
}


/// - public method ------------------------------------------------------------
std::string CGSOA::getRevision(void) 
{
   return "$Id: gsoa.cc 234 2018-08-16 20:09:49Z jf $";
}

/// - public method ------------------------------------------------------------
void CGSOA::visualize(const CoordsVector & path)
{
   load();
   finalSolution = path;
   drawPath();
   saveCanvas();
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
      Coords min(std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
      Coords max(-std::numeric_limits<double>::max(), -std::numeric_limits<double>::max());
      if (config.get<bool>("disable-area-radius")) {
         const double x = config.get<double>("add-x");
         const double y = config.get<double>("add-y");
         foreach(const STarget * t, targets) {
            if (min.x > t->coords.x - x) { min.x = t->coords.x - x; }
            if (max.x < t->coords.x + x) { max.x = t->coords.x + x; }
            if (min.y > t->coords.y - y) { min.y = t->coords.y - y; }
            if (max.y < t->coords.y + y) { max.y = t->coords.y + y; }
         }
      } else {
         foreach(const STarget * t, targets) {
            if (min.x > t->coords.x - t->radius) { min.x = t->coords.x - t->radius; }
            if (max.x < t->coords.x + t->radius) { max.x = t->coords.x + t->radius; }
            if (min.y > t->coords.y - t->radius) { min.y = t->coords.y - t->radius; }
            if (max.y < t->coords.y + t->radius) { max.y = t->coords.y + t->radius; }
         }
      }
      *canvas 
         << canvas::AREA << min << max << canvas::END
         << "targets" << shapeTargets << canvas::POINT;
      if (config.get<bool>("draw-depot") and DEPOT_IDX >= 0 and DEPOT_IDX < targets.size())  {
         foreach(const STarget *target, targets) {
            if (target->label != DEPOT_IDX) {
               *canvas << target->coords;
            }
         }
         *canvas << shapeDepot << targets[DEPOT_IDX]->coords;
      } else {
         *canvas << targets;
      }
      if (config.get<bool>("draw-communication-radius")) {
         *canvas << shapeCommRadius << canvas::ARC;
         foreach(const STarget *target, targets) {
            if (target->radius > 0.0) {
               *canvas << target->coords.x << target->coords.y << target->radius << 0.0 << 2 * M_PI;
            }
         }
      } //end draw-communication-radius
      *canvas << canvas::REDRAW;
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

   ring->initialize_neurons(targets[0]->coords);
   foreach(STarget *target, targets) {
      target->selectedWinner = 0;
      target->stepWinnerSelected = -1;
   }
   schema->G = 10;
   schema->mi = config.get<double>("learning-rate");
   //thresholds for the termination conditions
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
      DEBUG("Step: " << step << " G: " << schema->G << " error: " << error);
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
      << crl::result::endrec;
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
      if (DRAW_RING_NODES) {
         *canvas << canvas::POINT << shapeNeurons << ring;
      }
      if (DRAW_TOUR_REPRESENTED_BY_RING) {
	 CoordsVector path;
	 double r;
	 ring->get_ring_path(step, path); 
	 if (path.size() > 1) {
	    *canvas
	       << shapeTourRepresentedByRing << canvas::LINESTRING
	       << path;
	    *canvas << canvas::END;
	 }
      }
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
