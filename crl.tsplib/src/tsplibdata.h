/*
 * File name: tsplibdata.h
 * Date:      2006/04/28 10:34
 * Author:
 */

#ifndef __TSPLIBDATA_H__
#define __TSPLIBDATA_H__

#include <crl/tsp/tsplibspec.h>

namespace crl { namespace tsp {

   /// --------------------------------------------------------------------------
   /// @brief  TSP Data common interface
   /// --------------------------------------------------------------------------
   class CTSPLibData {
    public:
      virtual const CTSPLibSpec &getSpec(void) = 0;
   };

}} // namespace crl::tsp

#endif

/* end of tsplibdata.h */
