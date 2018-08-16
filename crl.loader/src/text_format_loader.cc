/*
 * File name: text_format_loader.cc
 * Date:      2008/03/03 09:40
 * Author:    Jan Faigl
 */

#include <cassert> //TODO use crl_assert
#include <cmath>
#include <cstring>
#include <iomanip>

#include <crl/assert.h>
#include <crl/file_utils.h>
#include <crl/logging.h>
#include <crl/stringconversions.h>
//#include "text_stream.h"

#include <crl/loader/text_format_loader.h>

using namespace crl::loader;

/// ----------------------------------------------------------------------------
const std::string LOADER_VERSION = "1.0";
const int PRECISION = 14;
typedef std::ifstream Stream;
// typedef CTextStream Stream;

/// ----------------------------------------------------------------------------
/// Class CTriangularMesh
/// ----------------------------------------------------------------------------
CTriangularMesh::CTriangularMesh(const Points &map_points)
    : map_points(map_points)
{
}

/// ----------------------------------------------------------------------------
CTriangularMesh::~CTriangularMesh() { releaseNodes(); }

/// ----------------------------------------------------------------------------
CTriangularMesh &CTriangularMesh::operator=(const CTriangularMesh &other)
{
   if (this != &other) {
      triangles_ = other.triangles_;
      releaseNodes();
      for (unsigned int i = 0; i < map_points.size();
           i++) { // this->map_points should be already set
         addMapNode(i);
      }
      mesh_points.clear();
      for (unsigned int i = 0; i < other.mesh_points.size(); i++) {
         const Point *pt(other.mesh_points[i]);
         Node *node(other.nodes[i + map_points.size()]);
         addNode(pt->x, pt->y, pt->label, node->type);
         nodes.back()->source = node->source;
         nodes.back()->target = node->target;
      }
   }
   return *this;
}

/// ----------------------------------------------------------------------------
void CTriangularMesh::CTriangularMesh::addMapNode(int label)
{
   assert(label >= 0 && label < map_points.size());
   assert(label == nodes.size());
   nodes.push_back(new Node(map_points[label]));
}

/// ----------------------------------------------------------------------------
void CTriangularMesh::addMapNode(const Point &pt)
{
   assert(pt.label >= 0 && pt.label < map_points.size() &&
          pt.label == map_points[pt.label].label);
   assert(map_points[pt.label].compare(pt));
   addMapNode(pt.label);
}

/// ----------------------------------------------------------------------------
void CTriangularMesh::CTriangularMesh::addNode(double x, double y, int label,
                                               TNodeType type)
{
   assert_argument(label >= 0 &&
                       label == (map_points.size() + mesh_points.size()),
                   "Unexpected label '" + crl::string_format<int>(label) + "'");
   mesh_points.push_back(new Point(x, y, label));
   nodes.push_back(new Node(mesh_points.back(), type));
}

/// ----------------------------------------------------------------------------
void CTriangularMesh::setEdge(int label, int s, int t)
{
   assert(label >= 0 && label < nodes.size());
   assert(s >= 0 && s < map_points.size() && t >= 0 && t < map_points.size());
   nodes[label]->setEdge(s, t);
}

/// ----------------------------------------------------------------------------
void CTriangularMesh::addTriangle(int v1, int v2, int v3)
{
   assert(v1 >= 0 && v2 >= 0 && v3 >= 0);
   assert(v1 < (map_points.size() + mesh_points.size()) &&
          v2 < (map_points.size() + mesh_points.size()) &&
          v3 < (map_points.size() + mesh_points.size()));
   triangles_.push_back(Triangle(v1, v2, v3));
}

/// ----------------------------------------------------------------------------
const CTriangularMesh::Node &CTriangularMesh::operator[](int i) const
{
   assert(i >= 0 && i < nodes.size());
   return *nodes[i];
}

/// ----------------------------------------------------------------------------
void CTriangularMesh::clear(void)
{
   releaseNodes();
   mesh_points.clear();
   triangles_.clear();
}

/// - protected method ---------------------------------------------------------
void CTriangularMesh::releaseNodes(void)
{
   for (NodePtrVector::iterator it = nodes.begin(); it != nodes.end(); it++) {
      delete *it;
   }
   for (PointPtrVector::iterator it = mesh_points.begin();
        it != mesh_points.end(); it++) {
      delete *it;
   }
   nodes.clear();
   mesh_points.clear();
}

/// ----------------------------------------------------------------------------
typedef enum {
   INFO,
   MAP_POINTS,
   MAP_BORDER,
   MAP_OBSTACLE,
   CITY_POINTS,
   MAP_VISIBILITY_GRAPH,
   CITY_VISIBILITY,
   MAP_CONVEX_REGION,
   MAP_CONVEX_REGION_CITIES,
   POINTS,
   MAP_VISIBILITY_REGION,
   MESH_NODES,
   MESH_TRIANGLES,
   MESH_BOUNDARY_NODES,
   MAP_VERTEX_LINESTRING,
   TRIMESH_CONVEX_REGION,
   TRIMESH_CONVEX_REGION_TRIANGLES,
   TRIMESH_CONVEX_REGION_BORDER_POINTS,
   TRIMESH_CONVEX_REGION_BORDER,
   NONE
} TLoadState;

