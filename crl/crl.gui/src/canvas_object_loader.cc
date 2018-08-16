/*
 * File name: canvas_object_loader.cc
 * Date:      2009/07/25 22:43
 * Author:    Jan Faigl
 */

#include <fstream>

#include <crl/file_utils.h>
#include <crl/logging.h>
#include <crl/stringconversions.h>

#include <crl/gui/shape.h>
#include <crl/gui/canvas_object_loader.h>

using namespace crl::gui;

using crl::logger;

/// ----------------------------------------------------------------------------
CCanvasObjectLoader::CCanvasObjectLoader(const std::string &filename,
                                         CCanvasBase &canvas)
{
   std::ifstream in(filename.c_str());
   load(in, canvas);
}

/// - protected method ---------------------------------------------------------
void CCanvasObjectLoader::load(std::istream &in, CCanvasBase &canvas)
{
   std::string line;
   int state = 0; // 0 text, 1 doubles
   std::string::size_type i = std::string::npos;
   double x;
   double y;
   while (in) {
      if (state == 0) { // process text
         if (getline(in, line) && !line.empty()) {
            if (line[0] == '#') { // skip comment
	    } else if (line == "[OBJECT]") {
               canvas << canvas::END; // close current opened object
	    } else if (line == "[POLYGON]") {
               canvas << canvas::POLYGON;
               state = 1;
	    } else if (line == "[AREA]") {
               canvas << canvas::AREA;
               state = 1;
	    } else if (line == "[SEGMENT]") {
               canvas << canvas::LINE;
               state = 1;
	    } else if (line == "[LINESTRING]") {
               canvas << canvas::LINESTRING;
               state = 1;
	    } else if (line == "[POINT]") {
               canvas << canvas::POINT;
               state = 1;
	    } else if (line[0] == '[') {
               WARN("Unsupported object type on line: '" << line << "'");
	    } else if ((i = line.find("TAGS=")) != std::string::npos) {
               canvas << line.substr(i + 5);
	    } else if ((i = line.find("SHAPE=")) != std::string::npos) {
               canvas << CShape(line.substr(i + 6));
	    } else {
               WARN("Unsupported line: '" << line << "'");
            }
         }
         // end process text
      }
      else { // parse doubles
         if (in >> x >> y) {
            canvas << x << y;
	 } else {
            canvas << canvas::END;
            state = 0; // switch back to text
            in.clear();
         }
      } // end parse doubles
   }    // end while in
}

/* end of canvas_object_loader.cc */
