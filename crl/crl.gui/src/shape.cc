/*
 * File name: shape.cc
 * Date:      2005/10/21 16:41
 * Author:    Jan Faigl
 */

#include <cstring>
#include <sstream>

#include <crl/assert.h>
#include <crl/stringconversions.h>

#include <crl/gui/shape.h>

using namespace crl::gui;

bool get_color(const char *strname, double *colors);

typedef enum {
   PEN_COLOR = 0,
   FILL_COLOR,
   PEN_ALPHA,
   FILL_ALPHA,
   WIDTH,
   RADIUS,
   ZOOM,
   FILL,
   OUTLINE,
   DEPTH,
   ATTR_NUM
} TAttribute;

std::string AttributeStr[] = {
    "pen_color", "fill_color", "pen_alpha", "fill_alpha", "width",
    "radius",    "zoom",       "fill",      "outline",    "depth"};

/// ----------------------------------------------------------------------------
void set_color(const std::string &colorname, SColor &color)
{
   crl::assert_argument(get_color(colorname.c_str(), (double *)&color),
                        "Color " + colorname + " has not been found!");
   color.alpha = 1.0;
}

/// ----------------------------------------------------------------------------
/// struct SColor
/// ----------------------------------------------------------------------------
SColor::SColor(const std::string &colorname)
{
   crl::assert_argument(get_color(colorname.c_str(), (double *)this),
                        "Color " + colorname + " has not been found!");
   alpha = 1.0;
}

/// ----------------------------------------------------------------------------
/// Class CShape
/// ----------------------------------------------------------------------------
const bool CShape::ZOOM = false;
const bool CShape::FILL = true;
const bool CShape::OUTLINE = true;
const double CShape::PEN_ALPHA = 1.0;
const double CShape::FILL_ALPHA = 1.0;

/// static method --------------------------------------------------------------
std::string CShape::getStringOptions(void)
{
   std::ostringstream os;
   os << "pen_color:"
      << "STR;fill_color:"
      << "STR;";
   os << "pen_alpha:"
      << "FLOAT;fill_alpha:"
      << "FLOAT;";
   os << "width:"
      << "INT;"
      << "radius:"
      << "INT"
      << ";zoom:"
      << "0/1;";
   os << "fill:"
      << "0/1;"
      << "outline:"
      << "0/1;"
      << "depth:"
      << "INT;";
   return os.str();
}

/// static method --------------------------------------------------------------
SColor CShape::getColor(std::string colorname)
{
   SColor color;
   assert_argument(get_color(colorname.c_str(), (double *)&color),
                   "Color " + colorname + " has not been found!");
   color.alpha = 1.0;
   return color;
}

/// ----------------------------------------------------------------------------
CShape::CShape()
{
   std::string pColor("black");
   std::string fColor("white");
   penColor.alpha = 1.0;
   fillColor.alpha = 1.0;
   init(pColor, fColor, 1, 1, CShape::PEN_ALPHA, CShape::FILL_ALPHA,
        CShape::ZOOM, CShape::FILL, CShape::OUTLINE, DEFAULT_DEPTH);
}

/// ----------------------------------------------------------------------------
CShape::CShape(const CShape &shape)
{
   init(shape.penColor, shape.fillColor, shape.width, shape.radius,
        shape.penColor.alpha, shape.fillColor.alpha, shape.zoom, shape.fill,
        shape.outline, shape.depth);
   strPenColor = shape.strPenColor;
   strFillColor = shape.strFillColor;
}

/// ----------------------------------------------------------------------------
CShape::CShape(CShape &shape)
{
   init(shape.penColor, shape.fillColor, shape.width, shape.radius,
        shape.penColor.alpha, shape.fillColor.alpha, shape.zoom, shape.fill,
        shape.outline, shape.depth);
   strPenColor = shape.strPenColor;
   strFillColor = shape.strFillColor;
}

/// ----------------------------------------------------------------------------
CShape::CShape(const char *iPenColor, const char *iFillColor, int iWidth,
               int iRadius, const double penAlpha, const double fillAlpha,
               int depth)
{
   std::string sP(iPenColor);
   std::string sF(iFillColor);
   init(sP, sF, iWidth, iRadius, penAlpha, fillAlpha, ZOOM, FILL, OUTLINE,
        depth);
}