/// ----------------------------------------------------------------------------
typedef enum {
   MAP_VIS_GRAPH_CITY_VERTEX,
   MAP_VIS_GRAPH_MAP_VERTEX, // ma
   MAP_VIS_GRAPH_NONE
} TMapVisGraphState;

/// ----------------------------------------------------------------------------
typedef enum { REGION_LABEL, REGION_DATA } TRegionState;

/// ----------------------------------------------------------------------------
typedef enum { LINESTRING_LABEL, LINESTRING_DATA } TLineStringState;

/// ----------------------------------------------------------------------------
std::string LoadStateStr[] = {
    "[INFO]",
    "[MAP_POINTS]",
    "[MAP_BORDER]",
    "[MAP_OBSTACLE]",
    "[CITY_POINTS]",
    "[MAP_VISIBILITY_GRAPH]",
    "[CITY_VISIBILITY]",
    "[MAP_CONVEX_REGION]",
    "[MAP_CONVEX_REGION_CITIES]",
    "[POINTS]",
    "[MAP_VISIBILITY_REGION]",
    "[MESH_NODES]",
    "[MESH_TRIANGLES]",
    "[MESH_BOUNDARY_NODES]",
    "[MAP_VERTEX_LINESTRING]",
    "[TRIMESH_CONVEX_REGION]",
    "[TRIMESH_CONVEX_REGION_TRIANGLES]",
    "[TRIMESH_CONVEX_REGION_BORDER_POINTS]",
    "[TRIMESH_CONVEX_REGION_BORDER]",
};

/// ----------------------------------------------------------------------------
std::string MapVisGraphState[] = {"CITY_VERTEX", "MAP_VERTEX"};

/// ----------------------------------------------------------------------------
TLoadState getState(std::string &name)
{
   TLoadState ret = NONE;
   for (int i = 0; i < NONE; i++) {
      if (LoadStateStr[i] == name) {
         ret = TLoadState(i);
         break;
      }
   }
   return ret;
}

/// ----------------------------------------------------------------------------
TMapVisGraphState getMapVisGraphState(std::string &name)
{
   TMapVisGraphState ret = MAP_VIS_GRAPH_NONE;
   for (int i = 0; i < MAP_VIS_GRAPH_NONE; i++) {
      if (MapVisGraphState[i] == name) {
         ret = TMapVisGraphState(i);
         break;
      }
   }
   return ret;
}

/// ----------------------------------------------------------------------------
void new_state(TLoadState &state, TLoadState &prevState, TLoadState newState)
{
   prevState = state;
   state = newState;
}

/// ----------------------------------------------------------------------------
/// @brief adding_point_label parse label from in stream, label value is checked
///                           if it is >= 0 and < maxLabel, than it is added to
///			      list, if label can not be parse from stream list
///                           is added into indexesList
///
///
/// @param in
/// @param state
/// @param maxLabel
/// @param indexes
/// @param indexesList
///
/// ----------------------------------------------------------------------------
void adding_point_label(Stream &in, TLoadState &state, int maxLabel,
                        CTextFormatLoader::Indexes &indexes,
                        CTextFormatLoader::IndexesList *indexesList)
{
   int label;
   if (in >> label) {
      assert(label >= 0 && label < maxLabel);
      indexes.push_back(label);
   } else {
      if (indexes.size() > 0 && indexesList != 0) {
         indexesList->push_back(indexes);
         indexes.clear();
      }
      in.clear();
      state = NONE;
   }
}

/// ----------------------------------------------------------------------------
/// @brief adding_point_labelDirect parse label from in stream, label is checked
///			            if value is >= 0 and < maxLabel, than it is
///                                 added into indexList at indexNumber position
///				    in indexesList
///
/// @param in
/// @param state
/// @param maxLabel
/// @param indexNumber
/// @param indexesList
///
/// @return
/// ----------------------------------------------------------------------------
void adding_point_labelDirect(Stream &in, TLoadState &state, int maxLabel,
                              unsigned int indexNumber,
                              CTextFormatLoader::IndexesList &indexesList)
{
   int label;
   if (in >> label) {
      assert(label >= 0 && label < maxLabel);
      assert(indexNumber < indexesList.size());
      indexesList[indexNumber].push_back(label);
   } else {
      in.clear();
      state = NONE;
   }
}

/// ----------------------------------------------------------------------------
/// Class CTextFormatLoader::SInfo
/// ----------------------------------------------------------------------------
std::string CTextFormatLoader::SInfo::INFO[] = {"FORMAT", "LOADER_VERSION",
                                                "COMMENT", ""};

/// ----------------------------------------------------------------------------
std::string CTextFormatLoader::SInfo::FORMATS[] = {"MAP_POINTS",
                                                   "MAP_POINTS_CITY_POINTS",
                                                   "MAP_CITY_POINTS",
                                                   "CELL_MAP",
                                                   "VORONOI_DIAGRAM",
                                                   "TRIANGULAR_MESH",
                                                   ""};

/// ----------------------------------------------------------------------------
CTextFormatLoader::SInfo::SInfo() : format(MAP_POINTS), version(LOADER_VERSION)
{
}

