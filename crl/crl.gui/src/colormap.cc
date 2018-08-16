/*
 * File name: colormap.cc
 * Date:      2009/04/13 07:45
 * Author:    Jan Faigl
 */

#include <fstream>
#include <limits>

#include <crl/assert.h>
#include <crl/logging.h>

#include <crl/gui/colormap.h>

using namespace crl::gui;

/// ----------------------------------------------------------------------------
/// Class CColorMap
/// ----------------------------------------------------------------------------
CColorMap::CColorMap()
{
   min = -std::numeric_limits<double>::max();
   max = std::numeric_limits<double>::max();
   p = 1.0;
   reset();
};

/// ----------------------------------------------------------------------------
CColorMap::~CColorMap() {}

/// ----------------------------------------------------------------------------
void CColorMap::load(const std::string &filename)
{
   std::ifstream ifs(filename.c_str());
   SColor color;
   color.alpha = 1.0;
   colors.clear();
   while (ifs >> color.red >> color.green >> color.blue) {
      colors.push_back(color);
   }
   assert_argument(colors.size() > 1, "At least 2 colors must be loaded");
   reset();
}

/// ----------------------------------------------------------------------------
int CColorMap::getNumColors(void) const { return colors.size(); }

/// ----------------------------------------------------------------------------
void CColorMap::reset(void) { cur = colors.begin(); }

/// ----------------------------------------------------------------------------
const SColor &CColorMap::next(void)
{
   assert_argument(colors.size() > 0, "Color map does not contain colors");
   if (++cur == colors.end()) {
      cur = colors.begin();
   }
   return *cur;
}

/// ----------------------------------------------------------------------------
const SColor &CColorMap::getColor(int i) const
{
   assert_argument(
       i >= 0 && i < colors.size(),
       "Requested color number is out of the curret colormap range");
   return colors[i];
}

/// ----------------------------------------------------------------------------
void CColorMap::setRange(double minI, double maxI)
{
   min = minI;
   max = maxI;
   p = (colors.size() - 1) / (max - min); // max must be color.size() - 1
}

/// ----------------------------------------------------------------------------
const SColor &CColorMap::getColor(double v) const
{
   return getColor(int(p * (v - min)));
}

/// ----------------------------------------------------------------------------
int CColorMap::getColorIndex(double v) const
{
   //  DEBUG("p: " << p << " v: " << v << " min: " << min);
   return int(p * (v - min));
}

/* end of colormap.cc */
