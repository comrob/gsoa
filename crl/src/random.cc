/*
 * File name: random.cc
 * Date:      2005/10/31 08:38
 * Author:    Jan Faigl
 */

#include <cstdlib>
#include <sys/time.h>

#include <crl/random.h>

using namespace crl;

/// ----------------------------------------------------------------------------
/// Class CRandom
/// ----------------------------------------------------------------------------
void CRandom::randomize(void)
{
   struct timeval tp;
   gettimeofday(&tp, NULL);
   srandom(tp.tv_sec);
   srandom(tp.tv_usec);
}

/// ----------------------------------------------------------------------------
long CRandom::random(void) { return ::random(); }

/// ----------------------------------------------------------------------------
int CRandom::next(int val) { return ::random() % val; }

/// ----------------------------------------------------------------------------
double CRandom::random(const double from, const double to)
{
   return (double)(1.0 * random() / (1.0 * RAND_MAX)) * (to - from) + from;
}

/* end of random.cc */
