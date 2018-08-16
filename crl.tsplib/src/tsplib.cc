/*
 * File name: tsplib.cc
 * Date:      2006/04/27 13:00
 * Author:    Jan Faigl
 */

#include <crl/logging.h>
#include <crl/stringconversions.h>

#include <crl/tsp/tsplib.h>

using namespace crl::tsp;

/// --------------------------------------------------------------------------
/// @brief  TSPLib loader wrapper
/// --------------------------------------------------------------------------
CTSPLib::CTSPLib() { loaders.clear(); };

/// --------------------------------------------------------------------------
CTSPLib::~CTSPLib()
{
   for (unsigned int i = 0; i < loaders.size(); ++i) {
      delete loaders[i];
   }
}

/// --------------------------------------------------------------------------
void CTSPLib::registerLoader(CTSPProblemLoader &loader)
{
   loaders.push_back(loader.clone());
}

/// --------------------------------------------------------------------------
CTSPLibData *CTSPLib::load(std::string filename)
{
   CTSPLibData *data = 0;
   CTSPLibSpec spec;
   std::ifstream in(filename.c_str());
   if (in) {
      if (in >> spec) {
         for (Loaders::iterator it = loaders.begin(); it != loaders.end();
              ++it) {
            if ((*it)->implements(spec)) {
               if ((data = (*it)->load(spec, in))) {
                  INFO("TSPLIB data from '" + filename + "' was loaded");
                  break;
               }
            }
         }
         if (!data) {
            WARN("Can not load data from '" + filename +
                 "' no registered loader can be used");
         }
      } else {
         WARN("Can not load TSPLIB problem specification from '" + filename +
              "'");
      }
   } else {
      WARN("Can not open file '" + filename + "'");
   }
   return data;
}

/// --------------------------------------------------------------------------
/// Private
/// --------------------------------------------------------------------------
std::ifstream &CTSPLib::loadSpec(std::ifstream &in, CTSPLibSpec &spec)
{
   string specStr;
   while (in) {
      in >> specStr;
      if (in) {
         if (specStr.size() > 0) {
            std::cout << "READED : " << specStr << std::endl;
	 } else {
         }
      }
   }
   return in;
}

/* end of tsplib.cc */
