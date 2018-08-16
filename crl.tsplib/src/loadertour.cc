/*
 * File name: loadertour.cc
 * Date:      2006/06/27 22:15
 * Author:    Jan Faigl
 */

#include <crl/logging.h>
#include <crl/stringconversions.h>

#include <crl/tsp/loadertour.h>

using namespace crl::tsp;

/// --------------------------------------------------------------------------
/// Class CLoaderTour
/// --------------------------------------------------------------------------
string CLoaderTour::capabilities = "TYPE:TOUR";

/// --------------------------------------------------------------------------
CLoaderTour::CLoaderTour() : CTSPProblemLoader() {}

/// --------------------------------------------------------------------------
CLoaderTour::~CLoaderTour() {}

/// --------------------------------------------------------------------------
CTSPProblemLoader *CLoaderTour::clone(void) { return new CLoaderTour(); }

/// --------------------------------------------------------------------------
string CLoaderTour::getCapabilities(void) { return capabilities; }

/// --------------------------------------------------------------------------
bool CLoaderTour::implements(CTSPLibSpec &spec)
{
   bool ret = false;
   if ((spec[Spec::TYPE] == "TOUR") && (spec[Spec::CAPACITY] == "") &&
       (spec[Spec::EDGE_WEIGHT_TYPE] == "") &&
       (spec[Spec::EDGE_WEIGHT_FORMAT] == "") &&
       (spec[Spec::EDGE_DATA_FORMAT] == "") &&
       (spec[Spec::NODE_COORD_TYPE] == "") &&
       (spec[Spec::DISPLAY_DATA_TYPE] == "")) {
      ret = true;
   }
   return ret;
}

/// --------------------------------------------------------------------------
CTSPLibData *CLoaderTour::load(CTSPLibSpec &spec, std::ifstream &is)
{
   CTSPLibData *ret = 0;
   DataSection::Type section;
   bool r = implements(spec);
   bool tourLoaded = false;
   if (r) {
      while (spec.getDataSection(section)) {
         switch (section) {
         case DataSection::TOUR_SECTION:
            if (tourLoaded) {
               WARN("One tour is already loaded, CLoaderTour support only one "
                    "tour (TSP)");
	    } else {
               ret = loadTour(spec, is);
               tourLoaded = true;
            }
            break;
         default:
            WARN("DataSection '" + spec.get(section) +
                 "'not implemented by CLoaderTour");
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
CTSPLibData *CLoaderTour::loadTour(CTSPLibSpec &spec, std::ifstream &is)
{
   int number;
   int count = -2;
   int j = 0;
   TSPTour::Label n;
   number =
       (crl::string_cast<int>(spec.getSpec(Spec::DIMENSION), number) ? number
                                                                     : -1);
   TSPTour *ret = new TSPTour(spec);
   is.clear();
   while ((count < number) && (is >> n)) {
      if (n != -1) {
         count += 1;
         ret->addNode(n);
      } else {
         // end of tour section
      }
   }
   if (number > -1) {
      if ((count + 2) != number) {
         WARN("File does not contain '" + string_format<int>(number) +
              "' nodes,  only '" + string_format<int>(ret->getTour().size()) +
              " nodes are loaded, count:" + string_format<int>(count));
      }
   } else {
      is.clear();
   }
   return ret;
}

/* end of loadertour.cc */
