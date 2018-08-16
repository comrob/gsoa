/*
 * File name: text_format_loader.h
 * Date:      2008/03/03 09:26
 * Author:    Jan Faigl
 */

#ifndef __TEXT_FORMAT_LOADER_H__
#define __TEXT_FORMAT_LOADER_H__

#include <fstream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include <crl/assert.h>
#include <crl/exceptions.h>

#include <crl/loader/loader_module.h>
#include <crl/loader/loader_types.h>

namespace crl { namespace loader {

   /// ----------------------------------------------------------------------------
   /// @brief
   /// ----------------------------------------------------------------------------
   class CTriangularMesh {
    public:
      typedef enum {
         MAP,
         ON_EDGE,
         FREESPACE,
         MESH,
         EMPTY
      } TNodeType; // MESH general mesh point
      struct Node {
         const Point *point; // const point is used, as mesh_points can be
                             // modified, hence reference will be invalid
         TNodeType type;
         int source; // for ON_EDGE index of source map point
         int target; // for ON_EDGE index of target map point
         Node(const Point &point)
             : point(&point), type(MAP), source(-1), target(-1)
         {
         }
         Node(const Point *point, TNodeType type)
             : point(point), type(type), source(-1), target(-1)
         {
         }
         void setEdge(int s, int t)
         {
            source = s;
            target = t;
            type = ON_EDGE;
         }
      };

      struct Triangle {
         int v[3];
         Triangle(int v1, int v2, int v3)
         {
            v[0] = v1;
            v[1] = v2;
            v[2] = v3;
         }
         const int operator[](int i) const { return v[i]; }
      };

      typedef std::vector<Node *> NodePtrVector; // pointer* is required as node
                                                 // has const reference member
      typedef std::vector<Triangle> TriangleVector;
      typedef NodePtrVector::iterator NodeIterator;
      typedef TriangleVector::iterator TriangleIterator;

      CTriangularMesh(const Points &map_points);
      ~CTriangularMesh();

      CTriangularMesh &operator=(const CTriangularMesh &other);

      void addMapNode(int label);
      void addMapNode(const Point &pt);
      void addNode(double x, double y, int label, TNodeType type = FREESPACE);

      void setEdge(int label, int s, int t);

      void addTriangle(int v1, int v2, int v3);

      const Node &operator[](int i) const;

      const NodePtrVector getNodes(void) const { return nodes; }

      NodeIterator nodes_begin(void) { return nodes.begin(); }

      NodeIterator nodes_end(void) { return nodes.end(); }

      TriangleIterator triangles_begin(void) { return triangles_.begin(); }

      TriangleIterator triangles_end(void) { return triangles_.end(); }

      int size(void) const { return nodes.size(); }

      bool empty(void) const { return triangles_.empty(); }

      TriangleVector &triangles(void) { return triangles_; }

      const TriangleVector &triangles(void) const { return triangles_; }

      void clear(void);

    protected:
      void releaseNodes(void);

    private:
      const Points &map_points;
      PointPtrVector mesh_points;
      NodePtrVector nodes;
      TriangleVector triangles_;
   };