/// ----------------------------------------------------------------------------
void CTextFormatLoader::SInfo::createLines(void)
{
   lines.clear();
   lines.push_back("FORMAT=" + FORMATS[format]);
   lines.push_back("NAME=" + name);
   lines.push_back("LOADER_VERSION=" + version);
   for (StringList::iterator it = comments.begin(); it != comments.end();
        it++) {
      lines.push_back("COMMENT=" + *it);
   }
   for (StringList::iterator it = modules.begin(); it != modules.end(); it++) {
      lines.push_back("MODULE=" + *it);
   }
}

/// ----------------------------------------------------------------------------
void CTextFormatLoader::SInfo::parseLines(void)
{
   const std::string delims("=");
   for (StringList::iterator it = lines.begin(); it != lines.end(); it++) {
      std::string::size_type pos = it->find("=");
      assert(pos != std::string::npos);
      std::string name = it->substr(0, pos);
      std::string value = (pos + 1) < it->size()
                              ? it->substr(it->find_first_not_of(delims, pos))
                              : "";
      if (name == "FORMAT") {
         int i = 0;
         bool formatOk = false;
         while (FORMATS[i].size() > 0) {
            if (value == FORMATS[i]) {
               formatOk = true;
               format = (SInfo::TFormat)i;
               break;
            }
            i++;
         }
         assert(formatOk);
      } else if (name == "NAME") {
         filename = value;
      } else if (name == "LOADER_VERSION") {
         version = value;
      } else if (name == "COMMENT") {
         comments.push_back(value);
      } else if (name == "MODULE") {
         modules.push_back(value);
      }
   }
}

/// ----------------------------------------------------------------------------
void CTextFormatLoader::SInfo::addComment(const std::string &comment)
{
   bool already = false;
   for (StringList::iterator it = comments.begin(); it != comments.end();
        it++) {
      if (*it == comment) {
         already = true;
         break;
      }
   }
   if (!already) {
      comments.push_back(comment);
   }
}

/// ----------------------------------------------------------------------------
CTextFormatLoader::SInfo &CTextFormatLoader::SInfo::
operator=(const SInfo &other)
{
   if (this != &other) {
      name = other.name;
      filename = other.filename;
      version = other.version;
      comments = other.comments;
      lines = other.lines;
      format = other.format;
   }
   return *this;
}

/// ----------------------------------------------------------------------------
/// Class CTextFormatLoader

/// - static method ------------------------------------------------------------
int CTextFormatLoader::getPrecision(void) { return PRECISION; }

/// ----------------------------------------------------------------------------
CTextFormatLoader::CTextFormatLoader() : triangular_mesh(map_points) {}

/// ----------------------------------------------------------------------------
CTextFormatLoader::CTextFormatLoader(const CTextFormatLoader &other)
    : triangular_mesh(map_points)
{
   *this = other;
}

/// ----------------------------------------------------------------------------
CTextFormatLoader::~CTextFormatLoader()
{
   for (ModuleMap::iterator it = modulesMap.begin(); it != modulesMap.end();
        it++) {
      delete it->second;
   }
}

/// ----------------------------------------------------------------------------
void CTextFormatLoader::clear()
{
   map_points.clear();
   city_points.clear();
   map_border.clear();
   map_obstacles.clear();
   map_convex_regions.clear();
   map_vertices_visibility_graphs.clear();
   city_map_visibility_graphs.clear();
   region_cities.clear();
   city_vertex_visible_cities.clear();
   map_vertex_visible_cities.clear();
   map_linestrings.clear();
   triangular_mesh.clear();
   trimesh_convex_hulls.clear();
   trimesh_convex_hull_triangles.clear();
   trimesh_convex_border_points.clear();
   trimesh_convex_borders.clear();
   for (StringList::const_iterator it = registeredModules.begin();
        it != registeredModules.end(); it++) {
      ModuleMap::iterator mIt = modulesMap.find(*it);
      crl::assert_argument(mIt != modulesMap.end(),
                           "Inconsistency in registered modules");
      mIt->second->clear();
   }
}

