/*
 * File name: loadertsp.cc
 * Date:      2006/04/27 10:40
 * Author:    Jan Faigl
 */

#include <crl/logging.h>
#include <crl/stringconversions.h>

#include <crl/tsp/tsplibtypes.h>
#include <crl/tsp/loadertsp.h>

using namespace crl::tsp;

/// --------------------------------------------------------------------------
/// Class CLoaderTSP
/// --------------------------------------------------------------------------
string CLoaderTSP::capabilities =
    "TYPE:TSP;EDGE_WEIGHT_TYPE:EUC_2D;NODE_COORD_TYPE:TWOD_COORDS";

/// --------------------------------------------------------------------------
CLoaderTSP::CLoaderTSP() : CTSPProblemLoader() {}

/// --------------------------------------------------------------------------
CLoaderTSP::~CLoaderTSP() {}

/// --------------------------------------------------------------------------
CTSPProblemLoader *CLoaderTSP::clone(void) { return new CLoaderTSP(); }

/// --------------------------------------------------------------------------
string CLoaderTSP::getCapabilities(void) { return capabilities; }

/// --------------------------------------------------------------------------
bool CLoaderTSP::implements(CTSPLibSpec &spec)
{
   bool ret = false;

   if ((spec[Spec::TYPE] == "TSP") && (spec[Spec::CAPACITY] == "") &&
       (spec[Spec::EDGE_WEIGHT_TYPE] == "EUC_2D") &&
       (spec[Spec::EDGE_WEIGHT_FORMAT] == "") &&
       (spec[Spec::EDGE_DATA_FORMAT] == "") &&
       (spec[Spec::DISPLAY_DATA_TYPE] == "")) {
      if (spec[Spec::NODE_COORD_TYPE] == "") { // patch to loader berlin52 and
                                               // so
         spec.setSpec(Spec::NODE_COORD_TYPE, "TWOD_COORDS");
         WARN("NODE_COORD_TYPE is not set used default TWOD_COORDS");
      }
      if (spec[Spec::NODE_COORD_TYPE] == "TWOD_COORDS") {
         ret = true;
      }
   }
   return ret;
}

/// --------------------------------------------------------------------------
CTSPLibData *CLoaderTSP::load(CTSPLibSpec &spec, std::ifstream &is)
{
   CTSPLibData *ret = 0;
   DataSection::Type section;
   bool r = implements(spec);
   if (r) {
      while (spec.getDataSection(section)) {
         switch (section) {
         case DataSection::NODE_COORD_SECTION:
            ret = loadNodes(spec, is);
            break;
         default:
            WARN("DataSection '" + spec.get(section) + "'not implemented");
            break;
         }
         is >> spec;
      }
      if (!spec.getEof()) {
         ERROR("EOF expected! loading was terminated");
      }
   } else {
      ERROR("This loader can not load give TSPLIB specification");
   }
   return ret;
}

/// --------------------------------------------------------------------------
/// Private
/// --------------------------------------------------------------------------
CTSPLibData *CLoaderTSP::loadNodes(CTSPLibSpec &spec, std::ifstream &is)
{
   int number;
   int count = -2;
   int j = 0;
   number =
       (crl::string_cast<int>(spec.getSpec(Spec::DIMENSION), number) ? number
                                                                     : -1);
   TSPProblem *ret = new TSPProblem(spec);
   Node n;
   std::string s;
   if (number > -1) {
      count = 0;
      j = 1;
   }
   is.clear();
   while ((count < number) && (is >> n)) {
      count += j;
      ret->addNode(n);
   }
   if (number > -1) {
      if (count != number) {
         WARN("File does not contain '" + string_format<int>(number) +
              "' nodes; only '" + string_format<int>(ret->getNodes().size()) +
              " nodes are loaded");
      }
   } else {
      is.clear();
   }
   return ret;
}

/* end of loadertsp.cc */
