/*
 * File name: random.h
 * Date:      2005/10/31 08:37
 * Author:    Jan Faigl
 */

#ifndef __RANDOM_H__
#define __RANDOM_H__

namespace crl {

///
/// @brief
class CRandom {
 public:
   static void randomize(void);
   static long random(void);
   static int next(int val);
   static double random(const double from, const double to);
};

} // namespace crl

#endif

/* end of random.h */