/// ----------------------------------------------------------------------------
void CTextFormatLoader::load(const std::string &filename)
{
   Stream in(filename.c_str());
   double x, y;
   int label;
   int vis_origin;
   int v1, v2, v3;
   std::string line;
   int maxPointLabel = -1;
   TLoadState state = NONE;
   TMapVisGraphState mapVisGraphState = MAP_VIS_GRAPH_NONE;
   TRegionState regionState = REGION_LABEL;
   TLineStringState lineStringState = LINESTRING_LABEL;
   clear();
   Indexes indexes;
   Indexes emptyList;

   while (in) {
      switch (state) {
      case NONE:
         if (in >> line and !line.empty()) {
            state = getState(line);
            mapVisGraphState = MAP_VIS_GRAPH_NONE;
            regionState = REGION_LABEL;
            lineStringState = LINESTRING_LABEL;
            while (
                state == NONE and
                !line.empty()) { // try find section in the registered modules
               ModuleMap::iterator mIt = firstSectionMap.find(line);
               if (mIt != firstSectionMap.end()) { // use the module parser
                  mIt->second->parse(in, line);
                  state = getState(line); // next section
	       } else { // section is
                  WARN("Section: '"
                       << line
                       << "' does not match with registered modules nor "
                          "standard sections, it is skipped");
                  break;
               }
               in.clear();
            }
         }
         break;
      case INFO:
         if (getline(in, line)) { // first end of line from the [INFO]
            while (getline(in, line) and !line.empty()) { // parse next line
               info.lines.push_back(line);
            } // getline loop until empty line is read
         }
         state = NONE;
         break;
      case MAP_POINTS: // read map points
         if (in >> label >> x >> y) {
            assert(label == map_points.size());
            map_points.push_back(Point(x, y, label));
	 } else {
            in.clear();
            state = NONE;
         }
         break;
      case MAP_BORDER: // read border indexes
         adding_point_label(in, state, map_points.size(), map_border, 0);
         break;
      case MAP_OBSTACLE: // read obstacle indexes
         adding_point_label(in, state, map_points.size(), indexes,
                            &map_obstacles);
         break;
      case CITY_POINTS:
         if (in >> label >> x >> y) {
            assert(label == city_points.size());
            city_points.push_back(Point(x, y, label));
	 } else {
            in.clear();
            state = NONE;
         }
         break;
      case POINTS: // read points
         if (in >> label >> x >> y) {
            assert(label == points.size());
            points.push_back(Point(x, y, label));
            if (label >= maxPointLabel) {
               maxPointLabel = label + 1;
            }
	 } else {
            in.clear();
            state = NONE;
         }
         break;
      case MAP_VISIBILITY_GRAPH: // TODO
         switch (mapVisGraphState) {
         case MAP_VIS_GRAPH_MAP_VERTEX:
            adding_point_labelDirect(in, state, map_points.size(), vis_origin,
                                     map_vertices_visibility_graphs);
            break;
         case MAP_VIS_GRAPH_CITY_VERTEX:
            adding_point_labelDirect(in, state, map_points.size(), vis_origin,
                                     city_map_visibility_graphs);
            break;
         case MAP_VIS_GRAPH_NONE:
            if (in >> line >> vis_origin) {
               mapVisGraphState = getMapVisGraphState(line);
               switch (mapVisGraphState) {
               case MAP_VIS_GRAPH_MAP_VERTEX:
                  assert(vis_origin >= 0 && vis_origin < map_points.size());
                  map_vertices_visibility_graphs.push_back(emptyList);
                  break;
               case MAP_VIS_GRAPH_CITY_VERTEX:
                  assert(vis_origin >= 0 && vis_origin < city_points.size());
                  city_map_visibility_graphs.push_back(emptyList);
                  break;
               case MAP_VIS_GRAPH_NONE:
                  throw std::invalid_argument(
                      "Wrong vertex origin in MAP_VISIBILITY_GRAPH section");
                  break;
               }
	    } else {
               throw std::invalid_argument(
                   "Wrong format of MAP_VISIBILITY_GRAPH section");
            }
            break;
         }
         break;

      case MAP_VISIBILITY_REGION:
         switch (mapVisGraphState) {
         case MAP_VIS_GRAPH_MAP_VERTEX:
            adding_point_labelDirect(in, state, maxPointLabel, vis_origin,
                                     map_vertices_visibility_regions);
            break;
         case MAP_VIS_GRAPH_CITY_VERTEX:
            adding_point_labelDirect(in, state, maxPointLabel, vis_origin,
                                     city_map_visibility_regions);
            break;
         case MAP_VIS_GRAPH_NONE:
            if (in >> line >> vis_origin) {
               mapVisGraphState = getMapVisGraphState(line);
               switch (mapVisGraphState) {
               case MAP_VIS_GRAPH_MAP_VERTEX:
                  assert(vis_origin >= 0 && vis_origin < map_points.size());
                  map_vertices_visibility_regions.push_back(emptyList);
                  break;
               case MAP_VIS_GRAPH_CITY_VERTEX:
                  assert(vis_origin >= 0 && vis_origin < city_points.size());
                  city_map_visibility_regions.push_back(emptyList);
                  break;
               case MAP_VIS_GRAPH_NONE:
                  throw std::invalid_argument(
                      "Wrong vertex origin in MAP_VISIBILITY_REGION section");
                  break;
               }
	    } else {
               throw std::invalid_argument(
                   "Wrong format of MAP_VISIBILITY_REGION section");
            }
            break;
         }
         break;

      case CITY_VISIBILITY:
         switch (mapVisGraphState) {
         case MAP_VIS_GRAPH_MAP_VERTEX:
            adding_point_labelDirect(in, state, city_points.size(), vis_origin,
                                     map_vertex_visible_cities);
            break;
         case MAP_VIS_GRAPH_CITY_VERTEX:
            adding_point_labelDirect(in, state, city_points.size(), vis_origin,
                                     city_vertex_visible_cities);
            break;
         case MAP_VIS_GRAPH_NONE:
            if (in >> line >> vis_origin) {
               mapVisGraphState = getMapVisGraphState(line);
               switch (mapVisGraphState) {
               case MAP_VIS_GRAPH_MAP_VERTEX:
                  assert(vis_origin >= 0 && vis_origin < map_points.size());
                  map_vertex_visible_cities.push_back(emptyList);
                  break;
               case MAP_VIS_GRAPH_CITY_VERTEX:
                  assert(vis_origin >= 0 && vis_origin < city_points.size());
                  city_vertex_visible_cities.push_back(emptyList);
                  break;
               case MAP_VIS_GRAPH_NONE:
                  throw std::invalid_argument(
                      "Wrong vertex origin in CITY_VISIBILITY section");
                  break;
               }
	    } else {
               throw std::invalid_argument(
                   "Wrong format of CITY_VISIBILITY section");
            }
         }
         break;
      case MAP_CONVEX_REGION:
         switch (regionState) {
         case REGION_LABEL:
            if (in >> line >> label) {
               assert(line == "REGION_LABEL");
               assert(label == map_convex_regions.size());
               regionState = REGION_DATA;
            }
            break;
         case REGION_DATA:
            if (in >> label) {
               assert(label >= 0 && label < map_points.size());
               indexes.push_back(label);
	    } else {
               if (indexes.size() > 0) {
                  map_convex_regions.push_back(indexes);
                  indexes.clear();
                  in.clear();
                  state = NONE;
               }
               regionState = REGION_LABEL;
            }
            break;
         } // end regionState switch
         break;
      case MAP_CONVEX_REGION_CITIES:
         throw std::invalid_argument(
             "MAP_CONVEX_REGION_CITIES not implemented");
         // TODO
         break;
      case MESH_NODES:
         if (in >> label >> x >> y) {
            assert(label == triangular_mesh.size());
            if (label < map_points.size()) { // map node
               assert(map_points[label].compare(Point(x, y)));
               triangular_mesh.addMapNode(label);
	    } else {
               triangular_mesh.addNode(x, y, label);
            }
	 } else {
            in.clear();
            state = NONE;
         }
         break;
      case MESH_TRIANGLES:
         if (in >> label >> v1 >> v2 >> v3) {
            assert(label == triangular_mesh.triangles().size());
            triangular_mesh.addTriangle(v1, v2, v3);
	 } else {
            in.clear();
            state = NONE;
         }
         break;
      case MESH_BOUNDARY_NODES:
         if (in >> label >> v1 >> v2) {
            triangular_mesh.setEdge(label, v1, v2);
	 } else {
            in.clear();
            state = NONE;
         }
         break;
      case MAP_VERTEX_LINESTRING:
         switch (lineStringState) {
         case LINESTRING_LABEL:
            if (in >> line >> label) {
               assert(line == "LINESTRING_LABEL");
               assert(label == map_linestrings.size());
               lineStringState = LINESTRING_DATA;
            }
            break;
         case LINESTRING_DATA:
            if (in >> label) {
               assert(label >= 0 && label < map_points.size());
               indexes.push_back(label);
	    } else {
               if (indexes.size() > 0) {
                  map_linestrings.push_back(indexes);
                  indexes.clear();
                  in.clear();
                  state = NONE;
               }
               lineStringState = LINESTRING_LABEL;
            }
            break;
         } // end lineStringState switch
         break;
      case TRIMESH_CONVEX_REGION:
         switch (regionState) {
         case REGION_LABEL:
            if (in >> line >> label) {
               assert(line == "REGION_LABEL");
               assert(label == trimesh_convex_hulls.size());
               regionState = REGION_DATA;
            }
            break;
         case REGION_DATA:
            if (in >> label) {
               assert(label >= 0 && label < triangular_mesh.getNodes().size());
               indexes.push_back(label);
	    } else {
               if (!indexes.empty()) {
                  trimesh_convex_hulls.push_back(indexes);
                  indexes.clear();
                  in.clear();
                  state = NONE;
               }
               regionState = REGION_LABEL;
            }
            break;
         } // end regionState switch
         break;
      case TRIMESH_CONVEX_REGION_TRIANGLES:
         switch (regionState) {
         case REGION_LABEL:
            if (in >> line >> label) {
               assert(line == "REGION_LABEL");
               assert(label >= 0 && label < trimesh_convex_hulls.size());
               regionState = REGION_DATA;
            }
            break;
         case REGION_DATA:
            if (in >> label) {
               assert(label >= 0 && label < triangular_mesh.triangles().size());
               indexes.push_back(label);
	    } else {
               if (!indexes.empty()) {
                  trimesh_convex_hull_triangles.push_back(indexes);
                  indexes.clear();
                  in.clear();
                  state = NONE;
               }
               regionState = REGION_LABEL;
            }
            break;
         } // end regionState switch
         break;
      case TRIMESH_CONVEX_REGION_BORDER_POINTS: // read trimesh convex region
                                                // border point
         if (in >> label >> x >> y) {
            assert(label == trimesh_convex_border_points.size());
            trimesh_convex_border_points.push_back(Point(x, y, label));
	 } else {
            in.clear();
            state = NONE;
         }
         break;
      case TRIMESH_CONVEX_REGION_BORDER:
         switch (regionState) {
         case REGION_LABEL:
            if (in >> line >> label) {
               assert(line == "REGION_LABEL");
               assert(label >= 0 && label < trimesh_convex_hulls.size());
               regionState = REGION_DATA;
            }
            break;
         case REGION_DATA:
            if (in >> label) {
               assert(label >= 0 &&
                      label < trimesh_convex_border_points.size());
               indexes.push_back(label);
	    } else {
               if (!indexes.empty()) {
                  trimesh_convex_borders.push_back(indexes);
                  indexes.clear();
                  in.clear();
                  state = NONE;
               }
               regionState = REGION_LABEL;
            }
            break;
         } // end regionState switch
         break;

      } // end switch state
   }    // end while loop
   info.parseLines();
   // check list of modules in the info
   for (int i = 0; i < info.modules.size(); i++) {
      bool found = false;
      for (int j = 0; j < registeredModules.size(); j++) {
         if (info.modules[i] == registeredModules[j]) {
            found = true;
            break;
         }
      }
      if (!found) {
         //        WARN("Module '" << info.modules[i] << "' is not registered in
         //        the loader");
      }
   }
   if (!registeredModules.empty()) {
      //   DEBUG("Force modules in [INFO] section");
      info.modules.clear();
      for (int j = 0; j < registeredModules.size(); j++) {
         info.modules.push_back(registeredModules[j]);
      }
      info.createLines();
   }
}

