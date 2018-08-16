/*
 * File name: route_module.h
 * Date:      2011/03/06 13:17
 * Author:    Jan Faigl
 */

#ifndef __ROUTE_MODULE_H__
#define __ROUTE_MODULE_H__

#include <crl/loader/loader_module.h>
#include <crl/loader/loader_types.h>

namespace crl { namespace loader {

   class CRoute : public CModule {
      typedef enum {
         ROUTE_INDEXES = 0, /// - lst if indexes of the routes, just to check
                            /// the number of routes.
         ROUTE_ROUTE_GOALS = 1, /// - is followed by identifier of the route
                                /// (ROUTE label) that is
         ///                         followed by sequence of the goals on the
         ///                         route (only goals ``cities'' are in the
         ///                         list)
         ROUTE_ROUTE_NODES = 2, /// - is followed by identifier of the route
                                /// (ROUTE label) that is
         ///                         followed by sequence of the nodes on the
         ///                         route (including nodes that are not goals)
         ROUTE_ROUTES_COSTS = 3, /// - optinal, followed by a pair (i c) where i
                                 /// is the route index and c associated costs
                                 /// (double)
         ROUTE_NUM = 4,
         NONE = 5
      } TSections;

      static const std::string name;

    public:
      typedef crl::loader::LabeledCost LabeledCost;

      CRoute();
      ~CRoute();

      CModule *createInstance(void) const { return new CRoute(); }

      const std::string &getName(void) const;

      const std::string &getFirstSection(void) const;

      std::vector<std::string> &getSections(std::vector<std::string> &sections);

      CRoute &operator=(const CRoute &r);

      void clear(void);

      void parse(std::istream &in, std::string &line);

      void print(std::ostream &out) const;

      void fill(const CModule *module);

      Indexes &routeIndexes(void) { return routeIndexes_; }
      LabeledCostVector &routeCost(void) { return routeCost_; }
      IndexesList &routeGoals(void) { return routeGoals_; }
      IndexesList &routeNodes(void) { return routeNodes_; }

      const Indexes &routeIndexes(void) const { return routeIndexes_; }
      const LabeledCostVector &routeCost(void) const { return routeCost_; }
      const IndexesList &routeGoals(void) const { return routeGoals_; }
      const IndexesList &routeNodes(void) const { return routeNodes_; }

    private:
      TSections getSection(const std::string &name) const;

    protected:
      Indexes routeIndexes_;
      IndexesList routeGoals_;
      IndexesList routeNodes_;
      LabeledCostVector routeCost_;
   };

}} // end namespace crl::loader

#endif

/* end of route_module.h */
