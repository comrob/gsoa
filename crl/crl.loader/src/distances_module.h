/*
 * File name: distances_module.h
 * Date:      2011/02/24 20:53
 * Author:    Jan Faigl
 */

#ifndef __DISTANCES_MODULE_H__
#define __DISTANCES_MODULE_H__

#include <crl/loader/loader_module.h>
#include <crl/loader/loader_types.h>

namespace crl { namespace loader {

   class CDistances : public CModule {
      typedef enum {
         DISTANCES_NODES = 0, /// - is followed by a list of nodes indexes
         DISTANCES_NODE_NODE_DISTANCES =
             1, /// followed by a list of triplets (v1 v2 d) where
         //                                     v1 and v2 are indexes to nodes
         //                                     and d is a distance between the
         //                                     nodes
         DISTANCES_NUM = 2,
         NONE = 3
      } TSections;

      static const std::string name;

    public:
      CDistances();
      ~CDistances();

      CModule *createInstance(void) const { return new CDistances(); }

      const std::string &getName(void) const;

      const std::string &getFirstSection(void) const;

      std::vector<std::string> &getSections(std::vector<std::string> &sections);

      CDistances &operator=(const CDistances &mp);

      void clear(void);

      void parse(std::istream &in, std::string &line);

      void print(std::ostream &out) const;

      void fill(const CModule *module);

      Indexes &nodes(void) { return nodes_; }
      NodesDistanceVector &distances(void) { return distances_; }
      const Indexes &nodes(void) const { return nodes_; }
      const NodesDistanceVector &distances(void) const { return distances_; }

    private:
      TSections getSection(const std::string &name) const;

    protected:
      Indexes nodes_;
      NodesDistanceVector distances_;
   };

}} // end namespace crl::loader

#endif

/* end of graph_module.h */