/// ----------------------------------------------------------------------------
bool CTextFormatLoader::save(const std::string &filename)
{
   std::fstream out(filename.c_str(), std::ios::out | std::ios_base::trunc);
   print(out);
   out.close();
   return not out.fail();
}

/// ----------------------------------------------------------------------------
void CTextFormatLoader::print(std::ostream &out)
{
   if (info.lines.size() > 0) {
      out << LoadStateStr[INFO] << std::endl;
      for (StringList::iterator it = info.lines.begin(); it != info.lines.end();
           it++) {
         out << *it << std::endl;
      }
      out << std::endl;
   }

   if (map_points.size() > 0) {
      out << std::setprecision(PRECISION);
      out << LoadStateStr[MAP_POINTS] << std::endl;
      for (Points::iterator it = map_points.begin(); it != map_points.end();
           it++) {
         out << it->label << " " << it->x << " " << it->y << std::endl;
      }
      if (map_border.size() > 0) {
         out << std::endl;
         out << LoadStateStr[MAP_BORDER] << std::endl;
         for (Indexes::iterator it = map_border.begin(); it != map_border.end();
              it++) {
            out << *it << std::endl;
         }
      }
      for (IndexesList::iterator oIt = map_obstacles.begin();
           oIt != map_obstacles.end(); oIt++) {
         out << std::endl;
         out << LoadStateStr[MAP_OBSTACLE] << std::endl;
         for (Indexes::iterator it = oIt->begin(); it != oIt->end(); it++) {
            out << *it << std::endl;
         }
      }
      out << std::endl;
   }

   if (city_points.size() > 0) {
      out << LoadStateStr[CITY_POINTS] << std::endl;
      for (Points::iterator it = city_points.begin(); it != city_points.end();
           it++) {
         out << it->label << " " << it->x << " " << it->y << std::endl;
      }
      out << std::endl;
   }

   if (map_convex_regions.size() > 0) {
      int i = 0;
      for (IndexesList::iterator rIt = map_convex_regions.begin();
           rIt != map_convex_regions.end(); rIt++, i++) {
         out << std::endl;
         out << LoadStateStr[MAP_CONVEX_REGION] << std::endl;
         out << "REGION_LABEL"
             << " " << i << std::endl;
         for (Indexes::iterator it = rIt->begin(); it != rIt->end(); it++) {
            out << *it << std::endl;
         }
      }
      out << std::endl;
   }

   if (map_vertices_visibility_graphs.size() > 0) {
      int i = 0;
      for (IndexesList::iterator rIt = map_vertices_visibility_graphs.begin();
           rIt != map_vertices_visibility_graphs.end(); rIt++, i++) {
         out << LoadStateStr[MAP_VISIBILITY_GRAPH] << std::endl;
         out << MapVisGraphState[MAP_VIS_GRAPH_MAP_VERTEX] << " " << i
             << std::endl;
         for (Indexes::iterator it = rIt->begin(); it != rIt->end(); it++) {
            out << *it << std::endl;
         }
         out << std::endl;
      }
   }

   if (city_map_visibility_graphs.size() > 0) {
      int i = 0;
      for (IndexesList::iterator rIt = city_map_visibility_graphs.begin();
           rIt != city_map_visibility_graphs.end(); rIt++, i++) {
         out << LoadStateStr[MAP_VISIBILITY_GRAPH] << std::endl;
         out << MapVisGraphState[MAP_VIS_GRAPH_CITY_VERTEX] << " " << i
             << std::endl;
         for (Indexes::iterator it = rIt->begin(); it != rIt->end(); it++) {
            out << *it << std::endl;
         }
         out << std::endl;
      }
   }

   if (city_vertex_visible_cities.size() > 0) {
      assert(city_vertex_visible_cities.size() == city_points.size());
      int i = 0;
      for (IndexesList::iterator rIt = city_vertex_visible_cities.begin();
           rIt != city_vertex_visible_cities.end(); rIt++, i++) {
         out << LoadStateStr[CITY_VISIBILITY] << std::endl;
         out << MapVisGraphState[MAP_VIS_GRAPH_CITY_VERTEX] << " " << i
             << std::endl;
         for (Indexes::iterator it = rIt->begin(); it != rIt->end(); it++) {
            out << *it << std::endl;
         }
         out << std::endl;
      }
   }

   if (points.size() > 0) {
      out << LoadStateStr[POINTS] << std::endl;
      for (Points::iterator it = points.begin(); it != points.end(); it++) {
         out << it->label << " " << it->x << " " << it->y << std::endl;
      }
      out << std::endl;
   }

   if (map_vertices_visibility_regions.size() > 0) {
      int i = 0;
      for (IndexesList::iterator rIt = map_vertices_visibility_regions.begin();
           rIt != map_vertices_visibility_regions.end(); rIt++, i++) {
         out << std::endl;
         out << LoadStateStr[MAP_VISIBILITY_REGION] << std::endl;
         out << MapVisGraphState[MAP_VIS_GRAPH_MAP_VERTEX] << " " << i
             << std::endl;
         for (Indexes::iterator it = rIt->begin(); it != rIt->end(); it++) {
            out << *it << std::endl;
         }
         out << std::endl;
      }
   }

   if (city_map_visibility_regions.size() > 0) {
      int i = 0;
      for (IndexesList::iterator rIt = city_map_visibility_regions.begin();
           rIt != city_map_visibility_regions.end(); rIt++, i++) {
         out << std::endl;
         out << LoadStateStr[MAP_VISIBILITY_REGION] << std::endl;
         out << MapVisGraphState[MAP_VIS_GRAPH_CITY_VERTEX] << " " << i
             << std::endl;
         for (Indexes::iterator it = rIt->begin(); it != rIt->end(); it++) {
            out << *it << std::endl;
         }
         out << std::endl;
      }
   }

   if (!triangular_mesh.empty()) {

      out << LoadStateStr[MESH_NODES] << std::endl;
      for (CTriangularMesh::NodeIterator it = triangular_mesh.nodes_begin();
           it != triangular_mesh.nodes_end(); it++) {
         out << (*it)->point->label << " " << (*it)->point->x << " "
             << (*it)->point->y << std::endl;
      }
      out << std::endl;
      int i = 0;
      out << LoadStateStr[MESH_TRIANGLES] << std::endl;
      for (CTriangularMesh::TriangleIterator
               it = triangular_mesh.triangles_begin();
           it != triangular_mesh.triangles_end(); it++, i++) {
         out << i << " " << it->v[0] << " " << it->v[1] << " " << it->v[2]
             << std::endl;
      }
      out << std::endl;
      out << LoadStateStr[MESH_BOUNDARY_NODES] << std::endl;
      for (CTriangularMesh::NodeIterator it = triangular_mesh.nodes_begin();
           it != triangular_mesh.nodes_end(); it++) {
         if ((*it)->type == CTriangularMesh::ON_EDGE) {
            out << (*it)->point->label << " " << (*it)->source << " "
                << (*it)->target << std::endl;
         }
      }
      out << std::endl;
   }

   if (!trimesh_convex_hulls.empty()) {
      int i = 0;
      for (IndexesList::iterator rIt = trimesh_convex_hulls.begin();
           rIt != trimesh_convex_hulls.end(); rIt++, i++) {
         out << std::endl;
         out << LoadStateStr[TRIMESH_CONVEX_REGION] << std::endl;
         out << "REGION_LABEL"
             << " " << i << std::endl;
         for (Indexes::iterator it = rIt->begin(); it != rIt->end(); it++) {
            out << *it << std::endl;
         }
         out << std::endl;
      }
   }

   if (!trimesh_convex_hull_triangles.empty()) {
      int i = 0;
      for (IndexesList::iterator rIt = trimesh_convex_hull_triangles.begin();
           rIt != trimesh_convex_hull_triangles.end(); rIt++, i++) {
         out << std::endl;
         out << LoadStateStr[TRIMESH_CONVEX_REGION_TRIANGLES] << std::endl;
         out << "REGION_LABEL"
             << " " << i << std::endl;
         for (Indexes::iterator it = rIt->begin(); it != rIt->end(); it++) {
            out << *it << std::endl;
         }
         out << std::endl;
      }
   }

   if (!trimesh_convex_border_points.empty()) {
      out << LoadStateStr[TRIMESH_CONVEX_REGION_BORDER_POINTS] << std::endl;
      for (Points::iterator it = trimesh_convex_border_points.begin();
           it != trimesh_convex_border_points.end(); it++) {
         out << it->label << " " << it->x << " " << it->y << std::endl;
      }
      out << std::endl;
   }

   if (!trimesh_convex_borders.empty()) {
      assert(trimesh_convex_borders.size() == trimesh_convex_hulls.size());
      int i = 0;
      for (IndexesList::iterator rIt = trimesh_convex_borders.begin();
           rIt != trimesh_convex_borders.end(); rIt++, i++) {
         out << std::endl;
         out << LoadStateStr[TRIMESH_CONVEX_REGION_BORDER] << std::endl;
         out << "REGION_LABEL"
             << " " << i << std::endl;
         for (Indexes::iterator it = rIt->begin(); it != rIt->end(); it++) {
            out << *it << std::endl;
         }
         out << std::endl;
      }
   }

   if (map_linestrings.size() > 0) {
      int i = 0;
      for (IndexesList::iterator rIt = map_linestrings.begin();
           rIt != map_linestrings.end(); rIt++, i++) {
         out << std::endl;
         out << LoadStateStr[MAP_VERTEX_LINESTRING] << std::endl;
         out << "LINESTRING_LABEL"
             << " " << i << std::endl;
         for (Indexes::iterator it = rIt->begin(); it != rIt->end(); it++) {
            out << *it << std::endl;
         }
      }
      out << std::endl;
   }

   for (StringList::const_iterator it = registeredModules.begin();
        it != registeredModules.end(); it++) {
      ModuleMap::iterator mIt = modulesMap.find(*it);
      crl::assert_argument(mIt != modulesMap.end(),
                           "Inconsistency in registered modules");
      mIt->second->print(out);
   }
}

