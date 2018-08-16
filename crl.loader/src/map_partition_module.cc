/*
 * File name: map_partition_module.cc
 * Date:      2011/02/15 17:24
 * Author:    Jan Faigl
 */

#include <crl/assert.h>
#include <crl/loader/map_partition_module.h>

using namespace crl::loader;

const std::string crl::loader::CMapPartition::name = "partition";

/// ----------------------------------------------------------------------------
static const std::string State[] = {"[PARTITION_POINTS]", "[PARTITION_REGION]",
                                    "[PARTITION_POINTS_ON_MAP_EDGES]",
                                    "[PARTITION_SIMPLE_POLYGONS_WITH_GATES]",
                                    "[PARTITION_TRIANGLES]"};

/// ----------------------------------------------------------------------------
typedef enum { REGION_LABEL, REGION_DATA } TRegionState;

/// - constructor --------------------------------------------------------------
CMapPartition::CMapPartition() : CModule() {}

/// - destructor ---------------------------------------------------------------
CMapPartition::~CMapPartition() {}

/// - public method ------------------------------------------------------------
const std::string &CMapPartition::getName(void) const { return name; }

/// - public method ------------------------------------------------------------
const std::string &CMapPartition::getFirstSection(void) const
{
   return State[PARTITION_POINTS];
}

/// - public method ------------------------------------------------------------
std::vector<std::string> &
CMapPartition::getSections(std::vector<std::string> &sections)
{
   for (int i = PARTITION_POINTS; i != PARTITION_NUM; ++i) {
      sections.push_back(State[i]);
   }
   return sections;
}

/// - public method ------------------------------------------------------------
CMapPartition &CMapPartition::operator=(const CMapPartition &mp)
{
   if (this != &mp) {
      points_ = mp.points_;
      regions_ = mp.regions_;
      edgeNodes_ = mp.edgeNodes_;
      simplePolygons_ = mp.simplePolygons_;
      triangles_ = mp.triangles_;
   }
   return *this;
}

/// - public method ------------------------------------------------------------
void CMapPartition::clear(void)
{
   points_.clear();
   regions_.clear();
   edgeNodes_.clear();
   simplePolygons_.clear();
   triangles_.clear();
}

/// - public method ------------------------------------------------------------
void CMapPartition::parse(std::istream &in, std::string &line)
{
   double x, y;
   int label;
   int v1, v2, v3;
   TRegionState regionState = REGION_LABEL;
   clear();
   Indexes indexes;

   crl::assert_argument(
       line == getFirstSection(),
       "CMapPartition must be initialized with the first section '" +
           getFirstSection() + "'");
   TSections section = getSection(line);

   while (in and section != NONE) {
      switch (section) {
      case PARTITION_NUM:
         if (in >> line) {
            section = getSection(line);
            regionState = REGION_LABEL;
            if (section != NONE) {
               line = "";
            }
         }
         break;
      case PARTITION_POINTS: // read map points
         if (in >> label >> x >> y) {
            crl::assert_argument(
                label == points_.size(),
                "PARTITION_POINTS labels must be increasing (starting from 0)");
            points_.push_back(Point(x, y, label));
	 } else {
            in.clear();
            section = PARTITION_NUM;
         }
         break;
      case PARTITION_REGION:
         switch (regionState) {
         case REGION_LABEL:
            if (in >> line >> label) {
               crl::assert_argument(
                   line == "REGION_LABEL",
                   "REGION_LABEL expected for the PARTITION_REGION");
               crl::assert_argument(
                   label == regions_.size(),
                   "Region labels must be increasing (starting from 0)");
               regionState = REGION_DATA;
            }
            break;
         case REGION_DATA:
            if (in >> label) {
               crl::assert_argument(label >= 0 and label < points_.size(),
                                    "PARTITION_REGION region indexes must "
                                    "correspond with PARTITION_POINTS");
               indexes.push_back(label);
	    } else {
               if (indexes.size() > 0) {
                  regions_.push_back(indexes);
                  indexes.clear();
                  in.clear();
                  section = PARTITION_NUM;
               }
               regionState = REGION_LABEL;
            }
            break;
         } // end regionState switch
         break;
      case PARTITION_POINTS_ON_MAP_EDGES:
         if (in >> label >> v1 >> v2) {
            crl::assert_argument(
                (label >= 0 and label < points_.size()) and v1 >= 0 and
                    v1 < points_.size() and v2 >= 0 and v2 < points_.size(),
                "Nodes labels must correspond to the PARTITION_POINTS");
            edgeNodes_.push_back(EdgeNode(label, v1, v2));
	 } else {
            in.clear();
            section = PARTITION_NUM;
         }
         break;
      case PARTITION_SIMPLE_POLYGONS_WITH_GATES:
         if (in >> label) {
            crl::assert_argument(
                label >= 0 and label < regions_.size(),
                "A region label in PARTITION_SIMPLE_POLYGONS_WITH_GATES must "
                "correspond with the number of PARTITION_REGIONs");
            simplePolygons_.push_back(label);
	 } else {
            in.clear();
            section = PARTITION_NUM;
         }
         break;
      case PARTITION_TRIANGLES:
         if (in >> label) {
            crl::assert_argument(label >= 0 and label < regions_.size(),
                                 "A region label in PARTITION_TRIANGLES must "
                                 "correspond with the number of "
                                 "PARTITION_REGIONs");
            triangles_.push_back(label);
	 } else {
            in.clear();
            section = PARTITION_NUM;
         }
         break;
      case NONE:
         // nothing to do
         break;
      } // end switch state
   }    // end while loop
}

