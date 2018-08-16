/*
 * File name: gsoa_ring.cc
 * Date:      2016/12/09 21:47
 * Author:    Jan Faigl
 */

#include <limits>

#include <boost/foreach.hpp>

#include <crl/logging.h>

#include "simple_intersection.h"

#include "gsoa_neuron.h"
#include "gsoa_learning.h"
#include "gsoa_ring.h"

#define foreach BOOST_FOREACH

using namespace gsoa;

typedef geom::CIntersection<Coords> Intersection;
static const double NEURON_COORDS_IDENTITY2 = 1e-5*1e-5;

using crl::logger;

/// - constructor --------------------------------------------------------------
CRing::CRing(const TargetPtrVector &targets) : targets(targets)
{
   start = 0;
   m = 0;
}

/// - destructor ---------------------------------------------------------------
CRing::~CRing()
{
   deallocate_neurons();
}

/// - public method ------------------------------------------------------------
void CRing::initialize_neurons(const Coords &pt)
{
   deallocate_neurons();
   start = new SNeuron(pt);
   start->prev = start;
   start->next = start;
   m = 1;
}

/// - public method ------------------------------------------------------------
void CRing::deallocate_neurons(void)
{
   SNeuron *cur = start;
   for (int i = 0; i < m; ++i) {
      SNeuron *n = cur->next;
      delete cur;
      cur = n;
   }
   m = 0;
   start = 0;
}

/// - public method ------------------------------------------------------------
SNeuron* CRing::insertNeuron(SNeuron* cur, SNeuron* neuron)
{  // insert neuron after the cur
   neuron->prev = cur;
   neuron->next = cur->next;
   cur->next->prev = neuron;
   cur->next = neuron;
   m += 1;
   return neuron;
}

/// - public method ------------------------------------------------------------
void CRing::removeNeuron(SNeuron *neuron)
{
   neuron->prev->next = neuron->next;
   neuron->next->prev = neuron->prev;
   if (start == neuron) {
      start = neuron->next;
      if (m == 1) {
         start = 0;
      }
   }
   delete neuron;
   m -= 1;
}

/// - public method ------------------------------------------------------------
SWinnerSelection* CRing::selectWinner(int step, STarget* target, double &errorToGoal)
{
   double error = std::numeric_limits<double>::max();
   winner.hasWinner = false;
   winner.target = target;
   winner.preWinner = winner.winner= 0;
   winner.alternateGoal = target->coords;
   double bestLength = std::numeric_limits<double>::max();
   double bestRingAddition = 0.0;
   SNeuron *cur = start;
   for (int i = 0; i < m; ++i) {
      const double dist2 = Intersection::point_segment_squared_distance(target->coords, cur->coords, cur->next->coords, cur->s, cur->x);
      if (dist2 < bestLength) {
         if (cur->s == 's') { //new point inside the segment
            winner.newPoint = cur->x; winner.preWinner = cur; winner.winner = 0; //use new point
         } else {
            winner.preWinner = 0; winner.winner = cur->s == 'a' ?  cur : cur->next; //use winner
         }
         bestLength = error = dist2;
         winner.hasWinner = true;
         winner.alternateGoal = target->coords;
      } //end new winner
      cur = cur->next;
   } //end all neurons
   if (winner.hasWinner) {
      winner.targetOnTour = target->label;
      errorToGoal = sqrt(error);
   }
   return &winner;
}

/// - public method ------------------------------------------------------------
void CRing::adapt(const int step)
{
   ASSERT_ARGUMENT(winner.hasWinner, "Cannot adapt without winner");
   //Update the network 
   if (winner.winner) { // it is a regular winner
      SNeuron *wNeuron = winner.winner;
      if (winner.winner == start) { // it is the first neuron
         wNeuron = new SNeuron(winner.winner->coords);
         insertNeuron(start, wNeuron);
      } else if (winner.winner->isInhibited(step)) { //also create a new one
         if (winner.winner->targetOnTour == winner.targetOnTour) {
            // avoid replication of the winner as it has been already selected in this epoch
         } else {
            wNeuron = new SNeuron(winner.winner->coords);
            insertNeuron(winner.winner, wNeuron);
         }
      }
      winner.winner = wNeuron;
   } else {
      ASSERT_ARGUMENT(winner.preWinner, "newPoint needs preWinner");
      SNeuron *wNeuron = new SNeuron(winner.newPoint);
      insertNeuron(winner.preWinner, wNeuron);
      winner.winner = wNeuron;
   }
   winner.winner->alternateGoal = winner.alternateGoal;
   winner.winner->targetOnTour = winner.targetOnTour;
   winner.winner->targetOnTourStep = step;

   { // uninhibit previous winner associated to the target in the current learning epoch
      STarget *target = winner.target;
      if (target->stepWinnerSelected == step) {
         //target has been already associated in this epoch
         if (target->selectedWinner and target->selectedWinner != winner.winner) {
            //new winner has been selected, clear te previous association
            target->selectedWinner->targetOnTourStep = -1; //uninhibit the tour
            //TODO maybe we can consider to delete the neuron right in tne current epoch
         }
      }
      target->stepWinnerSelected = step;
      // target->selectedRing = this;
      target->selectedWinner = winner.winner;
   }

   SNeuron *neuron = winner.winner;
   SNeuron *neuronBackward = neuron->prev;
   SNeuron *neuronForward = neuron->next;
   double dd = 1.0;
   const int d = m / schema->NEIGHBORHOOD_FACTOR;
   const int t = d < (m / 2) ? d : m / 2;
   const int TO = t < schema->explen ? t : schema->explen; // up to MIN_GAIN 
   const Coords& target = neuron->alternateGoal;

   const SNeuron *ringStart = start;

   if (neuron->coords.squared_distance(target) > 0.0) {
      const double mi = schema->mi;
      neuron->adapt(target, mi);
      for (int i = 0; i < TO; i++) {
         const double b = schema->exps[i];
         if (neuronBackward != neuron) {
            neuronBackward->adapt(target, b);
            neuronBackward = neuronBackward->prev;
         }
         if (neuronForward != neuron) {
            neuronForward->adapt(target, b);
            neuronForward = neuronForward->next;
         }
         dd += 1.0;
      } //end neighborhood
   } //end conditional adapt 
}

/// - public method ------------------------------------------------------------
void CRing::regenerate(int step)
{
   SNeuron *del[m];
   int c = 0;
   SNeuron *cur = start->next;
   for (int i = 0; i < m; ++i) {
      if (not cur->isInhibited(step)) {
         del[c++] = cur;
      }
      cur = cur->next;
   }
   for (int i = 0; i < c; ++i) {
      removeNeuron(del[i]);
   }
}

/// - public method ------------------------------------------------------------
CoordsVector& CRing::get_ring_path(int step, CoordsVector &path) const
{
   path.clear();
   SNeuron *cur = start;
   for (int i = 0; i < m; ++i) {
      if (cur->isInhibited(step) and cur->targetOnTourStep >= 0 and cur == targets[cur->targetOnTour]->selectedWinner) {
         path.push_back(cur->alternateGoal);
      }
      cur = cur->next;
   }
   return path;
}

/// - public method ------------------------------------------------------------
IntVector& CRing::get_ring_route(int step, IntVector &route) const
{  
   route.clear();
   SNeuron *cur = start;
   for (int i = 0; i < m; ++i) {
      if (cur->isInhibited(step) and cur->targetOnTourStep >= 0 and cur == targets[cur->targetOnTour]->selectedWinner) {
         route.push_back(cur->targetOnTour);
      }
      cur = cur->next;
   }
   return route;
}

/* end of gsoa_ring.cc */
