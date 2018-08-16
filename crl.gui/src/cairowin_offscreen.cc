/*
 * File name: cairowin_offscreen.cc
 * Date:      2007/08/21 21:32
 * Author:    Jan Faigl
 */

#include <crl/gui/cairowin_offscreen.h>

using namespace crl::gui::cairo;

/// ----------------------------------------------------------------------------
/// Class CCairoWinOffScreen
/// ----------------------------------------------------------------------------
CCairoWinOffScreen::CCairoWinOffScreen(CCanvasContainer *iCanvas) : CCairoWin(iCanvas)
{
   width = 640;
   height = 480;
}

/// ----------------------------------------------------------------------------
CCairoWinOffScreen::~CCairoWinOffScreen() {}

/// ----------------------------------------------------------------------------
void CCairoWinOffScreen::initialize(void)
{
   // nothing to do
}

/// ----------------------------------------------------------------------------
void CCairoWinOffScreen::redraw(void)
{
   // nothing to do
}

/// ----------------------------------------------------------------------------
void CCairoWinOffScreen::invalidate(void) {}

/// ----------------------------------------------------------------------------
void CCairoWinOffScreen::start(void)
{
   // nothing to do
}

/// ----------------------------------------------------------------------------
void CCairoWinOffScreen::stop(void)
{
   // nothing to do
}

/// ----------------------------------------------------------------------------
void CCairoWinOffScreen::handleEvents(void) {}

/// ----------------------------------------------------------------------------
void CCairoWinOffScreen::setBackgroundColor(const SColor &color)
{
   canvas->setBackgroundColor(color);
}

/* end of cairowin_offscreen.cc */
