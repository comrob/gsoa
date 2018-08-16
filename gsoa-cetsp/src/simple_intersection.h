/*
 * File name: simple_intersection.h
 * Date:      2008/08/07 09:06
 * Author:    Jan Faigl
 */

#ifndef __SIMPLE_INTERSECTION_H__
#define __SIMPLE_INTERSECTION_H__

#include <vector> //wn_PnPoly, cn_PnPoly
#include <cmath>
#include <iostream>

namespace geom {

   template <int v>
      struct Int2Type {
         enum { value = v };
      };


   /// ----------------------------------------------------------------------------
   /// @brief  Implementation of intersection without improper intersection
   /// ----------------------------------------------------------------------------
   template<class T, bool with_improper_intersection = false>
      class CIntersection {
         public:
            typedef enum { LEFT_SIDE = 1, COLLINEAR = 0, RIGHT_SIDE = -1 } TOrientation;

            static bool on_left_side(const T &a, const T &b, const T &c) 
            {
               return wind(a, b, c) == LEFT_SIDE;
            }

            static bool on_right_side(const T &a, const T &b, const T &c) 
            {
               return wind(a, b, c) == RIGHT_SIDE;
            }

            inline static double squared_distance(const T &a, const T &b) 
            {
               const double dx = a.x - b.x;
               const double dy = a.y - b.y;
               return dx*dx + dy*dy;
            }

            static bool equal(const double &a, const double &b) 
            {
               static const double EPS = 0.00001;
               return (fabs(a - b) < EPS) ? true : false;
            }

            static TOrientation wind(const T &a, const T &b, const T &c)
            {
               double w;
               w = ((a.y - b.y)*(c.x - b.x) - (c.y - b.y)*(a.x - b.x));
               // need to allow for small math errors   seen with "gcc -O2 -mcpu=i686 -ffast-math"
               //return (w > .0001) ? 1 : ((w < -.0001) ? -1 : 0);
               return (w > .0001) ? LEFT_SIDE : ((w < -.0001) ? RIGHT_SIDE : COLLINEAR);
            }

            static double zeroRound(double x) 
            {
               return fabs(x) < .00001 ? 0.0 : x;
            }

            static TOrientation windZeroRound(const T &a, const T &b, const T &c) 
            {
               double w;
               w = (zeroRound(a.y - b.y)*zeroRound(c.x - b.x) - zeroRound(c.y - b.y)*zeroRound(a.x - b.x));
               // need to allow for small math errors   seen with "gcc -O2 -mcpu=i686 -ffast-math"
               //return (w > .0001) ? 1 : ((w < -.0001) ? -1 : 0);
               return (w > .0001) ? LEFT_SIDE : ((w < -.0001) ? RIGHT_SIDE : COLLINEAR);
            }

            static double windD(const T &a, const T &b, const T &c) 
            {
               double w;
               w = ((a.y - b.y)*(c.x - b.x) - (c.y - b.y)*(a.x - b.x));
               // need to allow for small math errors   seen with "gcc -O2 -mcpu=i686 -ffast-math"
               //	  return (w > .0001) ? 1 : ((w < -.0001) ? -1 : 0);
               return w;
            }

            static double area2(const T &a, const T &b, const T &c) 
            {
               return (b.x - a.x)*(c.y - a.y) - (c.x - a.x)*(b.y - a.y);
            }

         protected:
            static int inBetween (const T &a, const T &b, const T &c, Int2Type<false>) 
            {
               //if (a.x != b.x)   // not vertical 
               if (!equal(a.x, b.x))
                  return (((a.x < c.x) && (c.x < b.x)) || ((b.x < c.x) && (c.x < a.x)));
               else
                  return (((a.y < c.y) && (c.y < b.y)) || ((b.y < c.y) && (c.y < a.y)));
            }

            static int inBetween (const T &a, const T &b, const T &c, Int2Type<true>) 
            {
               //	  if (a.x != b.x)   // not vertical 
               if (!equal(a.x, b.x))
                  return (((a.x <= c.x) && (c.x <= b.x)) || ((b.x <= c.x) && (c.x <= a.x)));
               else
                  return (((a.y <= c.y) && (c.y <= b.y)) || ((b.y <= c.y) && (c.y <= a.y)));
            }

