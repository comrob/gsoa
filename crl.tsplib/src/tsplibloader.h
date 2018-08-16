/*
 * File name: tsplibloader.h
 * Date:      2006/04/27 13:50
 * Author:    Jan Faigl
 */

#ifndef __TSPLIBLOADER_H__
#define __TSPLIBLOADER_H__

#include <crl/tsp/tsplib.h>

namespace crl { namespace tsplib {

   /// --------------------------------------------------------------------------
   /// @brief Class for loading TSPLIB problems
   /// --------------------------------------------------------------------------
   class CTSPLibLoader {
      tsp::CTSPLib tsplib;

    public:
      CTSPLibLoader();

      virtual ~CTSPLibLoader();

      crl::tsp::CTSPLibData *load(std::string filename);
   };

   extern CTSPLibLoader loader;

}} // namespace crl::tsplib

#endif

/* end of tsplibloader.h */
