/*
 * File name: canvasview_gsoa.h
 * Date:      2016/12/09 08:01
 * Author:    Jan Faigl
 */

#ifndef __CANVASVIEW_GSOA_H__
#define __CANVASVIEW_GSOA_H__

#include <vector>

#include <boost/foreach.hpp>

#include <crl/gui/gui.h>
#include <crl/gui/colors.h>
#include <crl/gui/renderer.h>
#include <crl/gui/canvas.h>

#include "target.h"
#include "gsoa_neuron.h"
#include "gsoa_ring.h"

namespace gsoa {
   typedef std::vector<gsoa::STarget*> TargetPtrVector;
} // end namespace gsoa

/// ----------------------------------------------------------------------------
inline crl::gui::CCanvasBase& operator<<(crl::gui::CCanvasBase &canvas, const gsoa::STarget &target) 
{
   canvas << target.coords.x << target.coords.y;
   return canvas;
}

/// ----------------------------------------------------------------------------
inline crl::gui::CCanvasBase& operator<<(crl::gui::CCanvasBase &canvas, const gsoa::TargetPtrVector &targets)
{
   BOOST_FOREACH(const gsoa::STarget *target, targets) {
      canvas << target->coords.x << target->coords.y;
   }
   return canvas;
}

/// ----------------------------------------------------------------------------
inline crl::gui::CCanvasBase& operator<<(crl::gui::CCanvasBase &canvas, const gsoa::SNeuron &neuron) 
{
   canvas << neuron.coords.x << neuron.coords.y;
   return canvas;
}

/// ----------------------------------------------------------------------------
inline crl::gui::CCanvasBase& operator<<(crl::gui::CCanvasBase &canvas, const gsoa::SNeuron *neuron) 
{
   if (neuron) {
      canvas << neuron->coords.x << neuron->coords.y;
   }
   return canvas;
}

/// ----------------------------------------------------------------------------
inline crl::gui::CCanvasBase& operator<<(crl::gui::CCanvasBase &canvas, const gsoa::CRing &ring) 
{
   const int n = ring.size();
   const gsoa::SNeuron *cur = ring.begin();
   for (int i = 0; i < n; ++i) {
      if (cur) {
         canvas << cur->coords.x << cur->coords.y;
         cur = cur->next;
      } else {
         break;
      }
   }
   return canvas;
}

/// ----------------------------------------------------------------------------
inline crl::gui::CCanvasBase& operator<<(crl::gui::CCanvasBase &canvas, const gsoa::CRing *ring) 
{
   return ring ? canvas << *ring : canvas;
}

#endif

/* end of canvasview_gsoa.h */
