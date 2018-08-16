/*
 * File name: loader_types.h
 * Date:      2011/02/15 17:15
 * Author:    Jan Faigl
 */

#ifndef __LOADER_TYPES_H__
#define __LOADER_TYPES_H__

#include <vector>

namespace crl { namespace loader {

   /// ----------------------------------------------------------------------------
   /// @brief
   /// ----------------------------------------------------------------------------
   struct SPoint {
      int label;
      double x;
      double y;
      SPoint() : label(-1), x(0.0), y(0.0) {}
      SPoint(const double x, const double y, const int label = -1)
          : label(label), x(x), y(y)
      {
      }
      SPoint(const SPoint &pt) : label(pt.label), x(pt.x), y(pt.y) {}
      SPoint &operator=(const SPoint &pt);
      bool operator==(const SPoint &pt) const
      {
         return (x == pt.x) && (y == pt.y) && (label == pt.label);
      }

      bool compare(const SPoint &pt) const;

      double squared_distance(const SPoint &pt) const
      {
         const double dx = x - pt.x;
         const double dy = y - pt.y;
         return dx * dx + dy * dy;
      }
   };

   /// ----------------------------------------------------------------------------
   /// @brief Basic Loader Types
   /// ----------------------------------------------------------------------------
   typedef SPoint Point;
   typedef std::vector<Point> Points;
   typedef std::vector<Point *> PointPtrVector;
   typedef std::vector<Points> PointsVector;
   typedef std::vector<int> Indexes;
   typedef std::vector<Indexes> IndexesList;

   /// ----------------------------------------------------------------------------
   /// @brief add_point
   ///
   /// @param x
   /// @param y
   /// @param pts
   ///
   /// @return
   /// ----------------------------------------------------------------------------
   Points &add_point(const Point &pt, Points &pts);

   /// ----------------------------------------------------------------------------
   /// @brief add_point
   ///
   /// @param x
   /// @param y
   /// @param pts
   ///
   /// @return
   /// ----------------------------------------------------------------------------
   Points &add_point(double x, double y, Points &pts);

   /// ----------------------------------------------------------------------------
   /// @brief add_point
   ///
   /// @param x
   /// @param y
   /// @param pts
   /// @param idxs
   ///
   /// @return
   /// ----------------------------------------------------------------------------
   Points &add_point(double x, double y, Points &pts, Indexes &idxs);

   struct SEdgeNode {
      int label; // label of the node lying on the edge between v1 and v2
      int v1;    // first ed
      int v2;
      SEdgeNode() : label(-1), v1(-1), v2(-1) {}
      SEdgeNode(int label, int v1 = -1, int v2 = -1)
          : label(label), v1(v1), v2(v2)
      {
      }
      SEdgeNode(const SEdgeNode &n) : label(n.label), v1(n.v1), v2(n.v2) {}
      bool operator==(const SEdgeNode &n)
      {
         return ((label == n.label) and (v1 == n.v1) and (v2 == n.v2));
      }
      SEdgeNode &operator=(const SEdgeNode &n)
      {
         if (this != &n) {
            label = n.label;
            v1 = n.v1;
            v2 = n.v2;
         }
         return *this;
      }
   };

   typedef SEdgeNode EdgeNode;
   typedef std::vector<SEdgeNode> EdgeNodeVector;

   struct SNodeNodeDistance {
      int v1; // first ed
      int v2;
      double distance; // distance between the v1 and v2
      SNodeNodeDistance() : v1(-1), v2(-1), distance(-1.) {}
      SNodeNodeDistance(int v1 = -1, int v2 = -1, double distance = -1.)
          : v1(v1), v2(v2), distance(distance)
      {
      }
      SNodeNodeDistance(const SNodeNodeDistance &n)
          : v1(n.v1), v2(n.v2), distance(n.distance)
      {
      }
      bool operator==(const SNodeNodeDistance &n)
      {
         return ((v1 == n.v1) and (v2 == n.v2) and
                 (distance == n.distance)); // Watch the double comparison
      }
      SNodeNodeDistance &operator=(const SNodeNodeDistance &n)
      {
         if (this != &n) {
            v1 = n.v1;
            v2 = n.v2;
            distance = n.distance;
         }
         return *this;
      }
   };

   typedef SNodeNodeDistance NodesDistance;
   typedef std::vector<NodesDistance> NodesDistanceVector;

   struct SLCost {
      int label;
      double cost;
      SLCost() : label(-1), cost(-1.) {}
      SLCost(int label = -1, double cost = -1.) : label(label), cost(cost) {}
      SLCost(const SLCost &n) : label(n.label), cost(n.cost) {}
      bool operator==(const SLCost &n)
      {
         return ((label == n.label) and
                 (cost == n.cost)); // Watch the double comparison
      }
      SLCost &operator=(const SLCost &n)
      {
         if (this != &n) {
            label = n.label;
            cost = n.cost;
         }
         return *this;
      }
   };

   typedef SLCost LabeledCost;
   typedef std::vector<LabeledCost> LabeledCostVector;

}} // end namespace crl::loader

#endif

/* end of loader_types.h */
