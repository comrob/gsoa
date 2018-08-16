/*
 * File name: canvas.cc
 * Date:      2008/09/07 10:27
 * Author:    Jan Faigl
 */

#include <cmath>

#include <crl/gui/canvas.h>

#include <crl/gui/mouseclick.h>
#include <crl/gui/waitforclick.h>

using namespace crl::gui;

/// ----------------------------------------------------------------------------
/// Class CCanvasBase
/// ----------------------------------------------------------------------------
CCanvasBase::CCanvasBase(CGui &gui)
    : gui(gui), renderer(gui.getRenderer()), shape("black", "black", 1, 1)
{
   dNum = 0;
   param = canvas::POINT;
   colorParam = canvas::PEN_COLOR;
   renderOpen = false;
   clearFlag = false;
   tags = "default";
   arrowAngle = 30 * M_PI / 180.0; // 30 degreess
   arrowLength = 10.0;             // in pixels
   arrowMinLength = -1;            //-1 disable
}

/// ----------------------------------------------------------------------------
CCanvasBase::~CCanvasBase() {}

/// ----------------------------------------------------------------------------
CRenderer *CCanvasBase::getRenderer(void) { return renderer; }

/// ----------------------------------------------------------------------------
void CCanvasBase::redraw(void) { renderer->redraw(); };

/// ----------------------------------------------------------------------------
void CCanvasBase::click(void)
{
   redraw();
   CWaitForClick click(gui);
}

/// ----------------------------------------------------------------------------
void CCanvasBase::click(double &x, double &y)
{
   redraw();
   CMouseClick(gui).getMouseClick(x, y);
}

/// ----------------------------------------------------------------------------
void CCanvasBase::clear(const std::string &tags)
{
   renderer->clearObjects(tags);
}

/// ----------------------------------------------------------------------------
void CCanvasBase::drawPoint(const double x, const double y)
{
   renderer->drawPoint(x, y, tags, shape);
}

/// ----------------------------------------------------------------------------
void CCanvasBase::drawLine(const double x1, const double y1, const double x2,
                           const double y2)
{
   renderer->open(render::LINE, tags, shape);
   renderer->addPoint(x1, y1);
   renderer->addPoint(x2, y2);
   renderer->close();
}

/// ----------------------------------------------------------------------------
CShape CCanvasBase::getShape(void) { return shape; }

/// ----------------------------------------------------------------------------
std::string CCanvasBase::getTags(void) { return tags; }

/// ----------------------------------------------------------------------------
CCanvasBase &CCanvasBase::operator<<(const char *str)
{
   if (clearFlag) {
      clear(std::string(str));
      clearFlag = false;
   } else {
      tags = std::string(str);
   }
   return *this;
}

/// ----------------------------------------------------------------------------
CCanvasBase &CCanvasBase::operator<<(const std::string &str)
{
   if (clearFlag) {
      clear(str);
      clearFlag = false;
   } else {
      tags = str;
   }
   return *this;
}

/// ----------------------------------------------------------------------------
CCanvasBase &CCanvasBase::operator<<(const CShape &iShape)
{
   shape = iShape;
   return *this;
}

/// ----------------------------------------------------------------------------
CCanvasBase &CCanvasBase::operator<<(const PenColor &penColor)
{
   shape.setPenColor(penColor());
   return *this;
}

/// ----------------------------------------------------------------------------
CCanvasBase &CCanvasBase::operator<<(const FillColor &fillColor)
{
   shape.setFillColor(fillColor());
   return *this;
}

/// ----------------------------------------------------------------------------
CCanvasBase &CCanvasBase::operator<<(const Fill &fill)
{
   shape.setFill(fill());
   return *this;
}

/// ----------------------------------------------------------------------------
CCanvasBase &CCanvasBase::operator<<(const Outline &outline)
{
   shape.setOutline(outline());
   return *this;
}

/// ----------------------------------------------------------------------------
CCanvasBase &CCanvasBase::operator<<(const Width &width)
{
   shape.setPenWidth(width());
   return *this;
}

/// ----------------------------------------------------------------------------
CCanvasBase &CCanvasBase::operator<<(const Radius &radius)
{
   shape.setRadius(radius());
   return *this;
}

/// ----------------------------------------------------------------------------
CCanvasBase &CCanvasBase::operator<<(const Depth &depth)
{
   shape.setDepth(depth());
   return *this;
}

/// ----------------------------------------------------------------------------
CCanvasBase &CCanvasBase::operator<<(const DepthChange &depth)
{
   shape.setDepth(depth() + shape.getDepth());
   return *this;
}

/// ----------------------------------------------------------------------------
CCanvasBase &CCanvasBase::operator<<(const PenAlpha &alpha)
{
   shape.setPenAlpha(alpha());
   return *this;
}