/// - public method ------------------------------------------------------------
void CMapPartition::print(std::ostream &out) const
{
   if (!points_.empty()) { // points_ is the mandatory section
      out << State[PARTITION_POINTS] << std::endl;
      for (Points::const_iterator it = points_.begin(); it != points_.end();
           it++) {
         out << it->label << " " << it->x << " " << it->y << std::endl;
      }
      out << std::endl;
      if (!regions_.empty()) {
         int i = 0;
         for (IndexesList::const_iterator rIt = regions_.begin();
              rIt != regions_.end(); rIt++, i++) {
            out << State[PARTITION_REGION] << std::endl;
            out << "REGION_LABEL"
                << " " << i << std::endl;
            for (Indexes::const_iterator it = rIt->begin(); it != rIt->end();
                 it++) {
               out << *it << std::endl;
            }
            out << std::endl;
         } // end all regions
      }
      if (!edgeNodes_.empty()) {
         out << State[PARTITION_POINTS_ON_MAP_EDGES] << std::endl;
         for (EdgeNodeVector::const_iterator it = edgeNodes_.begin();
              it != edgeNodes_.end(); it++) {
            out << it->label << " " << it->v1 << " " << it->v2 << std::endl;
         }
         out << std::endl;
      }
      if (!simplePolygons_.empty()) {
         out << State[PARTITION_SIMPLE_POLYGONS_WITH_GATES] << std::endl;
         for (Indexes::const_iterator it = simplePolygons_.begin();
              it != simplePolygons_.end(); it++) {
            out << *it << std::endl;
         }
         out << std::endl;
      }
      if (!triangles_.empty()) {
         out << State[PARTITION_TRIANGLES] << std::endl;
         for (Indexes::const_iterator it = triangles_.begin();
              it != triangles_.end(); it++) {
            out << *it << std::endl;
         }
         out << std::endl;
      }
   }
}

/// - public method ------------------------------------------------------------
void CMapPartition::fill(const CModule *module)
{
   const CMapPartition *m = dynamic_cast<const CMapPartition *>(module);
   if (m) {
      *this = *m;
   }
}

/// - private method -----------------------------------------------------------
CMapPartition::TSections
CMapPartition::getSection(const std::string &name) const
{
   TSections ret = NONE;
   for (int i = 0; i < PARTITION_NUM; i++) {
      if (State[i] == name) {
         ret = TSections(i);
         break;
      }
   }
   return ret;
}

/* end of map_partition_module.cc */
