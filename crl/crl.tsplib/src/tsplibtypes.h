/*
 * File name: tsplibtypes.h
 * Date:      2006/04/28 10:34
 * Author:
 */

#ifndef __TSPLIBTYPES_H__
#define __TSPLIBTYPES_H__

#include <crl/tsp/tspdatatsp.h>
#include <crl/tsp/tspnode.h>
#include <crl/tsp/tsptour.h>

namespace crl { namespace tsp {

   typedef int NodeLabel;
   ///
   /// @brief problem definition
   ///
   typedef crl::tsp::CTSPDataTSP<crl::tsp::STSPNode<double, NodeLabel>>
       TSPProblem;

   ///
   /// @brief tour solution of TSP Tour
   ///
   typedef crl::tsp::CTSPTour<NodeLabel> TSPTour;

}} // namespace crl::tsp

#endif

/* end of tsplibtypes.h */
