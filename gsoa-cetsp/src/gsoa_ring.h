/*
 * File name: gsoa_ring.h
 * Date:      2016/12/09 08:18
 * Author:    Jan Faigl
 */

#ifndef __GSOA_RING_H__
#define __GSOA_RING_H__

#include <vector>

#include "coords.h"
#include "target.h"
#include "gsoa_neuron.h"

namespace gsoa {

   typedef std::vector<int> IntVector;

   /// ----------------------------------------------------------------------------
   /// @brief 
   /// ----------------------------------------------------------------------------
   struct SWinnerSelection {
      bool hasWinner;
      STarget *target;
      SNeuron *winner; // if 0, use the newPoint
      SNeuron *preWinner;
      Coords newPoint; 
      Coords alternateGoal;
      int targetOnTour;
   };

   /// ----------------------------------------------------------------------------
   /// @brief 
   /// ----------------------------------------------------------------------------
   class CRing {
      const double RADIUS_DECREASE;
      const bool IN_NEIGH_WAYPOINT;
      public:
      CRing(const TargetPtrVector &targets, const double radiusDecrease = 0.1, const bool in_neigh_waypoint = false);
      ~CRing();

      int size(void) const { return m; }
      const SNeuron* begin(void) const { return start; }

      void initialize_neurons(const Coords &pt);
      void deallocate_neurons(void);

      SNeuron* insertNeuron(SNeuron* cur, SNeuron* neuron);
      void removeNeuron(SNeuron *neuron);

      SWinnerSelection* selectWinner(int step, STarget* target, double &errorToGoal);
      void adapt(const int step);
      void regenerate(int step);

      CoordsVector& get_ring_path(int step, CoordsVector &path) const;
      IntVector& get_ring_route(int step, IntVector &route) const;
      private:
      const TargetPtrVector &targets;
      SNeuron *start;
      int m;
      SWinnerSelection winner;
   };

} // end namespace gsoa

#endif

/* end of gsoa_ring.h */
