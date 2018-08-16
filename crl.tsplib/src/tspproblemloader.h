/*
 * File name: tspproblemloader.h
 * Date:      2006/04/27 08:35
 * Author:    Jan Faigl
 */

#ifndef __TSPPROBLEMLOADER_H__
#define __TSPPROBLEMLOADER_H__

#include <fstream>
#include <string>

#include <crl/tsp/tsplibdata.h>
#include <crl/tsp/tsplibspec.h>

using std::string;

namespace crl { namespace tsp {

   /// --------------------------------------------------------------------------
   /// @brief  Common class for tsp problem loaders
   /// --------------------------------------------------------------------------
   class CTSPProblemLoader {

    public:
      CTSPProblemLoader(){};
      virtual ~CTSPProblemLoader(){};

      /// --------------------------------------------------------------------------
      /// @brief clone
      ///
      /// @return new instance of loader
      /// --------------------------------------------------------------------------
      virtual CTSPProblemLoader *clone(void) = 0;

      /// --------------------------------------------------------------------------
      /// @brief getCapabilities
      ///
      /// @return description of loader capabilities
      /// --------------------------------------------------------------------------
      virtual string getCapabilities(void) = 0;

      /// --------------------------------------------------------------------------
      /// @brief implements
      ///
      /// @param spec
      ///
      /// @return true if class implements given TSPLIB specification
      /// --------------------------------------------------------------------------
      virtual bool implements(CTSPLibSpec &spec) = 0;

      /// --------------------------------------------------------------------------
      /// @brief load
      ///
      /// @param spec
      /// @param in
      ///
      /// @return 0 if spec if no implemented otherwise loaded TSPLIB data
      /// --------------------------------------------------------------------------
      virtual CTSPLibData *load(CTSPLibSpec &spec, std::ifstream &in) = 0;
   };
}} // namespace crl::tsp

#endif

/* end of tspproblemloader.h */
