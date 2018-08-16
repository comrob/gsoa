/*
 * File name: route_path_utils.cc
 * Date:      2016/12/10 18:13
 * Author:    Jan Faigl
 */

#include <cmath>

#include "route_path_utils.h"

/// - function -----------------------------------------------------------------
double get_path_length(const CoordsVector &pts, bool closed)
{
   double len = 0.;
   for (int i = 1; i < pts.size(); ++i) {
      len += sqrt(pts[i-1].squared_distance(pts[i]));
   }
   if (closed and pts.size() > 1) {
      len += sqrt(pts.back().squared_distance(pts.front()));
   }
   return len;
}

#define dist(i, j) sqrt(path[i].squared_distance(path[j]))
/// - function -----------------------------------------------------------------
void two_opt(CoordsVector &path) 
{
   const int N = path.size();
   int counter = 0;
   double mchange;
   do {
      mchange = 0.0;
      int mi = -1;
      int mj = -1;
      for (int i = 1; i < N; ++i) {
         for (int j = i + 1; j < N-1; ++j) {
            double change = 
               dist(i-1, j) + dist(i, j+1) - dist(i-1, i) - dist(j, j+1);
            if (mchange > change) {
               mchange = change;
               mi = i; mj = j;
            }
            counter += 1;
         }
      }
      if (mi > 0 and mj > 0) {
         CoordsVector newPath;
         for (int i = 0; i < mi; ++i) {
            newPath.push_back(path[i]);
         }
         for (int i = mj; i >= mi; --i) {
            newPath.push_back(path[i]);
         }
         for (int i = mj+1; i < N; ++i) {
            newPath.push_back(path[i]);
         }
         path = newPath;
      }
   } while (mchange < -1e-5);
}

/* end of route_path_utils.cc */
