/*
 * File name: gsoa_neuron.h
 * Date:      2016/12/09 08:20
 * Author:    Jan Faigl
 */

#ifndef __GSOA_NEURON_H__
#define __GSOA_NEURON_H__

#include "coords.h"

namespace gsoa {

   struct SNeuron {
      SNeuron* prev;
      SNeuron* next;
      Coords coords;
      Coords alternateGoal;
      int targetOnTourStep;
      int targetOnTour; 

      char s;
      Coords x;
      double dist2; //squared distance 

      SNeuron()
      {
         clear();
      }

      SNeuron(const Coords &pt) : coords(pt)
      {
         clear();
      } 

      ~SNeuron() {}

      void setPoint(const Coords &pt) 
      {
         coords = pt;
         clear();
      }

      void clear(void) 
      {
         targetOnTour = -1;
         targetOnTourStep = -1;
      }

      void adapt(const Coords &pt, const double beta)
      {
         coords.x += beta * (pt.x - coords.x);
         coords.y += beta * (pt.y - coords.y);
      }

      inline bool isInhibited(int step) const { return targetOnTourStep >= step; }
   };

} // end namespace gsoa

#endif

/* end of gsoa_neuron.h */
