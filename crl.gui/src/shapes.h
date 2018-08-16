/*
 * File name: shapes.h
 * Date:      2005/10/21 16:56
 * Author:    Jan Faigl
 */

#ifndef __SHAPES_H__
#define __SHAPES_H__

#include <crl/gui/shape.h>

namespace crl { namespace gui {

   ///
   /// However shapes are problem oriented these shapes are general
   /// and can be shared with several modules
   ///
   namespace Shape {
      extern CShape MAP_BORDER;
      extern CShape MAP_OBSTACLE;
      extern CShape MAP_VERTEX;
      extern CShape MAP_BORDER_HIGHLIGHT;
      extern CShape MAP_OBSTACLE_HIGHLIGHT;
      extern CShape FREESPACE;
      extern CShape POINT;
      extern CShape RED_POINT;
      extern CShape ORANGE_POINT;
      extern CShape BROWN_POINT;
      extern CShape BLUE_POINT;
      extern CShape GREEN_POINT;
      extern CShape POINT2;
      extern CShape GOLD_POINT;
      extern CShape WHITE_POINT;
      extern CShape CITY;
      extern CShape CITY_GREEN;
      extern CShape CITY_EMPTY;
      extern CShape DEPOT;
      extern CShape RING;
      extern CShape REGION;
      extern CShape REGION_GOLD;
      extern CShape NEURON;
      extern CShape BLUE_LINE;
      extern CShape BLUE_BOLD_LINE;
      extern CShape RED_LINE;
      extern CShape RED_BOLD_LINE;
      extern CShape GREEN_LINE;
      extern CShape GREEN_BOLD_LINE;
      extern CShape BLACK_BOLD_LINE;
      extern CShape GOLD_BOLD_LINE;
      extern CShape POLYGON;
      extern CShape POLYGON_EMPTY;
      extern CShape POLYGON_FILL;
      extern CShape SEAGREEN_BOLD_LINE;
      extern CShape CITY_VISIBILITY_LINE;
      extern CShape WHITE_EXTRA_BOLD_LINE;
      extern CShape DODGER_BLUE_EXTRA_BOLD_LINE;
      extern CShape CYAN_EXTRA_POINT;
      extern CShape TAN_EXTRA_POINT;
      extern CShape FREESPACE_HIGHLIGHT;
      extern CShape BLACK_EXTRA_BOLD_LINE;
      extern CShape GOLD_EXTRA_BOLD_LINE;
   } // namespace Shape

}} // namespace crl::gui

#endif

/* end of shapes.h */
