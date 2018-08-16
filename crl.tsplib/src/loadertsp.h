/*
 * File name: loadertsp.h
 * Date:      2006/04/27 10:32
 * Author:    Jan Faigl
 */

#ifndef __TSPLIBLOADERTSP_H__
#define __TSPLIBLOADERTSP_H__

#include <vector>

#include <crl/tsp/tsplibdata.h>
#include <crl/tsp/tsplibtypes.h>
#include <crl/tsp/tspproblemloader.h>

namespace crl { namespace tsp {

   /// --------------------------------------------------------------------------
   /// @brief  TSPLIB loader for TSP problems
   /// --------------------------------------------------------------------------
   class CLoaderTSP : public CTSPProblemLoader {
      static string capabilities;
      typedef TSPProblem::Node Node;

    public:
      CLoaderTSP();
      ~CLoaderTSP();

      CTSPProblemLoader *clone(void);
      string getCapabilities(void);
      bool implements(CTSPLibSpec &spec);
      CTSPLibData *load(CTSPLibSpec &spec, std::ifstream &is);

    private:
      CTSPLibData *loadNodes(CTSPLibSpec &spec, std::ifstream &is);
   };

}} // namespace crl::tsp

#endif

/* end of loadertsp.h */
