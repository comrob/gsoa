/*
 * File name: graph_module.cc
 * Date:      2011/02/17 17:06
 * Author:    Jan Faigl
 */

#include <crl/assert.h>

#include <crl/loader/graph_module.h>

using namespace crl::loader;

const std::string crl::loader::CGraph::name = "graph";

/// ----------------------------------------------------------------------------
static const std::string State[] = {
    "[GRAPH_NODES]",       "[GRAPH_EDGES]",  "[GRAPH_CITY_NODES]",
    "[GRAPH_EDGES_COSTS]", "[GRAPH_DEPOTS]",
};

/// - constructor --------------------------------------------------------------
CGraph::CGraph() : CModule() {}

/// - destructor ---------------------------------------------------------------
CGraph::~CGraph() {}

/// - public method ------------------------------------------------------------
const std::string &CGraph::getName(void) const { return name; }

/// - public method ------------------------------------------------------------
const std::string &CGraph::getFirstSection(void) const
{
   return State[GRAPH_NODES];
}

/// - public method ------------------------------------------------------------
std::vector<std::string> &
CGraph::getSections(std::vector<std::string> &sections)
{
   for (int i = GRAPH_NODES; i != GRAPH_NUM; ++i) {
      sections.push_back(State[i]);
   }
   return sections;
}

/// - public method ------------------------------------------------------------
CGraph &CGraph::operator=(const CGraph &g)
{
   if (this != &g) {
      nodes_ = g.nodes_;
      edges_ = g.edges_;
      cities_ = g.cities_;
      edgesCosts_ = g.edgesCosts_;
      depots_ = g.depots_;
   }
   return *this;
}

/// - public method ------------------------------------------------------------
void CGraph::clear(void)
{
   nodes_.clear();
   edges_.clear();
   cities_.clear();
   edgesCosts_.clear();
   depots_.clear();
}

/// - public method ------------------------------------------------------------
void CGraph::parse(std::istream &in, std::string &line)
{
   double x, y;
   int label;
   int v1, v2, v3;
   clear();
   Indexes indexes;

   crl::assert_argument(line == getFirstSection(),
                        "CGraph must be initialized with the first section '" +
                            getFirstSection() + "'");
   TSections section = getSection(line);

   while (in and section != NONE) {
      switch (section) {
      case GRAPH_NUM:
         if (in >> line) {
            section = getSection(line);
            if (section != NONE) {
               line = "";
            }
         }
         break;
      case GRAPH_NODES: // read graph nodes
         if (in >> label >> x >> y) {
            crl::assert_argument(
                label == nodes_.size(),
                "GRAPH_NODES labels must be increasing (starting from 0)");
            nodes_.push_back(Point(x, y, label));
	 } else {
            in.clear();
            section = GRAPH_NUM;
         }
         break;
      case GRAPH_EDGES:
         if (in >> label >> v1 >> v2) {
            crl::assert_argument(
                label == edges_.size(),
                "GRAPH_EDGES labels must be increasing (starting from 0)");
            crl::assert_argument(
                v1 >= 0 and v1 < nodes_.size() and v2 >= 0 and
                    v2 < nodes_.size(),
                "Nodes labels must correspond to the GRAPH_NODES");
            edges_.push_back(EdgeNode(label, v1, v2));
	 } else {
            in.clear();
            section = GRAPH_NUM;
         }
         break;
      case GRAPH_CITY_NODES:
         if (in >> label) {
            crl::assert_argument(label >= 0 and label < nodes_.size(),
                                 "A city label in GRAPH_CITY_NODES must "
                                 "correspond with the number of GRAPH_NODES");
            cities_.push_back(label);
	 } else {
            in.clear();
            section = GRAPH_NUM;
         }
         break;
      case GRAPH_EDGES_COSTS:
         if (in >> label >> x) {
            crl::assert_argument(
                label >= 0 and label < edges_.size(),
                "A edge costs label must correspond to the edges");
            edgesCosts_.push_back(LabeledCost(label, x));
	 } else {
            in.clear();
            section = GRAPH_NUM;
         }
         break;
      case GRAPH_DEPOTS:
         if (in >> label) {
            crl::assert_argument(label >= 0 and label < nodes_.size(),
                                 "A label in GRAPH_DEPOTS must correspond with "
                                 "the number of GRAPH_NODES");
            depots_.push_back(label);
	 } else {
            in.clear();
            section = GRAPH_NUM;
         }
         break;
      case NONE:
         // nothing to do
         break;
      } // end switch state
   }    // end while loop
}

/// - public method ------------------------------------------------------------
void CGraph::print(std::ostream &out) const
{
   if (!nodes_.empty()) { // nodes_ is the mandatory section
      out << State[GRAPH_NODES] << std::endl;
      for (Points::const_iterator it = nodes_.begin(); it != nodes_.end();
           it++) {
         out << it->label << " " << it->x << " " << it->y << std::endl;
      }
      out << std::endl;
      if (!edges_.empty()) {
         out << State[GRAPH_EDGES] << std::endl;
         for (EdgeNodeVector::const_iterator it = edges_.begin();
              it != edges_.end(); it++) {
            out << it->label << " " << it->v1 << " " << it->v2 << std::endl;
         }
         out << std::endl;
      }
      if (!cities_.empty()) {
         out << State[GRAPH_CITY_NODES] << std::endl;
         for (Indexes::const_iterator it = cities_.begin(); it != cities_.end();
              it++) {
            out << *it << std::endl;
         }
         out << std::endl;
      }
      if (!edgesCosts_.empty()) {
         out << State[GRAPH_EDGES_COSTS] << std::endl;
         for (LabeledCostVector::const_iterator it = edgesCosts_.begin();
              it != edgesCosts_.end(); it++) {
            out << it->label << " " << it->cost << std::endl;
         }
         out << std::endl;
      }
      if (!depots_.empty()) {
         out << State[GRAPH_DEPOTS] << std::endl;
         for (Indexes::const_iterator it = depots_.begin(); it != depots_.end();
              it++) {
            out << *it << std::endl;
         }
         out << std::endl;
      }
   }
}

/// - public method ------------------------------------------------------------
void CGraph::fill(const CModule *module)
{
   const CGraph *m = dynamic_cast<const CGraph *>(module);
   if (m) {
      *this = *m;
   }
}

/// - private method -----------------------------------------------------------
CGraph::TSections CGraph::getSection(const std::string &name) const
{
   TSections ret = NONE;
   for (int i = 0; i < GRAPH_NUM; i++) {
      if (State[i] == name) {
         ret = TSections(i);
         break;
      }
   }
   return ret;
}

/* end of graph_module.cc */