            static int inBetween (const T &a, const T &b, const T &c) 
            {
               return inBetween(a, b, c, Int2Type<with_improper_intersection>());
            }
         public:

            static bool Xor(bool x, bool y) 
            {
               return !x ^ !y;
            }

            /// ----------------------------------------------------------------------------
            /// proper intersection
            /// ----------------------------------------------------------------------------
            static bool intersect(const T &a, const T  &b, const T &c, const T &d, Int2Type<false>) 
            {
               if (
                     collinear(a, b, c) ||
                     collinear(a, b, d) ||
                     collinear(c, d, a) ||
                     collinear(c, d, b)
                  ) 
                  return false;
               return 
                  //Xor(on_left_side(a, b, c), on_left_side(a, b, d)) &&
                  //Xor(on_left_side(c, d, a), on_left_side(c, d, b));
                  Xor(wind(a, b, c) == LEFT_SIDE, wind(a, b, d) == LEFT_SIDE) &&
                  Xor(wind(c, d, a) == LEFT_SIDE, wind(c, d, b) == LEFT_SIDE);
            }

            /// ----------------------------------------------------------------------------
            /// improper intersection
            /// ----------------------------------------------------------------------------
            static bool intersect(const T &a, const T & b, const T &c, const T &d, Int2Type<true>)
            {
               if (intersect(a, b, c, d, Int2Type<false>()))
                  return true;
               else if (
                     onSegment(a, b, c) ||
                     onSegment(a, b, d) ||
                     onSegment(c, d, a) ||
                     onSegment(c, d, b)
                     ) 
                  return true;
               else
                  return false;
            }

            static bool intersect(const T &a, const T & b, const T &c, const T &d)
            {
               return intersect(a, b, c, d, Int2Type<with_improper_intersection>());
            }


            /// ----------------------------------------------------------------------------
            /// return if intersection is improper
            /// ----------------------------------------------------------------------------
            static bool intersect(const T &a, const T & b, const T &c, const T &d, bool &improper, Int2Type<false>)
            {
               if (
                     collinear(a, b, c) ||
                     collinear(a, b, d) ||
                     collinear(c, d, a) ||
                     collinear(c, d, b)
                  ) 
                  return false;
               return 
                  Xor(wind(a, b, c) == LEFT_SIDE, wind(a, b, d) == LEFT_SIDE) &&
                  Xor(wind(c, d, a) == LEFT_SIDE, wind(c, d, b) == LEFT_SIDE);
            }

            static bool intersect(const T &a, const T & b, const T &c, const T &d, bool &improper, Int2Type<true>)
            {
               if (intersect(a, b, c, d, Int2Type<false>()))
                  return true;
               else if (
                     onSegment(a, b, c) ||
                     onSegment(a, b, d) ||
                     onSegment(c, d, a) ||
                     onSegment(c, d, b)
                     ) {
                  improper = true;
                  return true;
               }
               else
                  return false;
            }


            static bool intersect(const T &a, const T & b, const T &c, const T &d, bool &improper)
            { 
               improper = false;
               return intersect(a, b, c, d, improper, Int2Type<with_improper_intersection>());
            }

            /// ----------------------------------------------------------------------------
            /// @brief collinear
            /// 
            /// @param a 
            /// @param b 
            /// @param c 
            /// @param d 
            /// 
            /// @return true if segments (a,b) and (c,d) are collinear that is a,b,c and a,b,d are collinear
            /// ----------------------------------------------------------------------------
            static bool collinear(const T &a, const T &b, const T &c, const T &d)
            {
               int a_abc;
               int a_abd;
               a_abc = wind(a,b,c);
               a_abd = wind(a,b,d);
               return (a_abc == 0 && a_abd == 0);
            }

