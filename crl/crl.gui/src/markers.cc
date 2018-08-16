/*
 * File name: markers.cc
 * Date:      2007/02/17 19:33
 * Author:    Jan Faigl
 */

#include <cmath>

#include <crl/gui/markers.h>

using namespace crl::gui::cairo::markers;
using crl::gui::SColor;

/// ----------------------------------------------------------------------------
/// Class CCirclePoint

/// - static method ------------------------------------------------------------
void CCirclePoint::draw(cairo_t *cr, double xc, double yc, CShape &shape)
{
   double radius = shape.getRadius();
   //  double xr = radius;
   //  double yr = xr;
   const SColor &penColor(shape.getPenColor());
   const SColor &fillColor(shape.getFillColor());

   cairo_new_path(cr);
   if (shape.getOutline()) {
      cairo_set_source_rgba(cr, penColor.red, penColor.green, penColor.blue,
                            penColor.alpha);
      cairo_arc(cr, xc, yc, radius, 0., 2 * M_PI);
      if (shape.getFill()) {
         cairo_save(cr);
         cairo_set_source_rgba(cr, fillColor.red, fillColor.green,
                               fillColor.blue, fillColor.alpha);
         cairo_fill_preserve(cr);
         cairo_restore(cr);
         cairo_set_line_width(cr, shape.getPenWidth());
      }
   } else {
      cairo_set_source_rgba(cr, fillColor.red, fillColor.green, fillColor.blue,
                            fillColor.alpha);
      cairo_arc(cr, xc, yc, radius, 0., 2 * M_PI);
      if (shape.getFill()) {
         cairo_fill(cr);
      }
   }
   cairo_stroke(cr);
}

/// - static method ------------------------------------------------------------
bool CCirclePoint::in_stroke(cairo_t *cr, double xc, double yc, double sx,
                             double sy, CShape &shape)
{
   bool ret = false;
   cairo_save(cr);
   cairo_new_path(cr);
   cairo_set_source_rgba(cr, 0., 0., 0., 0);
   cairo_arc(cr, xc, yc, shape.getRadius(), 0., 2 * M_PI);
   cairo_fill_preserve(cr);
   ret = cairo_in_fill(cr, sx, sy);
   cairo_restore(cr);
   return ret;
}

/* end of markers.cc */
