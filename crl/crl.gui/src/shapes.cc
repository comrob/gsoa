/*
 * File name: shapes.cc
 * Date:      2005/10/23 11:41
 * Author:    Jan Faigl
 */

#include <crl/gui/shapes.h>

using namespace crl::gui;

/*
   CShape Shape::MAP_OBSTACLE("gray", "gray", 0, 0);
   CShape Shape::POINT("red", "black", 1, 4);
   CShape Shape::POINT2("blue", "black", 1, 4);
   CShape Shape::CITY("green", "black", 1, 4);
   CShape Shape::DEPOT("green", "black", 3, 5);
   CShape Shape::RING("blue", "", 3, 0);
   CShape Shape::REGION("lightblue", "lightblue", 0, 2);
   CShape Shape::NEURON("magenta4", "black", 1, 4);
   CShape Shape::BLUE_LINE("blue", "black", 1, 0);
   */

// CShape Shape::MAP_BORDER("gray", "whitesmoke", 0, 2); //old BORDER
CShape Shape::MAP_BORDER("bisque3", "white", 1, 2);
CShape Shape::MAP_OBSTACLE("bisque3", "bisque3", 0, 0);
// CShape Shape::MAP_BORDER("gray", "white", 1, 2);
// CShape Shape::MAP_OBSTACLE("gray", "gray", 0, 0);
CShape Shape::MAP_VERTEX("black", "DarkOrange1", 1, 3);
CShape Shape::MAP_BORDER_HIGHLIGHT("black", "linen", 1, 2);
CShape Shape::MAP_OBSTACLE_HIGHLIGHT("gray", "green", 0, 0);
CShape Shape::FREESPACE("", "SkyBlue1", 10, 0);
CShape Shape::POINT("black", "red", 1, 4);
CShape Shape::RED_POINT("black", "red", 1, 4);
CShape Shape::ORANGE_POINT("black", "orange", 1, 4);
CShape Shape::BROWN_POINT("black", "brown", 1, 4);
CShape Shape::BLUE_POINT("black", "blue", 1, 4);
CShape Shape::GREEN_POINT("black", "green", 1, 4);
CShape Shape::POINT2("black", "blue", 1, 4);
CShape Shape::GOLD_POINT("black", "gold", 1, 4);
CShape Shape::WHITE_POINT("black", "white", 1, 4);
CShape Shape::CITY("black", "green", 1, 4);
CShape Shape::CITY_GREEN("", "green", 1, 4);
CShape Shape::CITY_EMPTY("black", "", 1, 4);
CShape Shape::DEPOT("black", "green", 3, 5);
CShape Shape::RING("blue", "black", 3, 0);
CShape Shape::REGION("lightblue", "lightblue", 0, 2);
CShape Shape::REGION_GOLD("gold", "gold", 0, 2);
CShape Shape::NEURON("black", "lightblue", 1, 4);
// CShape Shape::NEURON("black", "magenta4", 1, 4);
CShape Shape::BLUE_LINE("blue", "black", 1, 0);
CShape Shape::BLUE_BOLD_LINE("blue", "black", 2, 0);
CShape Shape::RED_LINE("red", "black", 1, 0);
CShape Shape::RED_BOLD_LINE("red", "black", 2, 0);
CShape Shape::GREEN_LINE("green", "black", 1, 0);
CShape Shape::GREEN_BOLD_LINE("green", "black", 2, 0);
CShape Shape::BLACK_BOLD_LINE("black", "black", 2, 0);
CShape Shape::GOLD_BOLD_LINE("gold", "black", 2, 0);

CShape Shape::POLYGON("black", "green", 2, 0);
CShape Shape::POLYGON_EMPTY("red", "", 4, 0);
CShape Shape::POLYGON_FILL("", "blue", 10, 0);
CShape Shape::SEAGREEN_BOLD_LINE("seagreen", "black", 2, 0);
CShape Shape::CITY_VISIBILITY_LINE("mediumorchid4", "black", 1, 0);
CShape Shape::WHITE_EXTRA_BOLD_LINE("white", "black", 4, 0);
CShape Shape::DODGER_BLUE_EXTRA_BOLD_LINE("dodgerblue2", "black", 4, 0);
CShape Shape::CYAN_EXTRA_POINT("black", "cyan", 2, 6);
CShape Shape::TAN_EXTRA_POINT("black", "tan2", 2, 6);
CShape Shape::FREESPACE_HIGHLIGHT("white", "SkyBlue1", 2, 0);
CShape Shape::BLACK_EXTRA_BOLD_LINE("black", "black", 4, 0);
CShape Shape::GOLD_EXTRA_BOLD_LINE("gold", "black", 4, 0);

/* end of shapes.cc */