/// ----------------------------------------------------------------------------
CCanvasBase &CCanvasBase::operator<<(const FillAlpha &alpha)
{
   shape.setFillAlpha(alpha());
   return *this;
}

/// ----------------------------------------------------------------------------
CCanvasBase &CCanvasBase::operator<<(const ArrowAngle &angle)
{
   arrowAngle = angle();
   return *this;
}

/// ----------------------------------------------------------------------------
CCanvasBase &CCanvasBase::operator<<(const ArrowLength &length)
{
   arrowLength = length();
   return *this;
}

/// ----------------------------------------------------------------------------
CCanvasBase &CCanvasBase::operator<<(const ArrowMinLength &minLength)
{
   arrowMinLength = minLength();
   return *this;
}

/// ----------------------------------------------------------------------------
CCanvasBase &CCanvasBase::operator<<(const canvas::TParam &p)
{
   switch (p) {
   case canvas::PEN_COLOR:
   case canvas::FILL_COLOR:
      colorParam = p;
      break;
   case canvas::POINT:
      param = p;
      dNum = 0;
      break;
   case canvas::POLYGON:
      if (renderOpen) {
         renderer->reset();
         renderOpen = false;
      }
      param = p;
      dNum = 0;
      break;
   case canvas::ARROW_LINE:
   case canvas::LINE:
   case canvas::ARC:
   case canvas::SEGMENT_SET:
      if (renderOpen) {
         renderer->reset();
         renderOpen = false;
      }
      param = p;
      dNum = 0;
      break;
   case canvas::LINESTRING:
      if (renderOpen) {
         renderer->reset();
         renderOpen = false;
      }
      param = p;
      dNum = 0;
      break;
   case canvas::AREA:
      if (renderOpen) {
         renderer->reset();
         renderOpen = false;
      }
      param = p;
      dNum = 0;
      break;
   case canvas::END:
      if (renderOpen) {
         renderer->close();
         renderOpen = false;
      }
      dNum = 0;
      break;
   case canvas::CLICK:
      click();
      break;
   case canvas::CLEAR:
      clearFlag = true;
      break;
   case canvas::REDRAW:
      redraw();
      break;
   case canvas::PEN_ALPHA:
   case canvas::FILL_ALPHA:
      param = p;
      break;
   default: // filter unused param
      break;
   } // switch
   return *this;
}

/// ----------------------------------------------------------------------------
CCanvasBase &CCanvasBase::operator<<(const double &d)
{
   switch (param) {
   case canvas::POINT:
      if (dNum == 1) {
         renderer->drawPoint(x, d, tags, shape);
         dNum = 0;
      } else {
	 x = d;
         dNum++;
      }
      break;
   case canvas::POLYGON:
      switch (dNum) {
      case 0: // x of 1st point
         x = d;
         dNum++;
         break;
      case 1: // 1st point
         renderer->open(render::POLYGON, tags, shape);
         renderOpen = true;
         renderer->addPoint(x, d);
         dNum++;
         break;
      default:
         dNum++;
         if (dNum % 2) {
            x = d;
         } else { // y coord
            renderer->addPoint(x, d);
         }
         break;
      }
      break;
   case canvas::ARROW_LINE:
      dNum++;
      switch (dNum) {
      case 1: // x of 1st point
      case 3: // next x-coord
         x = d;
         v[dNum - 1] = d;
         break;
      case 2: // 1st point
         renderer->open(render::LINE, tags, shape);
         renderOpen = true;
         renderer->addPoint(x, d);
         v[dNum - 1] = d;
         break;
      case 4: // line is fully defined
         renderer->addPoint(x, d);
         renderer->close();
         renderOpen = false;
         v[dNum - 1] = d;
         drawArrow();
         dNum = 0;
         break;
      } // end dNum switch
      break;
   case canvas::LINE:
      dNum++;
      switch (dNum) {
      case 1: // x of 1st point
      case 3: // next x-coord
         x = d;
         break;
      case 2: // 1st point
         renderer->open(render::LINE, tags, shape);
         renderOpen = true;
         renderer->addPoint(x, d);
         break;
      case 4: // line is fully defined
         renderer->addPoint(x, d);
         renderer->close();
         renderOpen = false;
         dNum = 0;
         break;
      } // end dNum switch
      break;

   case canvas::LINESTRING:
      dNum++;
      switch (dNum) {
      case 1: // x of 1st point
      case 3: // next x-coord
         x = d;
         break;
      case 2: // 1st point
         renderer->open(render::LINE, tags, shape);
         renderOpen = true;
         renderer->addPoint(x, d);
         break;
      case 4: // line is fully defined
         renderer->addPoint(x, d);
         renderer->close();
         renderer->open(render::LINE, tags, shape); // start new line
                                                    // immediately
         renderer->addPoint(x, d);
         dNum = 2;
         break;
      }
      break;
   case canvas::SEGMENT_SET:
      dNum++;
      switch (dNum) {
      case 1: // x of 1st point
      case 3: // next x-coord
         x = d;
         break;
      case 2: // 1st point
         renderer->open(render::SEGMENT_SET, tags, shape);
         renderOpen = true;
         renderer->addPoint(x, d);
         break;
      case 4: // line is fully defined
         renderer->addPoint(x, d);
         dNum = 2; // start next point
         break;
      }
      break;

   case canvas::AREA:
      switch (dNum) {
      case 0: // x of 1st point
         x = d;
         dNum++;
         break;
      case 1: // 1st point
         renderer->open(render::AREA, tags, shape);
         renderOpen = true;
         renderer->addPoint(x, d);
         dNum++;
         break;
      default:
         dNum++;
         if (dNum % 2) {
            x = d;
         } else { // y coord
            renderer->addPoint(x, d);
         }
         break;
      }
      break;
   case canvas::ARC:
      switch (dNum) {
      case 0: // x of 1st point is the center
         x = d;
         dNum++;
         break;
      case 1: // 1st point
         renderer->open(render::ARC, tags, shape);
         renderOpen = true;
         renderer->addPoint(x, d);
         dNum++;
         break;
      case 2:
         renderer->addPoint(d, 0.0); // radius as point (r, 0.0)
         dNum++;
         break;
      case 3: // angle1
         x = d;
         dNum++;
         break;
      default: // angle2
         renderer->addPoint(x, d);
         renderer->close();
         renderOpen = false;
         dNum = 0;
         break;
      }
      break;
   case canvas::PEN_ALPHA:
      shape.setPenAlpha(d);
      break;
   case canvas::FILL_ALPHA:
      shape.setFillAlpha(d);
      break;
   default:
      break;
   } // end switch
   return *this;
}