            /// ----------------------------------------------------------------------------
            /// @brief collinear
            /// 
            /// @param a 
            /// @param b 
            /// @param c 
            /// 
            /// @return true if a,b,c are collinear
            /// ----------------------------------------------------------------------------
            static bool collinear(const T &a, const T &b, const T &c)
            {
               return wind(a,b,c) == 0;
            }

            /// ----------------------------------------------------------------------------
            /// @brief are_collinear_points_ordered_along_line
            /// 
            /// @param a 
            /// @param b 
            /// @param c 
            /// 
            /// @return true of b lies bettwen a and c, or b == a or b == c
            /// ----------------------------------------------------------------------------
            static bool are_collinear_points_ordered_along_line(const T &a, const T& b, const T& c)
            {
               return inBetween(a, c, b);
            }

            /// ----------------------------------------------------------------------------
            /// @brief collinear
            /// 
            /// @param a 
            /// @param b 
            /// @param c 
            /// 
            /// @return true if a,b,c are collinear and c lies inside segment defined by a and ba
            /// TODO @deprecated 
            /// ----------------------------------------------------------------------------
            static bool collinearSegment(const T &a, const T &b, const T &c)
            {
               return wind(a,b,c) == 0 && inBetween(a, b, c);
            }

            /// ----------------------------------------------------------------------------
            /// @brief onSegment
            /// 
            /// @param a 
            /// @param b 
            /// @param c 
            /// 
            /// @return true if point c lines on segment (a,b)
            /// ----------------------------------------------------------------------------
            static bool onSegment(const T &a, const T &b, const T &c)
            {
               if (collinear(a, b, c)) {
                  if (equal(a.x, b.x)) {
                     return 
                        ((a.y <= c.y && c.y <= b.y))  || 
                        ((a.y >= c.y && c.y >= b.y));
                  } else {
                     return 
                        ((a.x <= c.x && c.x <= b.x))  || 
                        ((a.x >= c.x && c.x >= b.x));
                  }
               } else 
                  return false;
            }

            /// ----------------------------------------------------------------------------
            /// @brief onSegment
            /// 
            /// @param a 
            /// @param b 
            /// @param c 
            /// @param wind (wind of a,b, c)
            /// 
            /// @return true if point c lines on segment (a,b) same as above, but use already computed wind
            /// ----------------------------------------------------------------------------
            static bool onSegment(const T &a, const T &b, const T &c, int wind)
            {
               if (wind == 0) {
                  if (equal(a.x, b.x)) {
                     return 
                        ((a.y <= c.y && c.y <= b.y))  || 
                        ((a.y >= c.y && c.y >= b.y));
                  } else {
                     return 
                        ((a.x <= c.x && c.x <= b.x))  || 
                        ((a.x >= c.x && c.x >= b.x));
                  }
               } else 
                  return false;
            }


            /// ----------------------------------------------------------------------------
            /// @brief in_cone
            /// 
            /// @param a0 
            /// @param a1 
            /// @param a2 
            /// @param b 
            /// 
            /// @return true if b is in cone of a0, a1, a2
            /// ----------------------------------------------------------------------------
            static bool in_cone(const T &a0, const T &a1, const T &a2, const T &b)
            {
               bool ret;
               int m = wind(b,a0,a1);
               int p = wind(b,a1,a2);
               if (wind(a0,a1,a2) > 0) {
                  ret = ( m >= 0 && p >= 0 ); // convex at a1
               }
               else {
                  ret = ( m >= 0 || p >= 0 ); // reflex at a1
               }
               return ret;
            }

            /// ----------------------------------------------------------------------------
            /// @brief bettwen based on algorithm ParallelInt from Computation Geomery in C
            /// 
            /// @param a 
            /// @param b 
            /// @param c 
            /// 
            /// @return 
            /// ----------------------------------------------------------------------------
            static bool bettwen(const T &a, const T &b, const T &c)
            {
               if (a.x != b.x)   // not vertical 
                  return (
                        ((a.x <= c.x) && (c.x <= b.x)) || 
                        ((a.x >= c.x) && (c.x >= b.x)));
               else
                  return (
                        ((a.y <= c.y) && (c.y <= b.y)) || 
                        ((a.y >= c.y) && (c.y >= b.y))
                        );
            }

