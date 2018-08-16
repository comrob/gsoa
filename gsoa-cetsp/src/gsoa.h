/*
 * File name: gsoa.h
 * Date:      2016/12/07 08:32
 * Author:    Jan Faigl
 */

#ifndef __GSOA_H__
#define __GSOA_H__

#include <crl/config.h>
#include <crl/alg/algorithm.h>

#include <crl/gui/shape.h>

#include "coords.h"
#include "target.h"
#include "gsoa_ring.h"

namespace gsoa {

   class CGSOA : public crl::CAlgorithm {
      typedef crl::CAlgorithm Base;
      typedef std::vector<int> IntVector;
      public:

      static crl::CConfig &getConfig(crl::CConfig &config);

      CGSOA(crl::CConfig &config);
      ~CGSOA();

      std::string getVersion(void);
      std::string getRevision(void);

      void visualize(const CoordsVector & path);
      void solve(void);

      protected:
      void load(void);
      void initialize(void);
      void after_init(void); 

      void iterate(int iter);
      double refine(int step, double errorMax);

      void save(void);
      void release(void);

      void defineResultLog(void);
      void fillResultRecord(int trial);

      private:
      void drawPath(void);
      void drawRing(int step);
      void savePic(int step, bool detail = false, const std::string &dir_suffix = "");

      void getSolution(int step, CoordsVector &solution) const;

      private:
      const int DEPOT_IDX; 
      const bool VARIABLE_RADIUS;
      const bool SAVE_RESULTS;
      const bool SAVE_SETTINGS;
      const bool SAVE_INFO;

      const bool DRAW_RING_ITER;
      const bool DRAW_RING_ENABLE;
      const bool DRAW_RING_NODES;
      const bool DRAW_TOUR_REPRESENTED_BY_RING;
      const bool SAVE_PIC;

      const bool IN_NEIGH_WAYPOINT;

      crl::gui::CShape shapeTargets;
      crl::gui::CShape shapeNeurons;
      crl::gui::CShape shapePath;
      crl::gui::CShape shapeRing;
      crl::gui::CShape shapeCommRadius;
      crl::gui::CShape shapePathNodes;
      crl::gui::CShape shapeDepot;
      crl::gui::CShape shapeTourRepresentedByRing;

      std::string method;

      IntVector permutation;
      TargetPtrVector targets;
      CoordsVector finalSolution;

      CRing *ring;
   };

} // end name gsoa

#endif

/* end of gsoa.h */