/// ----------------------------------------------------------------------------
CShape::CShape(const std::string &shape)
{
   std::string pColor("black");
   std::string fColor("white");
   penColor.alpha = 1.0;
   fillColor.alpha = 1.0;
   init(pColor, fColor, 1, 1, CShape::PEN_ALPHA, CShape::FILL_ALPHA,
        CShape::ZOOM, CShape::FILL, CShape::OUTLINE, DEFAULT_DEPTH);
   setShape(shape);
}

/// ----------------------------------------------------------------------------
CShape &CShape::operator=(const CShape &other)
{
   if (this != &other) {
      init(other.penColor, other.fillColor, other.width, other.radius,
           other.penColor.alpha, other.fillColor.alpha, other.zoom, other.fill,
           other.outline, other.depth);
      strPenColor = other.strPenColor;
      strFillColor = other.strFillColor;
   }
   return *this;
}

/// ----------------------------------------------------------------------------
std::string CShape::operator()(void) const
{
   std::stringstream ss;
   ss << *this;
   return ss.str();
}

/// ----------------------------------------------------------------------------
void CShape::init(const std::string &iPenColor, const std::string &iFillColor,
                  const int iWidth, const int iRadius, const double penAlpha,
                  const double fillAlpha, bool iZoom, bool iFill, bool iOutline,
                  int iDepth)
{
   setPenColor(iPenColor);
   setFillColor(iFillColor);
   width = iWidth;
   radius = iRadius;
   penColor.alpha = penAlpha;
   fillColor.alpha = fillAlpha;
   zoom = iZoom;
   setFill(iFill);
   setOutline(iOutline);
   depth = iDepth;
}

/// ----------------------------------------------------------------------------
void CShape::init(const SColor &iPenColor, const SColor &iFillColor,
                  const int iWidth, const int iRadius, const double penAlpha,
                  const double fillAlpha, const bool iZoom, const bool iFill,
                  const bool iOutline, int iDepth)
{
   penColor = iPenColor;
   fillColor = iFillColor;
   width = iWidth;
   radius = iRadius;
   penColor.alpha = penAlpha;
   fillColor.alpha = fillAlpha;
   zoom = iZoom;
   setFill(iFill);
   setOutline(iOutline);
   depth = iDepth;
}

/// ----------------------------------------------------------------------------
void CShape::setPenColor(const std::string &color)
{
   if (color.empty()) {
      outline = false;
   } else {
      set_color(color, penColor);
      strPenColor = color;
   }
}

/// ----------------------------------------------------------------------------
void CShape::setFillColor(const std::string &color)
{
   if (color.empty()) {
      fill = false;
   } else {
      set_color(color, fillColor);
      strFillColor = color;
   }
}

/// ----------------------------------------------------------------------------
void CShape::setPenColor(const SColor &color)
{
   penColor.set(color.red, color.green, color.blue, color.alpha);
}

/// ----------------------------------------------------------------------------
void CShape::setFillColor(const SColor &color) { fillColor = color; }

/// ----------------------------------------------------------------------------
void CShape::setPenAlpha(double alpha) { penColor.alpha = alpha; }

/// ----------------------------------------------------------------------------
void CShape::setFillAlpha(double alpha) { fillColor.alpha = alpha; }

/// ----------------------------------------------------------------------------
void CShape::setPenWidth(const int iWidth) { width = iWidth; }

/// ----------------------------------------------------------------------------
void CShape::setRadius(const int iRadius) { radius = iRadius; }

/// ----------------------------------------------------------------------------
void CShape::setZoom(bool iZoom) { zoom = iZoom; }

/// ----------------------------------------------------------------------------
void CShape::setFill(bool iFill)
{
   fill = iFill;
   /*  if (strFillColor.size() > 0) {
   }*/ // TODO
}

/// ----------------------------------------------------------------------------
void CShape::setOutline(bool iOutline)
{
   outline = iOutline;
   /*   if (strPenColor.size() > 0) {
        }
        */
}

/// ----------------------------------------------------------------------------
void CShape::setDepth(int iDepth) { depth = iDepth; }

/// ----------------------------------------------------------------------------
CShape &CShape::setShape(const std::string &shape)
{
   std::stringstream str(shape);
   str >> *this;
   return *this;
}