            /// ----------------------------------------------------------------------------
            /// @brief parallelIntersection based on algorithm ParallelInt from Computation Geomery in C
            /// 
            /// @param a 
            /// @param b 
            /// @param c 
            /// @param d 
            /// @param p 
            /// 
            /// @return 
            /// ----------------------------------------------------------------------------
            static char parallelIntersection(const T &a, const T &b, const T &c, const T &d, T &p)
            {
               if (!collinear(a, b, c)) {
                  return '0';
               }
               if (bettwen(a, b, c)) {
                  p = c;
                  return 'e';
               }
               if (bettwen(a, b, d)) {
                  p = d;
                  return 'e';
               }
               if (bettwen(c, d, a)) {
                  p = a;
                  return 'e';
               }
               if (bettwen(c, d, b)) {
                  p = b;
                  return 'e';
               }
               return '0';
            }

            /// ----------------------------------------------------------------------------
            /// @brief segmentIntersection based on algorithm SegSegInt from Computation Geomery in C
            /// 
            /// @param a segment(a,b)
            /// @param b 
            /// @param c segment(c,d)
            /// @param d 
            /// @param p interseciton point 
            /// 
            /// @return '1' proper intersection, '0' no intersection
            ///         'e' edge intersection, 'v' vertex intersection
            /// ----------------------------------------------------------------------------
            static char segmentIntersection(const T &a, const T &b, const T &c, const T &d, T &p)
            {
               static const double EPS = 0.00001;
               double s, t;
               double num, denom;
               char code = '0';

               denom = 
                  a.x * (d.y - c.y) +
                  b.x * (c.y - d.y) +
                  d.x * (b.y - a.y) +
                  c.x * (a.y - b.y);

               if (fabs(denom) < EPS) {
                  code = parallelIntersection(a, b, c, d, p);
               } else {
                  num = 
                     a.x * (d.y - c.y) +
                     c.x * (a.y - d.y) +
                     d.x * (c.y - a.y);
                  if ( (fabs(num) < EPS) || (fabs(num - denom) < EPS)) {
                     code = 'v';
                  }
                  s = num / denom;
                  num = - (
                        a.x * (c.y - b.y) +
                        b.x * (a.y - c.y) +
                        c.x * (b.y - a.y) );
                  if ( (fabs(num) < EPS) || (fabs(num - denom) < EPS)) {
                     code = 'v';
                  }
                  t = num / denom;

                  if ( 
                        (0.0 < s) && (s < 1.0) && 
                        (0.0 < t) && (t < 1.0)
                     ) {
                     code = '1';
                  } else if (
                        ( (0.0 > s) || (s > 1.0) ) && 
                        ( (0.0 > t) || (t > 1.0) ) 
                        ) {
                     code = '0';
                  }
                  p.x = a.x + s * (b.x - a.x);
                  p.y = a.y + s * (b.y - a.y);
               }
               return code;
            }

            static bool lineLineIntersect(const T &p1, const T &p2, const T &p3, const T &p4, T &pa, T &pb)
            {
               static const double EPS = 0.00001;
               T p13,p43,p21;
               double d1343,d4321,d1321,d4343,d2121;
               double numer,denom;
               double mua, mub;

               p13.x = p1.x - p3.x;
               p13.y = p1.y - p3.y;
               p43.x = p4.x - p3.x;
               p43.y = p4.y - p3.y;
               std::cout << "p43:" << p43 << std::endl;
               if (fabs(p43.x)  < EPS && fabs(p43.y)  < EPS)
                  return false;
               p21.x = p2.x - p1.x;
               p21.y = p2.y - p1.y;
               std::cout << "p21:" << p21 << std::endl;
               if (fabs(p21.x)  < EPS && fabs(p21.y)  < EPS)
                  return false;

               d1343 = p13.x * p43.x + p13.y * p43.y;
               d4321 = p43.x * p21.x + p43.y * p21.y;
               d1321 = p13.x * p21.x + p13.y * p21.y;
               d4343 = p43.x * p43.x + p43.y * p43.y;
               d2121 = p21.x * p21.x + p21.y * p21.y;

               denom = d2121 * d4343 - d4321 * d4321;
               std::cout << "denom:" << denom << std::endl;
               if (fabs(denom) < EPS) {
                  std::cout << "abs(denom)" << fabs(denom) << " " << EPS  << std::endl;
                  return false;
               }
               numer = d1343 * d4321 - d1321 * d4343;

               mua = numer / denom;
               mub = (d1343 + d4321 * mua) / d4343;

               pa.x = p1.x + mua * p21.x;
               pa.y = p1.y + mua * p21.y;
               pb.x = p3.x + mub * p43.x;
               pb.y = p3.y + mub * p43.y;

               std::cout << "pa:" << pa << std::endl;
               std::cout << "pb:" << pb << std::endl;
               return true;
            }

