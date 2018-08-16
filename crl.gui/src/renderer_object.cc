/*
 * File name: renderer_object.cc
 * Date:      2009/02/21 07:41
 * Author:    Jan Faigl
 */

#include <ostream>

#include <crl/gui/renderer_object.h>

using namespace crl::gui::render;

/// ----------------------------------------------------------------------------
/// Struct SRendererObject
/// ----------------------------------------------------------------------------
SRendererObject::SRendererObject(ObjectType type, const CShape &shape)
    : shape(shape), type(type), size(0)
{
}

/// ----------------------------------------------------------------------------
void SRendererObject::addPoint(const double &x, const double &y)
{
   size++;
   coords.push_back(Coord(x, y));
}

/// ----------------------------------------------------------------------------
void SRendererObject::simplePrint(std::ostream &out)
{
   CCoordsTransform transform;
   simplePrint(out, transform);
}

/// ----------------------------------------------------------------------------
void SRendererObject::simplePrint(std::ostream &out,
                                  const CCoordsTransform &transform)
{
   switch (type) {
   case AREA:
      out << "[AREA]";
      break;
   case POLYGON:
      out << "[POLYGON]";
      break;
   case LINE:
      if (size == 2) {
         out << "[SEGMENT]";
      } else {
         out << "[LINESTRING]";
      }
      break;
   case POINT:
      out << "[POINT]";
      break;
   case ARC:
      out << "[ARC]";
      break;
   case SEGMENT_SET:
      out << "[SEGMENT_SET]";
      break;
   default:
      out << "[RENDER_OBJECT]";
      break;
   };
   out << std::endl;
   for (Coords::iterator it = coords.begin(); it != coords.end(); it++) {
      out << transform.getX(it->x) << " " << transform.getY(it->y) << std::endl;
   }
   out << std::endl;
}

/* end of renderer_object.cc */