/// ----------------------------------------------------------------------------
std::ostream &operator<<(std::ostream &os, const crl::gui::CShape &shape)
{
   os << "pen_color:" << shape.getPenColorStr()
      << ";fill_color:" << shape.getFillColorStr() << ";";
   os << "pen_alpha:" << shape.getPenColor().alpha
      << ";fill_alpha:" << shape.getFillColor().alpha << ";";
   os << "width:" << shape.getPenWidth() << ";radius:" << shape.getRadius()
      << ";zoom:" << shape.getZoom() << ";";
   os << "fill:" << shape.getFill() << ";outline:" << shape.getOutline()
      << ";depth:" << shape.getDepth() << ";";
   return os;
}

/// ----------------------------------------------------------------------------
TAttribute getAttribute(std::string str)
{ // SPEEDUP case insensitive map
   TAttribute ret = ATTR_NUM;
   if (str.size() > 1) {
      const char *s = str.c_str();
      if (str.back() == ':') {
	 str.pop_back();
	 s = str.c_str();
      }
      //const char *s =
      //    (str[str.size() - 1] == ':' ? str.substr(0, str.size() - 1).c_str()
      //                               : str.c_str());
      for (int i = 0; i < ATTR_NUM; i++) {
         if (strcasecmp(AttributeStr[i].c_str(), s) == 0) {
            ret = (TAttribute)i;
            break;
         }
      }
   }
   return ret;
}

/// ----------------------------------------------------------------------------
std::istream &getToken(std::istream &is, std::string &token)
{
   token.clear();
   char c;
   while (is >> c) {
      if (c == ':' || c == ';' || c == '=' || c == ' ' || c == ',') {
         break;
      } else {
         token.push_back(c);
      }
   } // end while loop
   if (token.size() > 0) {
      is.clear();
   }
   return is;
}

/// ----------------------------------------------------------------------------
std::istream &operator>>(std::istream &is,
                         crl::gui::CShape &shape) throw()
{
   bool attributes[ATTR_NUM];
   for (int i = 0; i < ATTR_NUM; i++) {
      attributes[i] = false;
   }
   bool q = false;
   std::string name;
   std::string value;
   double d;
   int i;
   bool b;

   while (is && !q) {
      if (getToken(is, name) && getToken(is, value)) {
         TAttribute at = getAttribute(name);
         crl::assert_argument(at != ATTR_NUM,
                              "Unknown attribute '" + name + "'");
         if (!attributes[at]) {
            switch (at) {
            case PEN_COLOR:
               shape.setPenColor(value);
               break;
            case FILL_COLOR:
               shape.setFillColor(value);
               break;
            case PEN_ALPHA:
               crl::assert_argument(crl::string_cast<double>(value, d),
                                    "Can not parse pen  alpha from " + value);
               shape.setPenAlpha(d);
               break;
            case FILL_ALPHA:
               crl::assert_argument(crl::string_cast<double>(value, d),
                                    "Can not parse fill alpha from " + value);
               shape.setFillAlpha(d);
               break;
            case WIDTH:
               crl::assert_argument(crl::string_cast<int>(value, i),
                                    "Can not parse pen with from " + value);
               shape.setPenWidth(i);
               break;
            case RADIUS:
               crl::assert_argument(crl::string_cast<int>(value, i),
                                    "Can not parse radius from " + value);
               shape.setRadius(i);
               break;
            case ZOOM:
               crl::assert_argument(crl::string_cast<bool>(value, b),
                                    "Can not parse zoom from " + value);
               shape.setZoom(b);
               break;
            case FILL:
               crl::assert_argument(crl::string_cast<bool>(value, b),
                                    "Can not parse fill from " + value);
               shape.setFill(b);
               break;
            case OUTLINE:
               crl::assert_argument(crl::string_cast<bool>(value, b),
                                    "Can not parse outline from " + value);
               shape.setOutline(b);
               break;
            case DEPTH:
               crl::assert_argument(crl::string_cast<int>(value, i),
                                    "Can not parse depth from " + value);
               shape.setDepth(i);
               break;
            default:
               q = true;
               break;
            } // end switch
            attributes[at] = true;
	 } else { // move back
            q = false;
         }
      }
   }
   return is;
}

/* end of shape.cc */
