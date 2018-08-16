/*
 * File name: loadertour.h
 * Date:      2006/06/27 21:47
 * Author:    Jan Faigl
 */

#ifndef __LOADERTOUR_H__
#define __LOADERTOUR_H__

#include <crl/tsp/tspproblemloader.h>
#include <crl/tsp/tsplibtypes.h>

namespace crl { namespace tsp {

   /// --------------------------------------------------------------------------
   /// @brief  TSPLIB TSP TOUR loader
   /// solution of TSP problem, data section with one TOUR_SECTION
   /// TOUR_SECTION is a ordered sequence of nodes labels (int)
   /// --------------------------------------------------------------------------
   class CLoaderTour : public CTSPProblemLoader {
      static string capabilities;
      typedef NodeLabel Label;

    public:
      CLoaderTour();
      ~CLoaderTour();

      CTSPProblemLoader *clone(void);
      string getCapabilities(void);
      bool implements(CTSPLibSpec &spec);
      CTSPLibData *load(CTSPLibSpec &spec, std::ifstream &is);

    private:
      CTSPLibData *loadTour(CTSPLibSpec &spec, std::ifstream &is);
   };

}} // namespace crl::tsp

#endif

/* end of loadertour.h */
