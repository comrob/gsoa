/*
 * File name: route_module.cc
 * Date:      2011/03/06 13:17
 * Author:    Jan Faigl
 */

#include <crl/assert.h>

#include <crl/loader/route_module.h>

using namespace crl::loader;

const std::string crl::loader::CRoute::name = "route";

/// ----------------------------------------------------------------------------
static const std::string State[] = {
    "[ROUTE_INDEXES]",
    "[ROUTE_ROUTE_GOALS]",
    "[ROUTE_ROUTE_NODES]",
    "[ROUTE_ROUTES_COSTS]",
};

/// - constructor --------------------------------------------------------------
CRoute::CRoute() : CModule() {}

/// - destructor ---------------------------------------------------------------
CRoute::~CRoute() {}

/// - public method ------------------------------------------------------------
const std::string &CRoute::getName(void) const { return name; }

/// - public method ------------------------------------------------------------
const std::string &CRoute::getFirstSection(void) const
{
   return State[ROUTE_INDEXES];
}

/// - public method ------------------------------------------------------------
std::vector<std::string> &
CRoute::getSections(std::vector<std::string> &sections)
{
   for (int i = ROUTE_INDEXES; i != ROUTE_NUM; ++i) {
      sections.push_back(State[i]);
   }
   return sections;
}

/// - public method ------------------------------------------------------------
CRoute &CRoute::operator=(const CRoute &r)
{
   if (this != &r) {
      routeIndexes_ = r.routeIndexes_;
      routeGoals_ = r.routeGoals_;
      routeNodes_ = r.routeNodes_;
      routeCost_ = r.routeCost_;
   }
   return *this;
}

/// - public method ------------------------------------------------------------
void CRoute::clear(void)
{
   routeIndexes_.clear();
   routeGoals_.clear();
   routeNodes_.clear();
   routeCost_.clear();
}

/// - public method ------------------------------------------------------------
void CRoute::parse(std::istream &in, std::string &line)
{
   double v;
   int label;
   clear();
   Indexes indexes;
   enum { ROUTE_LABEL, ROUTE_DATA } routeState;
   routeState = ROUTE_LABEL;

   crl::assert_argument(line == getFirstSection(),
                        "CRoute must be initialized with the first section '" +
                            getFirstSection() + "'");
   TSections section = getSection(line);

   while (in and section != NONE) {
      switch (section) {
      case ROUTE_NUM:
         routeState = ROUTE_LABEL;
         if (in >> line) {
            section = getSection(line);
            if (section != NONE) {
               line = "";
            }
         }
         break;
      case ROUTE_INDEXES: // read graph nodes
         if (in >> label) {
            crl::assert_argument(
                label == routeIndexes_.size(),
                "ROUTE_INDEXES labels must be increasing (starting from 0)");
            routeIndexes_.push_back(label);
	 } else {
            in.clear();
            section = ROUTE_NUM;
         }
         break;
      case ROUTE_ROUTE_GOALS:
         switch (routeState) {
         case ROUTE_LABEL:
            if (in >> line >> label) {
               assert_argument(line == "ROUTE_LABEL", "Route label expected");
               assert_argument(label == routeGoals_.size(),
                               "Approriate route label expected");
               routeGoals_.push_back(Indexes());
               routeState = ROUTE_DATA;
            }
            break;
         case ROUTE_DATA:
            if (in >> label) {
               assert_argument(label >= 0,
                               "Node label in the route must be >= 0");
               routeGoals_.back().push_back(label);
	    } else {
               in.clear();
               section = ROUTE_NUM;
            }
            break;
         }
         break;
      case ROUTE_ROUTE_NODES:
         switch (routeState) {
         case ROUTE_LABEL:
            if (in >> line >> label) {
               assert_argument(line == "ROUTE_LABEL", "Route label expected");
               assert_argument(label == routeNodes_.size(),
                               "Approriate route label expected");
               routeNodes_.push_back(Indexes());
               routeState = ROUTE_DATA;
            }
            break;
         case ROUTE_DATA:
            if (in >> label) {
               assert_argument(label >= 0,
                               "Node label in the route must be >= 0");
               routeNodes_.back().push_back(label);
	    } else {
               in.clear();
               section = ROUTE_NUM;
            }
            break;
         }
         break;
      case ROUTE_ROUTES_COSTS:
         if (in >> label >> v) {
            crl::assert_argument(
                label >= 0 and label < routeIndexes_.size(),
                "A route costs label must correspond to the routes");
            routeCost_.push_back(LabeledCost(label, v));
	 } else {
            in.clear();
            section = ROUTE_NUM;
         }
         break;
      case NONE:
         // nothing to do
         break;
      } // end switch state
   }    // end while loop
}

/// - public method ------------------------------------------------------------
void CRoute::print(std::ostream &out) const
{
   if (!routeIndexes_.empty()) { // nodes_ is the mandatory section
      out << State[ROUTE_INDEXES] << std::endl;
      for (Indexes::const_iterator it = routeIndexes_.begin();
           it != routeIndexes_.end(); it++) {
         out << *it << std::endl;
      }
      out << std::endl;
      if (!routeGoals_.empty()) {
         int i = 0;
         for (IndexesList::const_iterator rIt = routeGoals_.begin();
              rIt != routeGoals_.end(); rIt++, i++) {
            out << State[ROUTE_ROUTE_GOALS] << std::endl;
            out << "ROUTE_LABEL"
                << " " << i << std::endl;
            for (Indexes::const_iterator it = rIt->begin(); it != rIt->end();
                 it++) {
               out << *it << std::endl;
            }
            out << std::endl;
         }
      }
      if (!routeNodes_.empty()) {
         int i = 0;
         for (IndexesList::const_iterator rIt = routeNodes_.begin();
              rIt != routeNodes_.end(); rIt++, i++) {
            out << State[ROUTE_ROUTE_NODES] << std::endl;
            out << "ROUTE_LABEL"
                << " " << i << std::endl;
            for (Indexes::const_iterator it = rIt->begin(); it != rIt->end();
                 it++) {
               out << *it << std::endl;
            }
            out << std::endl;
         }
      }
      if (!routeCost_.empty()) {
         out << State[ROUTE_ROUTES_COSTS] << std::endl;
         for (LabeledCostVector::const_iterator it = routeCost_.begin();
              it != routeCost_.end(); it++) {
            out << it->label << " " << it->cost << std::endl;
         }
         out << std::endl;
      }
   }
}

/// - public method ------------------------------------------------------------
void CRoute::fill(const CModule *module)
{
   const CRoute *m = dynamic_cast<const CRoute *>(module);
   if (m) {
      *this = *m;
   }
}

/// - private method -----------------------------------------------------------
CRoute::TSections CRoute::getSection(const std::string &name) const
{
   TSections ret = NONE;
   for (int i = 0; i < ROUTE_NUM; i++) {
      if (State[i] == name) {
         ret = TSections(i);
         break;
      }
   }
   return ret;
}

/* end of route_module.cc */
