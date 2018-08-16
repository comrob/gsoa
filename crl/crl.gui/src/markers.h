/*
 * File name: markers.h
 * Date:      2007/02/17 19:33
 * Author:    Jan Faigl
 */

#include <cairo.h>

#include <crl/gui/shape.h>

#ifndef __MARKERS_H__
#define __MARKERS_H__

using crl::gui::CShape;

namespace crl { namespace gui { namespace cairo { namespace markers {

   class CCirclePoint {
    public:
      // TODO introduce shapes
      static void draw(cairo_t *cr, double xc, double yc, CShape &shape);
      static bool in_stroke(cairo_t *cr, double xc, double yc, double sx,
                            double sy, CShape &shape);
   };

}}}} // end namespace crl::gui::cairo::markers

#endif

/* end of markers.h */
