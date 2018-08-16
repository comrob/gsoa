/*
 * File name: tspnode.h
 * Date:      2006/04/28 09:06
 * Author:
 */

#ifndef __TSPNODE_H__
#define __TSPNODE_H__

#include <cmath>
#include <iostream>

namespace crl { namespace tsp {

   template <class T, class TT = int> struct STSPNode {
      typedef T Kernel;
      typedef TT Label;
      Kernel x;
      Kernel y;
      Label i;

      STSPNode(){};
      STSPNode(Kernel iX, Kernel iY) : x(iX), y(iY){};
      STSPNode(Kernel iX, Kernel iY, Label iI) : x(iX), y(iY), i(iI){};

      Kernel distanceNoSqrt(const STSPNode<Kernel, Label> &n)
      {
         Kernel dx = x - n.x;
         Kernel dy = y - n.y;
         return dx * dx + dy * dy;
      }

      Kernel distanceSqrt(const STSPNode<Kernel, Label> &n)
      {
         Kernel dx = x - n.x;
         Kernel dy = y - n.y;
         return sqrt(dx * dx + dy * dy);
      }
   };

   template <class T, class TT>
   inline std::ostream &operator<<(std::ostream &os,
                                   const STSPNode<T, TT> &node)
   {
      os << node.i << " " << node.x << " " << node.y;
      return os;
   }

   template <class T, class TT>
   inline std::istream &operator>>(std::istream &is, STSPNode<T, TT> &node)
   {
      return (is >> node.i >> node.x >> node.y);
   }

}} // namespace crl::tsp

#endif

/* end of tspnode.h */