            // isLeft(): tests if a point is Left|On|Right of an infinite line.
            //    Input:  three points P0, P1, and P2
            //    Return: >0 for P2 left of the line through P0 and P1
            //            =0 for P2 on the line
            //            <0 for P2 right of the line
            //    See: the January 2001 Algorithm "Area of 2D and 3D Triangles and Polygons"
            static double isLeft(const  T &P0, const T &P1, const T &P2 ) 
            {
               return ( (P1.x - P0.x) * (P2.y - P0.y) - (P2.x - P0.x) * (P1.y - P0.y) );
            }


            /// ----------------------------------------------------------------------------
            /// @brief get_point_line_projections find point p as projection of c to line (a,b)
            /// 
            /// @param c 
            /// @param a 
            /// @param b 
            /// @param p 
            /// ----------------------------------------------------------------------------
            static void get_point_line_projections(const T &c, const T& a, const T& b, T& p)
            {
               const double r_num = (c.x - a.x) * (b.x - a.x) + (c.y - a.y) * (b.y - a.y);
               const double r_den = (b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y);
               const double r = r_num / r_den;
               p.x = a.x + r * (b.x - a.x);
               p.y = a.y + r * (b.y - a.y);
            }

            /// ----------------------------------------------------------------------------
            /// @brief point_line_distance return distance of point c from line a,b
            /// 
            /// @param c 
            /// @param a 
            /// @param b 
            /// 
            /// @return 
            /// ----------------------------------------------------------------------------
            static double point_line_squared_distance(const T &c, const T &a, const T &b) 
            {
               const double r_den = (b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y);
               const double s = (a.y - c.y) * (b.x - a.x) - (a.x - c.x) * (b.y - a.y);
               return s*s/r_den;
            }

            /// ----------------------------------------------------------------------------
            /// @brief point_segment_squared_distance return distance of point c from segment a,b
            /// 
            /// @param c 
            /// @param a 
            /// @param b 
            /// 
            /// @return 
            /// ----------------------------------------------------------------------------
            static double point_segment_squared_distance(const T &c, const T &a, const T &b)
            {
               const double r_num = (c.x - a.x) * (b.x - a.x) + (c.y - a.y) * (b.y - a.y);
               const double r_den = (b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y);
               const double r = r_num / r_den;
               double ret;

               if ( (r >= 0) && (r <= 1) ) { //segment distance is line distance
                  const double s =  (a.y - c.y) * (b.x - a.x) - (a.x - c.x) * (b.y - a.y);
                  ret = s*s/r_den;
               } else { //segment distance is distance from c to a or from c to b
                  const double dist1 = squared_distance(c, a);
                  const double dist2 = squared_distance(c, b);
                  ret = dist1 < dist2 ? dist1 : dist2;
               }
               return ret;
            }

