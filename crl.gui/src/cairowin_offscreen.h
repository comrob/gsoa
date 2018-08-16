/*
 * File name: cairowin_offscreen.h
 * Date:      2007/08/21 21:30
 * Author:    Jan Faigl
 */

#ifndef __CAIROWIN_OFFSCREEN_H__
#define __CAIROWIN_OFFSCREEN_H__

#include <crl/gui/cairowin.h>

namespace crl { namespace gui { namespace cairo {

   class CCairoWinOffScreen : public CCairoWin {
    public:
      CCairoWinOffScreen(CCanvasContainer *iCanvas = 0);
      ~CCairoWinOffScreen();
      void initialize(void);
      void redraw(void);
      void invalidate(void);
      void start(void);
      void stop(void);
      void handleEvents(void);
      void setBackgroundColor(const SColor &color);
   };

}}} // namespace crl::gui::cairo

#endif

/* end of cairowin_offscreen.h */
