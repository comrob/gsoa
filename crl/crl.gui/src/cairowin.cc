/*
 * File name: cairowin.cc
 * Date:      2007/02/19 10:15
 * Author:    Jan Faigl
 */

#include <crl/gui/cairowin.h>

using namespace crl::gui::cairo;

/// ----------------------------------------------------------------------------
/// Class CCairoWin
/// ----------------------------------------------------------------------------
CCairoWin::CCairoWin(CCanvasContainer *iCanvas)
    : canvas(iCanvas), handler(0), tolerance(0.5), width(640), height(480),
      quitRequest(false), wmName("crl-cairowin"), wmClass("ComRob_CairoWin")
{
}

/// ----------------------------------------------------------------------------
CCairoWin::~CCairoWin() {}

/// ----------------------------------------------------------------------------
void CCairoWin::setHandler(Handler *iHandler) { handler = iHandler; }

/// ----------------------------------------------------------------------------
void CCairoWin::setSize(int iWidth, int iHeigth)
{
   width = iWidth;
   height = iHeigth;
}

/// ----------------------------------------------------------------------------
void CCairoWin::setWMClass(const std::string &wmN, const std::string &wmC)
{
   wmName = wmN;
   wmClass = wmC;
}

/// ----------------------------------------------------------------------------
int CCairoWin::getWidth(void)
{ // TODO propagate resize to renderer
   return width;
}

/// ----------------------------------------------------------------------------
int CCairoWin::getHeigth(void) { return height; }

/// ----------------------------------------------------------------------------
void CCairoWin::setTolerance(double iTolerance) { tolerance = iTolerance; }

/// ----------------------------------------------------------------------------
bool CCairoWin::shutdownRequest(void) { return quitRequest; }

/* end of cairowin.cc */
