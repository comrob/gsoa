/*
 * File name: loader_types.cc
 * Date:      2011/02/15 17:16
 * Author:    Jan Faigl
 */

#include <cmath>

#include <crl/loader/loader_types.h>

namespace crl { namespace loader {

   const double EPS = 0.00001;

   /// ----------------------------------------------------------------------------
   Points &add_point(const Point &pt, Points &pts)
   {
      return add_point(pt.x, pt.y, pts);
   }

   /// ----------------------------------------------------------------------------
   Points &add_point(double x, double y, Points &pts)
   {
      pts.push_back(Point(x, y, pts.size()));
      return pts;
   }

   /// ----------------------------------------------------------------------------
   Points &add_point(double x, double y, Points &pts, Indexes &idxs)
   {
      pts.push_back(Point(x, y, pts.size()));
      idxs.push_back(pts.size() - 1);
      return pts;
   }

}} // end namespace crl::loader

using namespace crl::loader;

/// ----------------------------------------------------------------------------
SPoint &SPoint::operator=(const SPoint &pt)
{
   if (this != &pt) {
      x = pt.x;
      y = pt.y;
      label = pt.label;
   }
   return *this;
}

/// ----------------------------------------------------------------------------
bool SPoint::compare(const SPoint &pt) const
{
   return (fabs(x - pt.x) < EPS) && (fabs(y - pt.y) < EPS);
}

/* end of loader_types.cc */
