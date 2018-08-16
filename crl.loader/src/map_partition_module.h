/*
 * File name: map_partition_module.h
 * Date:      2011/02/15 17:23
 * Author:    Jan Faigl
 */

#ifndef __MAP_PARTITION_MODULE_H__
#define __MAP_PARTITION_MODULE_H__

#include <crl/loader/loader_module.h>
#include <crl/loader/loader_types.h>

namespace crl { namespace loader {

   class CMapPartition : public CModule {
      typedef enum {
         /// All the sections must placed in the file in the following order
         /// without interuppting by additional section
         PARTITION_POINTS, /// - is followed by set of points in format label x
                           /// y, where label
         ///                     is int and x and y are doubles
         ///                     the first n points are identical to MAP_POINTS,
         ///                     all other points are triangle points
         PARTITION_REGION, ///  followed by REGION_LABEL and sequence of indexes
                           ///  of partition points forming a polygon
         PARTITION_POINTS_ON_MAP_EDGES, /// followed by a list of triplets (i v1
                                        /// v2) where
         ///                                i is the index of the point (not a
         ///                                map point) i >= n (starting from 0)
         ///                                v1 and v2 are indexes of the map
         ///                                points, i.e., v1 < n and v2 < n
         PARTITION_SIMPLE_POLYGONS_WITH_GATES, /// followed by the list of
                                               /// labels that are simple
                                               /// polygons with gates
         PARTITION_TRIANGLES, /// followed by a list of labels of regions that
                              /// are triangles
         PARTITION_NUM,
         NONE
      } TSections;

      static const std::string name;

    public:
      CMapPartition();
      ~CMapPartition();

      CModule *createInstance(void) const { return new CMapPartition(); }

      const std::string &getName(void) const;

      const std::string &getFirstSection(void) const;

      std::vector<std::string> &getSections(std::vector<std::string> &sections);

      CMapPartition &operator=(const CMapPartition &mp);

      void clear(void);

      void parse(std::istream &in, std::string &line);

      void print(std::ostream &out) const;

      void fill(const CModule *module);

      Points &points(void) { return points_; }
      IndexesList &regions(void) { return regions_; }
      EdgeNodeVector &edgeNodes(void) { return edgeNodes_; }
      Indexes &simplePolygonsWithGates(void) { return simplePolygons_; }
      Indexes &triangles(void) { return triangles_; }

      const Points &points(void) const { return points_; }
      const IndexesList &regions(void) const { return regions_; }
      const EdgeNodeVector &edgeNodes(void) const { return edgeNodes_; }
      const Indexes &simplePolygonsWithGates(void) const
      {
         return simplePolygons_;
      }
      const Indexes &triangles(void) const { return triangles_; }

    private:
      TSections getSection(const std::string &name) const;

    protected:
      Points points_;
      IndexesList regions_;
      EdgeNodeVector edgeNodes_;
      Indexes simplePolygons_;
      Indexes triangles_;
   };

}} // end namespace crl::loader

// std::istream& operator<<(std::istream& in);
// std::ostream& operator>>(std::ostream& in);

#endif

/* end of map_partition_module.h */