/// ----------------------------------------------------------------------------
CCanvasBase &CCanvasBase::operator<<(const SColor &color)
{
   if (colorParam == canvas::PEN_COLOR) {
      shape.setPenColor(color);
   } else {
      shape.setFillColor(color);
   }
   return *this;
}

/// ----------------------------------------------------------------------------
CCanvasBase &CCanvasBase::operator<<(const CBBox &bbox)
{
   return *this << bbox.minx() << bbox.miny() << bbox.maxx() << bbox.miny()
                << bbox.maxx() << bbox.maxy() << bbox.minx() << bbox.maxy();
}

/// ----------------------------------------------------------------------------
CCanvasBase &CCanvasBase::operator>>(Depth &depth)
{
   depth.set(shape.getDepth());
   return *this;
}

/// ----------------------------------------------------------------------------
bool CCanvasBase::save(const std::string &filename)
{
   return renderer->save(filename);
}

/// - protected method ---------------------------------------------------------
void CCanvasBase::drawArrow()
{

   double v2[4];
   const CViewTransform &tr = renderer->getTransform();
   tr.real2view(v[0], v[1], v2[0], v2[1]);
   tr.real2view(v[2], v[3], v2[2], v2[3]);
   bool drawArrow = true;
   if (arrowMinLength > 0) {
      const double dx = v2[2] - v2[0];
      const double dy = v2[3] - v2[1];
      drawArrow = arrowMinLength * arrowMinLength < (dx * dx + dy * dy);
   }
   if (drawArrow) {
      const double angle = atan2(v2[3] - v2[1], v2[2] - v2[0]) + M_PI;
      *this << (shape.getFill() ? canvas::POLYGON : canvas::LINESTRING)
            << tr.view2realX(v2[2] + arrowLength * cos(angle - arrowAngle))
            << tr.view2realY(v2[3] + arrowLength * sin(angle - arrowAngle))
            << v[2] << v[3]
            << tr.view2realX(v2[2] + arrowLength * cos(angle + arrowAngle))
            << tr.view2realY(v2[3] + arrowLength * sin(angle + arrowAngle))
            << canvas::END << canvas::ARROW_LINE;
   }
}

/// ----------------------------------------------------------------------------
/// Operator for global scope
/// ----------------------------------------------------------------------------
crl::gui::CCanvasBase *operator<<(crl::gui::CCanvasBase *canvas,
                                  const std::string &iTags)
{
   *canvas << iTags;
   return canvas;
}

/// ----------------------------------------------------------------------------
crl::gui::CCanvasBase *operator<<(crl::gui::CCanvasBase *canvas,
                                  const crl::gui::CShape &shape)
{
   *canvas << shape;
   return canvas;
}

/* end of canvas.cc */
