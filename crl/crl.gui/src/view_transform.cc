/*
 * File name: view_transform.cc
 * Date:      2009/02/21 16:06
 * Author:    Jan Faigl
 */

#include <cmath>
#include <cstdlib>
#include <sstream>

#include <crl/logging.h>

#include <crl/gui/view_transform.h>

using namespace crl::gui;

static crl::CLoggerPtr loclog = 0;

/// ----------------------------------------------------------------------------
static void init(void)
{
   if (!loclog) {
      loclog = crl::getLogger("gui");
   }
}

/// ----------------------------------------------------------------------------
/// Class CViewTransform
/// ----------------------------------------------------------------------------
CViewTransform::CViewTransform()
    : viewX(640), viewY(480), viewBorderX(10), viewBorderY(10), realMinX(0.),
      realMinY(0.), realMaxX(0.), realMaxY(0.), kX(viewX / realMaxX),
      kY(viewY / realMaxY), ratioX(fabs(kX)), ratioY(fabs(kY)), ar(-1.0)
{
   init();
   compute();
}

/// ----------------------------------------------------------------------------
CViewTransform::~CViewTransform() {}

/// ----------------------------------------------------------------------------
void CViewTransform::setView(double width, double height)
{
   viewX = width;
   viewY = height;
   compute();
}

/// ----------------------------------------------------------------------------
void CViewTransform::setAspectRation(double arI) { ar = arI; }

/// ----------------------------------------------------------------------------
void CViewTransform::setViewBorder(double borderX, double borderY)
{
   viewBorderX = borderX;
   viewBorderY = borderY;
   compute();
}

/// ----------------------------------------------------------------------------
void CViewTransform::setReal(double minX, double maxX, double minY, double maxY)
{
   realMinX = minX;
   realMaxX = maxX;
   realMinY = minY;
   realMaxY = maxY;
   compute();
}

/// ----------------------------------------------------------------------------
void CViewTransform::view2real(double x, double y, double &oX, double &oY) const
{
   oX = view2realX(x);
   oY = view2realY(y);
}

/// ----------------------------------------------------------------------------
void CViewTransform::real2view(double x, double y, double &oX, double &oY) const
{
   oX = real2viewX(x);
   oY = real2viewY(y);
}

/// ----------------------------------------------------------------------------
double CViewTransform::view2realX(double x) const
{
   return (x - viewBorderX) / kX + realMinX;
}

/// ----------------------------------------------------------------------------
double CViewTransform::view2realY(double y) const
{
   return realMinY - (y - viewY + viewBorderY) / kY;
}

/// ----------------------------------------------------------------------------
double CViewTransform::real2viewX(double x) const
{
   return kX * (x - realMinX) + viewBorderX;
}

/// ----------------------------------------------------------------------------
double CViewTransform::real2viewY(double y) const
{
   return viewY - kY * (y - realMinY) - viewBorderY;
}

/// ----------------------------------------------------------------------------
std::string CViewTransform::getParam(void) const
{
   std::stringstream ss;
   ss << "VIEW:" << viewX << "x" << viewY << ";";
   ss << "BORDER:" << viewBorderX << "," << viewBorderY << ";";
   ss << "REAL_X:" << realMinX << "," << realMaxX << ";";
   ss << "REAL_Y:" << realMinY << "," << realMaxY << ";";
   ss << "ASPECT:" << ar << ";";
   return ss.str();
}

/// ----------------------------------------------------------------------------
crl::CBBox &CViewTransform::getBBox(crl::CBBox &bbox) const
{
   bbox.set(realMinX, realMaxX, realMinY, realMaxY);
   return bbox;
}

/// ----------------------------------------------------------------------------
double CViewTransform::getCanvasLengthX(double realD) const
{
   return kX * realD;
}

/// ----------------------------------------------------------------------------
double CViewTransform::getCanvasLengthY(double realD) const
{
   return kY * realD;
}

/// protected ------------------------------------------------------------------
void CViewTransform::compute(void)
{
   if (ar > 0.0) {                               // width / height
      const double w = viewX - 2. * viewBorderX; // w is fix
      const double h = viewY - 2. * viewBorderY; // h is fix
      const double rx = fabs(realMaxX - realMinX);
      const double ry = fabs(realMaxY - realMinY);
      const double w_coords = rx / w; // x coords per pixel
      const double h_coords = ry / h; // y coords per pixel
      const double a = w_coords / h_coords;
      const double realAspect = rx / ry;
      bool perfect = true;
      if (a > ar) { // h_coords must be increased
         const double k = a / ar;
         const double dy = ((h_coords * k * h) - ry) / 2.;
         const int optimalH = (int)ceil(ar * w / realAspect) + 2 * viewBorderY;
         if (optimalH != viewY) {
            realMinY -= dy;
            realMaxY += dy;
            DEBUG_M(loclog, "For aspect ratio "
                                << ar << " optimal canvas size is " << viewX
                                << "x" << optimalH);
            perfect = false;
         }
      } else if (a < ar) {
         const double k = ar / a;
         const double dx = ((w_coords * k * w) - rx) / 2.0;
         const int optimalW = (int)ceil(realAspect * h / ar) + 2 * viewBorderX;
         if (optimalW != viewX) {
            realMinX -= dx;
            realMaxX += dx;
            DEBUG_M(loclog, "For aspect ratio "
                                << ar << " optimal canvas size is " << optimalW
                                << "x" << viewY);
            perfect = false;
         }
      } else { // current aspection ration is same as required
      }
      if (perfect) {
         DEBUG_M(loclog, "Canvas size perfectly match current real coords with "
                         "required aspect ratio");
      }
   }
   kX = (viewX - 2. * viewBorderX) /
        (realMaxX - realMinX); // -1 for start from 0 to width-1
   kY = (viewY - 2. * viewBorderY) /
        (realMaxY - realMinY); // -1 for start from 0 to height-1
                               // kX = (viewX) / (realMaxX - realMinX);
                               // kY = (viewY) / (realMaxY - realMinY);
   ratioX = fabs(kX);
   ratioY = fabs(kY);
   DEBUG_M(loclog, "realMinX:" << realMinX << " realMaxX:" << realMaxX);
   DEBUG_M(loclog, "realMinY:" << realMinY << " realMaxY:" << realMaxY);
   DEBUG_M(loclog, "viewX:" << viewX << " kX:" << kX);
   DEBUG_M(loclog, "viewY:" << viewY << " kY:" << kY);
}

/* end of view_transform.cc */