            /// ----------------------------------------------------------------------------
            /// @brief point_segment_squared_distance
            ///
            /// same as above, but return indication endpoint indication and the point itself
            /// endpoint indication:
            /// 's' inside segment
            /// 'a' at the a point
            /// 'b' at the b point     
            /// ----------------------------------------------------------------------------
            static double point_segment_squared_distance(const T &c, const T &a, const T &b, char &endpoint, T &p)
            {
               const double r_num = (c.x - a.x) * (b.x - a.x) + (c.y - a.y) * (b.y - a.y);
               const double r_den = (b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y);
               const double r = r_num / r_den;
               double ret;

               if ( (r >= 0) && (r <= 1) ) { //segment distance is line distance
                  const double s =  (a.y - c.y) * (b.x - a.x) - (a.x - c.x) * (b.y - a.y);
                  p.x = a.x + r * (b.x - a.x);
                  p.y = a.y + r * (b.y - a.y);
                  ret = s*s/r_den;
                  endpoint = 's';
               } else { //segment distance is distance from c to a or from c to b
                  const double dist1 = squared_distance(c, a);
                  const double dist2 = squared_distance(c, b);
                  if (dist1 < dist2) {
                     ret = dist1;
                     p = a;
                     endpoint = 'a';
                  } else {
                     ret = dist2;
                     p = b;
                     endpoint = 'b';
                  }
               }
               return ret;
            }

            /// ----------------------------------------------------------------------------
            // wn_PnPoly(): winding number test for a point in a polygon
            //      Input:   P = a point,
            //               V[] = vertex points of a polygon V[n+1] with V[n]=V[0]
            //      Return:  wn = the winding number (=0 only if P is outside V[])
            static int wn_PnPoly( const T &P, const std::vector<T> &V) {
               int    wn = 0;    // the winding number counter
               const int n = V.size();

               // loop through all edges of the polygon
               for (int i=0; i<n; i++) {   // edge from V[i] to V[i+1]
                  if (V[i].y <= P.y) {         // start y <= P.y
                     if (V[i+1].y > P.y)      // an upward crossing
                        if (isLeft( V[i], V[i+1], P) > 0)  // P left of edge
                           ++wn;            // have a valid up intersect
                  }
                  else {                       // start y > P.y (no test needed)
                     if (V[i+1].y <= P.y)     // a downward crossing
                        if (isLeft( V[i], V[i+1], P) < 0)  // P right of edge
                           --wn;            // have a valid down intersect
                  }
               }
               return wn;
            }

            /// ----------------------------------------------------------------------------
            // cn_PnPoly(): crossing number test for a point in a polygon
            //      Input:   P = a point,
            //               V[] = vertex points of a polygon V[n+1] with V[n]=V[0]
            //      Return:  0 = outside, 1 = inside
            // This code is patterned after [Franklin, 2000]
            static int cn_PnPoly(const T &P, const std::vector< T > &V)
            {
               int    cn = 0;    // the crossing number counter
               const int n = V.size();

               // loop through all edges of the polygon
               for (int i=0; i<n; i++) {    // edge from V[i] to V[i+1]
                  if (((V[i].y <= P.y) && (V[i+1].y > P.y))    // an upward crossing
                        || ((V[i].y > P.y) && (V[i+1].y <= P.y))) { // a downward crossing
                     // compute the actual edge-ray intersect x-coordinate
                     double vt = (double)1.0*(P.y - V[i].y) / (V[i+1].y - V[i].y);
                     if (P.x < V[i].x + vt * (V[i+1].x - V[i].x)) // P.x < intersect
                        ++cn;   // a valid crossing of y=P.y right of P.x
                  }
               }
               return (cn&1);    // 0 if even (out), and 1 if odd (in)
            }

            // this seems be correct form the above listed version (be aware that V[n] = V[0], only for the multiple component)
            static int pnpoly(const T &P, const std::vector<T>  &V) 
            {
               int c = 0;
               const int n = V.size();
               int j = n - 1;
               for (int i = 0; i < n; j = i++) {
                  if ( ((V[i].y > P.y) != (V[j].y > P.y)) &&
                        (P.x < (V[j].x - V[i].x) * (P.y-V[i].y) / (V[j].y - V[i].y) + V[i].x ))
                     c = !c;
               }
               return c;
            }
      }; //end class CIntersection

} //end namespace geom

#endif

/* end of simple_intersection.h */
