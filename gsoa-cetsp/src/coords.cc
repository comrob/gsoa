/*
 * File name: coords.cc
 * Date:      2014/01/16 16:54
 * Author:    Jan Faigl
 */

#include <cmath>

#include "coords.h"

/// - public method ------------------------------------------------------------
std::ostream& operator<<(std::ostream &os, const Coords &pt)
{
   return os << pt.x << ", " << pt.y;
}

/// - public method ------------------------------------------------------------
std::istream& operator>>(std::istream &is, Coords &pt) 
{
   return is >> pt.x >> pt.y;
}

/// - public method ------------------------------------------------------------
std::istream& operator>>(std::istream &is, CoordsVector &pts)
{
   Coords pt;
   while (is >> pt) {
      pts.push_back(pt);
   }
   return is;
}

/* end of coords.cc */