/// ----------------------------------------------------------------------------
CTextFormatLoader &CTextFormatLoader::operator=(const CTextFormatLoader &other)
{
   if (this != &other) {
      info = other.info;
      info.modules = other.info.modules;
      points = other.points;
      map_points.clear();
      map_points = other.map_points;
      city_points = other.city_points;
      map_border = other.map_border;
      map_obstacles = other.map_obstacles;
      map_convex_regions = other.map_convex_regions;
      map_vertices_visibility_graphs = other.map_vertices_visibility_graphs;
      city_map_visibility_graphs = other.city_map_visibility_graphs;
      region_cities = other.region_cities;
      city_vertex_visible_cities = other.city_vertex_visible_cities;
      map_vertex_visible_cities = other.map_vertex_visible_cities;
      map_vertices_visibility_regions = other.map_vertices_visibility_regions;
      city_map_visibility_regions = other.city_map_visibility_regions;
      trimesh_convex_hulls = other.trimesh_convex_hulls;
      trimesh_convex_hull_triangles = other.trimesh_convex_hull_triangles;
      trimesh_convex_border_points = other.trimesh_convex_border_points;
      trimesh_convex_borders = other.trimesh_convex_borders;
      map_linestrings = other.map_linestrings;

      triangular_mesh = other.triangular_mesh;
      // recreate modules
      for (ModuleMap::iterator it = modulesMap.begin(); it != modulesMap.end();
           it++) {
         delete it->second;
      }
      firstSectionMap.clear();
      registeredModules.clear();
      modulesMap.clear();

      for (StringList::const_iterator it = other.registeredModules.begin();
           it != other.registeredModules.end(); it++) {
         const CModule *module = other.modulesMap.find(*it)->second;
         CModule *newModule = module->createInstance();
         newModule->fill(module);
         firstSectionMap[newModule->getFirstSection()] = newModule;
         modulesMap[*it] = newModule;
         registeredModules.push_back(module->getName());
      }
   }
   return *this;
}