   /// ----------------------------------------------------------------------------
   /// @brief Loader class to load ascii files with section []
   /// These sections are supported
   /// [INFO]                     - information about version, followed by
   /// string lines (FORMAT, VERSION, COMMENT)
   ///                            - info section ends while a one of following
   ///                            string has been read
   ///			     - FORMAT=MAP_POINTS separate section for map points and
   ///indexes  for border and obstacles, just map points
   ///                            - FORMAT=MAP_POINTS_CITY_POINTS, separate
   ///                            sections for map points and for city points
   ///                            - FORMAT=MAP_CITY_POINTS, one section for
   ///                            points (MAP and CITIES), cities are typicaly
   ///                            labeled after map points
   ///                            - FORMAT=CELL_MAP TODO !!!
   ///			     - TODO use multi line format to indicate several sections
   /// [MAP_POINTS]               - is followed by set of points in format label
   /// x y, where label
   ///                              is int and x and y are doubles
   /// [MAP_BORDER]               - followed by ordered sequence of labels which
   /// form a border [MAP_OBSTACLE]             - followed by ordered sequence
   /// of labels which [CITY_POINTS]              - followed by set of points
   /// (aka MAP_POINTS) [MAP_VISIBILITY_GRAPH]     - is followed by identifier
   /// of origin of visibility graph
   ///                              MAP_VERTEX label or CITY_VERTEX label,
   ///                              followed by order sequence of lables which
   ///                              are map vertices
   /// [CITY_VISIBILITY]          - list of visible cities from a vertex, which
   /// should be map vertex or city (similar to MAP_VISIBILITY_GRAPH)
   /// [MAP_CONVEX_REGION]        - followed by ordered sequence of lables which
   /// form a convex region of the map,
   ///                              lables are map vertices, first line is in
   ///                              form REGION_LABEL followed by label of
   ///                              region
   /// [MAP_CONVEX_REGION_CITIES] - similar to MAP_CONVEX_REGION but, numbers
   /// are label of cities which are in region
   ///
   /// [MAP_VISIBILITY_REGION]    - similar MAP_VISIBILITY_GRAPH, MAP_VERTEX
   /// label or CITY_VERTEX label is next line, followed by order sequence
   ///                              of labels which points, first label must be
   ///                              the map vertex, that is map_vertices must be
   ///                              mapped into points set.
   ///
   /// [POINTS]		      - general points, if file contains map vertices or
   /// cities, both sets should be mapped into this set.
   ///                               so real general poinst start with label
   ///                               greater than MAP_POINTS.size +
   ///                               CITY_POINTS.size
   /// Triangular mesh sections are [MESH_NODES], [MESH_TRIANGLES],
   /// [MESH_BOUNDARY_VERTICES],
   ///                            - mesh created in map require MAP_POINTS,
   ///                            MAP_BORDER and MAP_OBSTACLE sections
   /// [MESH_NODES]              - first n points are identical to MAP_POINTS,
   /// all other points are triangle points [MESH_TRIANGLES]           -
   /// triangles section, each triangle is on one line
   ///                              each line consists from  triangle_idx,
   ///                              mesh_vertex_idx1 mesh_vertex_idx2
   ///                              mesh_vertex_idx3
   /// [MESH_BOUNDARY_NODES]   - list of mesh vertices which are on map edges
   /// (BORDER or OBSTACLE)
   ///                              vertex_idx  map_vertex_edge_source
   ///                              map_vertex_edge_target
   ///
   /// [MAP_VERTEX_LINESTRING]      - linestring represents sequence of map
   /// vertices
   ///                               - each linestring starts with
   ///                               LINESTRING_LABEL label, where label is an
   ///                               integer number,
   ///                               - after this initial linestring line,
   ///                               labels of the map vertices are
   ///                               - placed each on seperate line
   /// [TRIMESH_CONVEX_REGION]       - followed by REGION_LABEL and sequence of
   /// trimesh nodes which form a convex hull [TRIMESH_CONVEX_REGION_TRIANGLES]
   /// - followed by REGION_LABEL and list of trimesh triangle ids thay are
   /// completely inside convex hull [TRIMESH_CONVEX_REGION_BORDER_POINTS] -
   /// followed by set of poitns (aka MAP_POINTS) [TRIMESH_CONVEX_REGION_BORDER]
   /// - followed by REGION_LABEL and list of indexes to the
   /// TRIMESH_CONVEX_REGION_BORDER_POINTS

   /// Lables must form ordered sequence starts from 0 and incremented by 1, for
   /// each particular section / type of data.
   /// Points definitions (MAP and CITIES) must placed before topology lists.
   /// ----------------------------------------------------------------------------
   class CTextFormatLoader {

    public:
      typedef std::vector<std::string> StringList;
      typedef crl::loader::Point Point;
      typedef crl::loader::Points Points;
      typedef crl::loader::Indexes Indexes;
      typedef crl::loader::IndexesList IndexesList;

      static int getPrecision(void);

    private:
      struct SInfo {
         typedef enum { INFO_FORMAT, INFO_VERSION, INFO_COMMENT } TType;

         typedef enum {
            MAP_POINTS,
            MAP_POINTS_CITY_POINTS,
            MAP_CITY_POINTS,
            CELL_MAP,
            VORONOI_DIAGRAM,
            TRIANGULAR_MESH
         } TFormat;

         static std::string INFO[];
         static std::string FORMATS[];

         TFormat format;
         std::string version;

         std::string name;     // name used for save
         std::string filename; // name used for load
         StringList comments;
         StringList lines;
         StringList modules;

         SInfo();
         /// ----------------------------------------------------------------------------
         /// @brief createLines create lines from particular info values
         ///        NOTE: it must be call before save if changes should be
         ///        written
         ///		     lines are not created automaticaly
         /// ----------------------------------------------------------------------------
         void createLines(void);

