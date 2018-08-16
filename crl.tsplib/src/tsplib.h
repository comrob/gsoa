/*
 * File name: tsplib.h
 * Date:      2006/04/27 08:44
 * Author:    Jan Faigl
 */

#ifndef __TSPLIB_H__
#define __TSPLIB_H__

#include <fstream>
#include <string>
#include <vector>

#include <crl/tsp/tsplibdata.h>
#include <crl/tsp/tsplibspec.h>
#include <crl/tsp/tspproblemloader.h>

namespace crl { namespace tsp {

   /// --------------------------------------------------------------------------
   /// @brief  TSPLib loader wrapper
   /// --------------------------------------------------------------------------
   class CTSPLib {
      typedef std::vector<CTSPProblemLoader *> Loaders;
      Loaders loaders;

    public:
      CTSPLib();
      ~CTSPLib();

      /// --------------------------------------------------------------------------
      /// @brief registerLoader register given loader into library
      ///
      /// @param loader
      /// --------------------------------------------------------------------------
      void registerLoader(CTSPProblemLoader &loader);

      /// --------------------------------------------------------------------------
      /// @brief load
      ///
      /// @param filename
      ///
      /// @return loaded data or 0
      /// --------------------------------------------------------------------------
      CTSPLibData *load(std::string filename);

    private:
      std::ifstream &loadSpec(std::ifstream &in, CTSPLibSpec &spec);
   };

}} // namespace crl::tsp

#endif

/* end of tsplib.h */