/// ----------------------------------------------------------------------------
void CTextFormatLoader::loadSimpleTxt(const std::string &filename)
{
   Stream in(filename.c_str());
   double x, y;
   int label = 0;
   int state = 0;
   std::string line;
   clear();

   Indexes emptyList;
   Indexes *indexes;

   info.format = SInfo::MAP_POINTS;
   info.version = LOADER_VERSION;
   info.name = crl::getBasename(filename);
   info.comments.push_back("Load from simple-txt file '" + filename + "'");
   info.createLines();

   while (in) {
      switch (state) {
      case 0: // load section
         if (in >> line && line.size() > 0 && line.at(0) == '[') {
            if (strcasecmp(line.c_str(), "[border]") == 0) {
               state = 1;
               map_border = emptyList;
               indexes = &map_border;
	    } else { // should be obstacle
               state = 1;
               map_obstacles.push_back(emptyList);
               indexes = &map_obstacles.back();
               ;
            }
         }
         break;
      case 1: // load points
         if (in >> x >> y) {
            map_points.push_back(Point(x, y, label));
            indexes->push_back(label);
            label++;
	 } else {
            state = 0;
            in.clear();
         }
         break;
      } // end switch
   }    // end while
}

/// ----------------------------------------------------------------------------
void CTextFormatLoader::printSimpleTxt(std::ostream &out)
{
   if (map_points.size() > 0) {
      out << std::setprecision(PRECISION);
      if (map_border.size() > 0) {
         out << "[Border]" << std::endl;
         for (Indexes::iterator it = map_border.begin(); it != map_border.end();
              it++) {
            out << map_points[*it].x << " " << map_points[*it].y << std::endl;
         }
      }
      for (IndexesList::iterator oIt = map_obstacles.begin();
           oIt != map_obstacles.end(); oIt++) {
         out << std::endl;
         out << "[Obstacle]" << std::endl;
         for (Indexes::iterator it = oIt->begin(); it != oIt->end(); it++) {
            out << map_points[*it].x << " " << map_points[*it].y << std::endl;
         }
      }
      out << std::endl;
   }
}