         /// ----------------------------------------------------------------------------
         /// @brief parseLines parse lines and fill particular info values
         /// ----------------------------------------------------------------------------
         void parseLines(void);

         /// ----------------------------------------------------------------------------
         /// @brief addComments add comment into comments, only if such comment
         /// is not
         ///                    already in the comments
         ///
         /// @param comment
         /// ----------------------------------------------------------------------------
         void addComment(const std::string &comment);

         SInfo &operator=(const SInfo &other);
      };

      typedef std::map<std::string, CModule *> ModuleMap;

    public:
      typedef SInfo Info;
      typedef CTriangularMesh TriangularMesh;
      Info info;

      Points points;
      Points map_points;
      Points city_points;
      Points trimesh_convex_border_points;

      Indexes map_border;
      IndexesList map_obstacles;
      IndexesList map_convex_regions;
      IndexesList map_vertices_visibility_graphs;
      IndexesList city_map_visibility_graphs;
      IndexesList region_cities;
      IndexesList city_vertex_visible_cities;
      IndexesList map_vertex_visible_cities;
      IndexesList map_vertices_visibility_regions;
      IndexesList city_map_visibility_regions;
      IndexesList map_linestrings;

      IndexesList trimesh_convex_hulls;
      IndexesList trimesh_convex_hull_triangles;

      IndexesList trimesh_convex_borders;

      CTriangularMesh triangular_mesh;

      CTextFormatLoader();
      CTextFormatLoader(const CTextFormatLoader &other);

      ~CTextFormatLoader();

      void clear();

      void load(const std::string &filename);
      bool save(const std::string &filename);

      void print(std::ostream &out);

      CTextFormatLoader &operator=(const CTextFormatLoader &other);

      void loadSimpleTxt(const std::string &filename);
      void printSimpleTxt(std::ostream &out);

      static bool
      isTextFormat(const std::string &filename) throw();

      /// ----------------------------------------------------------------------------
      /// @brief clearCities clear city_points and all related sections
      /// ----------------------------------------------------------------------------
      void clearCities(void);

      /// ----------------------------------------------------------------------------
      /// @brief addModule
      ///
      /// @param module is added to the loader, NOTE the loader is responsible
      /// for the
      ///               module deletion
      /// ----------------------------------------------------------------------------
      CTextFormatLoader &addModule(CModule *module);

      template <class T> T &getModule(const std::string &name)
      {
         T *r = 0;
         ModuleMap::iterator it = modulesMap.find(name);
         if (it != modulesMap.end()) {
            r = dynamic_cast<T *>(it->second);
         }
         crl::assert_argument(r, "Module '" + name +
                                     "' is not registered in the loader");
         return *r;
      }

      template <class T> const T &getModule(const std::string &name) const
      {
         T *r = 0;
         ModuleMap::const_iterator it = modulesMap.find(name);
         if (it != modulesMap.end()) {
            r = dynamic_cast<T *>(it->second);
         }
         crl::assert_argument(r, "Module '" + name +
                                     "' is not registered in the loader");
         return *r;
      }

    private:
      ModuleMap firstSectionMap;
      StringList registeredModules;
      ModuleMap modulesMap;

    public:
      /// ----------------------------------------------------------------------------
      /// @brief Access to x and y coords of Loader points array
      /// ----------------------------------------------------------------------------
      template <class Points> class CCoordsAccessor {
       public:
         typedef typename Points::const_iterator const_iterator;
         typedef typename Points::iterator iterator;
         typedef Points CoordsArray;
         CCoordsAccessor(const CoordsArray &points) {}
         const double getX(const_iterator &it) const { return it->x; }
         const double getY(const_iterator &it) const { return it->y; }
         const double getX(iterator &it) const { return it->x; }
         const double getY(iterator &it) const { return it->y; }

       private:
      };

      typedef CCoordsAccessor<Points> CoordsAccessor;

      /// ----------------------------------------------------------------------------
      /// @brief Access to x and y coords of Loader index to points array
      /// ----------------------------------------------------------------------------
      class IndexCoordsAccessor {
       public:
         typedef Indexes::const_iterator const_iterator;
         typedef Indexes::iterator iterator;
         typedef Points CoordsArray;
         IndexCoordsAccessor(const CoordsArray &points) : points(points) {}
         const double getX(const_iterator it) const { return points[*it].x; }
         const double getY(const_iterator it) const { return points[*it].y; }

       private:
         const CoordsArray &points;
      };
   };

}} // end namespace crl::loader

#endif

/* end of text_format_loader.h */
