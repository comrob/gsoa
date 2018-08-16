/*
 * File name: tsptour.h
 * Date:      2006/06/28 16:28
 * Author:    Jan Faigl
 */

#ifndef __TSPTOUR_H__
#define __TSPTOUR_H__

#include <vector>

#include <crl/tsp/tsplibdata.h>
#include <crl/tsp/tsplibspec.h>

namespace crl { namespace tsp {

   /// --------------------------------------------------------------------------
   /// @brief solution tour definition
   /// --------------------------------------------------------------------------
   template <class T, class Container = typename std::vector<T>>
   class CTSPTour : public CTSPLibData {
    public:
      typedef T Label;
      typedef Container TourList;

    private:
      const CTSPLibSpec &spec;
      TourList tour;

    public:
      CTSPTour(CTSPLibSpec &iSpec) : spec(iSpec){};
      ~CTSPTour(){};

      const CTSPLibSpec &getSpec(void) { return spec; }

      void addNode(Label &l) { tour.push_back(l); }

      TourList &getTour(void) { return tour; }
   };

}} // namespace crl::tsp

#endif

/* end of tsptour.h */