/// ----------------------------------------------------------------------------
void CTextFormatLoader::clearCities(void)
{
   city_points.clear();
   city_map_visibility_graphs.clear();
   city_vertex_visible_cities.clear();
   map_vertex_visible_cities.clear();
   city_map_visibility_regions.clear();
}

/// ----------------------------------------------------------------------------
CTextFormatLoader &CTextFormatLoader::addModule(CModule *module)
{
   if (module) {
      crl::assert_argument(firstSectionMap.find(module->getFirstSection()) ==
                               firstSectionMap.end(),
                           "Module '" + module->getName() +
                               "' already registered");
      crl::assert_argument(
          modulesMap.find(module->getName()) == modulesMap.end(),
          "Module '" + module->getName() + "' already registered");
      firstSectionMap[module->getFirstSection()] = module;
      modulesMap[module->getName()] = module;
      registeredModules.push_back(module->getName());
      info.modules.push_back(module->getName());
   }
   return *this;
}

/// ----------------------------------------------------------------------------
/// Static method
/// ----------------------------------------------------------------------------
bool CTextFormatLoader::isTextFormat(const std::string &filename) throw()
{
   std::string token = crl::firstFileToken(filename);
   return token.compare("[INFO]") == 0;
}

/* end of text_format_loader.cc */
