/*
 * File name: bbox.h
 * Date:      2009/05/25 09:47
 * Author:    Jan Faigl
 */

#ifndef __BBOX_H__
#define __BBOX_H__

#include <crl/random.h>

namespace crl {

class CBBox {
   enum { X_MIN = 0, X_MAX = 1, Y_MIN = 2, Y_MAX = 3 };

 public:
   CBBox();
   CBBox(const CBBox &b);
   ~CBBox();

   CBBox &operator=(const CBBox &b);
   void clear(void);
   void add(const double x, const double y);
   double minx(void) const { return bbox[X_MIN]; }
   double maxx(void) const { return bbox[X_MAX]; }
   double miny(void) const { return bbox[Y_MIN]; }
   double maxy(void) const { return bbox[Y_MAX]; }

   double width(void) const { return bbox[X_MAX] - bbox[X_MIN]; }
   double height(void) const { return bbox[Y_MAX] - bbox[Y_MIN]; }
   template <class Coords> void add(const Coords &pt) { add(pt.x, pt.y); }

   int getRandomX(void) const;
   int getRandomY(void) const;

   template <class Coords> Coords &getRandom(Coords &pt)
   {
      pt.x = CRandom::random(bbox[X_MIN], bbox[X_MAX]);
      pt.y = CRandom::random(bbox[Y_MIN], bbox[Y_MAX]);
      return pt;
   }

   void set(double xMin, double xMax, double yMin, double yMax);
   void setX(double xMin, double xMax);
   void setY(double yMin, double yMax);

   bool isInside(double x, double y) const;

   template <class T> void addArray(const T &array)
   {
      for (typename T::const_iterator it = array.begin(); it != array.end();
           it++) {
         add(it->x, it->y);
      }
   }
   template <class T> void addPtrArray(const T &array)
   {
      for (typename T::const_iterator it = array.begin(); it != array.end();
           it++) {
         add((*it)->x, (*it)->y);
      }
   }

   template <class T> T getMaxCoords(const T &t) { return T(maxx(), maxy()); }
   template <class T> T getMinCoords(const T &t) { return T(minx(), miny()); }

 private:
   double bbox[4];
};

} // end namespace crl

#endif

/* end of bbox.h */
