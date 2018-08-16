/*
 * File name: distances_module.cc
 * Date:      2011/02/24 20:53
 * Author:    Jan Faigl
 */

#include <crl/assert.h>

#include <crl/loader/distances_module.h>

using namespace crl::loader;

const std::string crl::loader::CDistances::name = "distances";

/// ----------------------------------------------------------------------------
static const std::string State[] = {
    "[DISTANCES_NODES]",
    "[DISTANCES_NODE_NODE_DISTANCES]",
};

/// - constructor --------------------------------------------------------------
CDistances::CDistances() : CModule() {}

/// - destructor ---------------------------------------------------------------
CDistances::~CDistances() {}

/// - public method ------------------------------------------------------------
const std::string &CDistances::getName(void) const { return name; }

/// - public method ------------------------------------------------------------
const std::string &CDistances::getFirstSection(void) const
{
   return State[DISTANCES_NODES];
}

/// - public method ------------------------------------------------------------
std::vector<std::string> &
CDistances::getSections(std::vector<std::string> &sections)
{
   for (int i = DISTANCES_NODES; i != DISTANCES_NUM; ++i) {
      sections.push_back(State[i]);
   }
   return sections;
}

/// - public method ------------------------------------------------------------
CDistances &CDistances::operator=(const CDistances &g)
{
   if (this != &g) {
      nodes_ = g.nodes_;
      distances_ = g.distances_;
   }
   return *this;
}

/// - public method ------------------------------------------------------------
void CDistances::clear(void)
{
   nodes_.clear();
   distances_.clear();
}

/// - public method ------------------------------------------------------------
void CDistances::parse(std::istream &in, std::string &line)
{
   int label;
   int v1, v2;
   double distance;
   clear();
   crl::assert_argument(
       line == getFirstSection(),
       "CDistances must be initialized with the first section '" +
           getFirstSection() + "'");
   TSections section = getSection(line);
   while (in and section != NONE) {
      switch (section) {
      case DISTANCES_NUM:
         if (in >> line) {
            section = getSection(line);
            if (section != NONE) {
               line = "";
            }
         }
         break;
      case DISTANCES_NODES: // read the indexes
         if (in >> label) {
            nodes_.push_back(label);
	 } else {
            in.clear();
            section = DISTANCES_NUM;
         }
         break;
      case DISTANCES_NODE_NODE_DISTANCES:
         if (in >> v1 >> v2 >> distance) {
            distances_.push_back(NodesDistance(v1, v2, distance));
	 } else {
            in.clear();
            section = DISTANCES_NUM;
         }
         break;
      case NONE:
         // nothing to do
         break;
      } // end switch state
   }    // end while loop
}

/// - public method ------------------------------------------------------------
void CDistances::print(std::ostream &out) const
{
   if (!nodes_.empty()) { // nodes_ is the mandatory section
      out << State[DISTANCES_NODES] << std::endl;
      for (Indexes::const_iterator it = nodes_.begin(); it != nodes_.end();
           it++) {
         out << *it << std::endl;
      }
      out << std::endl;
      if (!distances_.empty()) {
         out << State[DISTANCES_NODE_NODE_DISTANCES] << std::endl;
         for (NodesDistanceVector::const_iterator it = distances_.begin();
              it != distances_.end(); it++) {
            out << it->v1 << " " << it->v2 << " " << it->distance << std::endl;
         }
         out << std::endl;
      }
   }
}

/// - public method ------------------------------------------------------------
void CDistances::fill(const CModule *module)
{
   const CDistances *m = dynamic_cast<const CDistances *>(module);
   if (m) {
      *this = *m;
   }
}

/// - private method -----------------------------------------------------------
CDistances::TSections CDistances::getSection(const std::string &name) const
{
   TSections ret = NONE;
   for (int i = 0; i < DISTANCES_NUM; i++) {
      if (State[i] == name) {
         ret = TSections(i);
         break;
      }
   }
   return ret;
}

/* end of distances_module.cc */
