/*
 * File name: coords.h
 * Date:      2013/10/13 09:23
 * Author:    Jan Faigl
 */

#ifndef __COORDS_H__
#define __COORDS_H__

#include <vector>
#include <iostream>

/// ----------------------------------------------------------------------------
/// @brief Coords
/// ----------------------------------------------------------------------------
struct Coords {
   double x;
   double y;

   Coords(Coords  &c) : x(c.x), y(c.y) {}
   Coords(const Coords  &c) : x(c.x), y(c.y) {}
   Coords() {}
   Coords(double x, double y) : x(x), y(y) {}
   Coords& operator=(const Coords &c) {
      if (this != &c) {
         x = c.x;
         y = c.y;
      } 
      return *this;
   }

   inline double squared_distance(const Coords  &c) const {
      return squared_distance(*this, c);
   }

   inline static double squared_distance(const Coords  &c1, const Coords  &c2) {
      double dx = c1.x - c2.x;
      double dy = c1.y - c2.y;
      return dx*dx + dy*dy;
   }
};

typedef std::vector<Coords> CoordsVector;
typedef std::vector<CoordsVector> CoordsVectorVector;

std::ostream& operator<<(std::ostream &os, const Coords &pt);

std::istream& operator>>(std::istream &is, Coords &pt);

std::istream& operator>>(std::istream &is, CoordsVector &pts);

#endif

/* end of coords.h */
