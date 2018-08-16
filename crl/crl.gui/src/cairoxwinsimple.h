/*
 * File name: cairoxwinsimple.h
 * Date:      2007/02/17 18:56
 * Author:    Jan Faigl
 */

#ifndef __CAIROXWINSIMPLE_H__
#define __CAIROXWINSIMPLE_H__

#include <cairo-xlib.h>
#include <cairo.h>

#include <crl/gui/cairowin.h>

namespace crl { namespace gui { namespace cairo {

   class CCairoXWinSimple : public CCairoWin {
      Display *dpy;
      int scr;
      Window win;
      GC gc;
      Pixmap pix;
      long event_mask;

      int needs_refresh;

      double zoom;
      double xtrans;
      double ytrans;

      int click;
      pt_t drag_pt;
      int active;

      // TODO split refresh to handle pdf
      bool pdf;

    public:
      CCairoXWinSimple(CCanvasContainer *iCanvas = 0);
      ~CCairoXWinSimple();

      void initialize(void);
      void redraw(void);
      void invalidate(void);

      void start(void); // TODO rename
      void handleEvents(void);

    private:
      void init(void);
      void deinit(void);
      void refresh(void);
      void selectEvents(void);
      void handleConfigure(XConfigureEvent *cev);
      void handleExpose(XExposeEvent *eev);
      void growPixmap(void);

      int handleKeyPress(XKeyEvent *kev);

      void testDraw(cairo_t *cr);
   };

}}} // namespace crl::gui::cairo

#endif

/* end of cairoxwinsimple.h */
