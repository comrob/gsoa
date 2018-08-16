/*
 * File name: tspdatatsp.h
 * Date:      2006/04/27 10:50
 * Author:    Jan Faigl
 */

#ifndef __TSPDATATSP_H__
#define __TSPDATATSP_H__

#include <vector>

#include <crl/tsp/tsplibdata.h>
#include <crl/tsp/tsplibspec.h>

namespace crl { namespace tsp {

   /// --------------------------------------------------------------------------
   /// @brief task definition of TSP problem (cities description)
   /// --------------------------------------------------------------------------
   template <class T, class Container = typename std::vector<T>>
   class CTSPDataTSP : public CTSPLibData {
    public:
      typedef T Node;
      typedef Container NodeList;

    private:
      const CTSPLibSpec &spec;
      NodeList nodes;

    public:
      CTSPDataTSP(CTSPLibSpec &iSpec) : spec(iSpec){};
      ~CTSPDataTSP(){};

      NodeList &getNodes(void) { return nodes; }

      void addNode(Node &n) { nodes.push_back(n); }

      const CTSPLibSpec &getSpec(void) { return spec; }
   };

}} // namespace crl::tsp

#endif

/* end of tspdatatsp.h */
