/*
 * File name: colormap.h
 * Date:      2009/04/13 07:41
 * Author:    Jan Faigl
 */

#ifndef __COLORMAP_H__
#define __COLORMAP_H__

#include <string>
#include <vector>

#include <crl/gui/shape.h>

namespace crl { namespace gui {

   class CColorMap {
      typedef std::vector<SColor> ColorVector;

    public:
      CColorMap();
      ~CColorMap();

      void load(const std::string &filename);
      void addColor(const SColor &color) { colors.push_back(color); }

      int getNumColors(void) const;
      void reset(void);
      const SColor &next(void);
      const SColor &getColor(int i) const;

      void setRange(double minI, double maxI);
      const SColor &getColor(double v) const;
      int getColorIndex(double v) const;

      double getMin(void) const { return min; }
      double getMax(void) const { return max; }

    private:
      ColorVector colors;
      ColorVector::iterator cur;
      double min;
      double max;
      double p;
   };

}} // end namespace crl::gui

#endif

/* end of colormap.h */
