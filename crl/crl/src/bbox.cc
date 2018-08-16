/*
 * File name: bbox.cc
 * Date:      2009/05/25 09:48
 * Author:    Jan Faigl
 */

#include <limits>

#include <crl/bbox.h>
#include <crl/random.h>

using namespace crl;

/// ----------------------------------------------------------------------------
/// Class CBBox
/// ----------------------------------------------------------------------------
CBBox::CBBox() { clear(); }

/// ----------------------------------------------------------------------------
CBBox::CBBox(const CBBox &b)
{
   for (int i = 0; i < 4; i++) {
      bbox[i] = b.bbox[i];
   }
}

/// ----------------------------------------------------------------------------
CBBox::~CBBox() {}

/// ----------------------------------------------------------------------------
CBBox &CBBox::operator=(const CBBox &b)
{
   if (this != &b) {
      for (int i = 0; i < 4; i++) {
         bbox[i] = b.bbox[i];
      }
   }
   return *this;
}

/// ----------------------------------------------------------------------------
void CBBox::clear(void)
{
   bbox[X_MIN] = std::numeric_limits<double>::max();
   bbox[X_MAX] = -std::numeric_limits<double>::max();
   bbox[Y_MIN] = std::numeric_limits<double>::max();
   bbox[Y_MAX] = -std::numeric_limits<double>::max();
}

/// ----------------------------------------------------------------------------
void CBBox::add(const double x, const double y)
{
   if (bbox[X_MIN] > x) {
      bbox[X_MIN] = x;
   }
   if (bbox[X_MAX] < x) {
      bbox[X_MAX] = x;
   }
   if (bbox[Y_MIN] > y) {
      bbox[Y_MIN] = y;
   }
   if (bbox[Y_MAX] < y) {
      bbox[Y_MAX] = y;
   }
}

/// ----------------------------------------------------------------------------
int CBBox::getRandomX(void) const
{
   return CRandom::random(bbox[X_MIN], bbox[X_MAX]);
}

/// ----------------------------------------------------------------------------
int CBBox::getRandomY(void) const
{
   return CRandom::random(bbox[Y_MIN], bbox[Y_MAX]);
}

/// ----------------------------------------------------------------------------
void CBBox::set(double xMin, double xMax, double yMin, double yMax)
{
   bbox[X_MIN] = xMin;
   bbox[X_MAX] = xMax;
   bbox[Y_MIN] = yMin;
   bbox[Y_MAX] = yMax;
}

/// ----------------------------------------------------------------------------
void CBBox::setX(double xMin, double xMax)
{
   bbox[X_MIN] = xMin;
   bbox[X_MAX] = xMax;
}

/// ----------------------------------------------------------------------------
void CBBox::setY(double yMin, double yMax)
{
   bbox[Y_MIN] = yMin;
   bbox[Y_MAX] = yMax;
}

/// ----------------------------------------------------------------------------
bool CBBox::isInside(double x, double y) const
{
   return (bbox[X_MIN] <= x && x <= bbox[X_MAX] && bbox[Y_MIN] <= y &&
           y <= bbox[Y_MAX]);
}

/* end of bbox.cc */
