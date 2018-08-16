/*
 * File name: cairoxwin.h
 * Date:      2007/02/17 18:56
 * Author:    Jan Faigl
 */

#ifndef __CAIROXWIN_H__
#define __CAIROXWIN_H__

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <cairo-xlib.h>
#include <cairo.h>

#include <crl/gui/cairowin.h>

namespace crl { namespace gui { namespace cairo {

   class CCairoXWin : public CCairoWin {
      bool initialized;
      bool stopped;
      Display *dpy;
      int scr;
      Window root;
      Window win;
      GC gc;
      Pixmap pix;
      cairo_t *cr;
      cairo_surface_t *surface;
      XSizeHints xsh;
      XSetWindowAttributes xswa;
      XVisualInfo *vinfo;

      long event_mask;

      int needs_refresh;

      double zoom;
      double xtrans;
      double ytrans;

      int click;
      pt_t drag_pt;
      int active;

    public:
      CCairoXWin(CCanvasContainer *iCanvas = 0);
      ~CCairoXWin();

      void setSize(int iWidth, int iHeigth);
      void initialize(void);
      void redraw(void);
      void invalidate(void);

      void start(void);
      void stop(void);
      void handleEvents(void);

    private:
      void init(void);
      void deinit(void);
      void refresh(void);
      void selectEvents(void);
      void handleConfigure(XConfigureEvent *cev);
      void growPixmap(int iWidth, int iHeigth);

      int handleKeyPress(XKeyEvent *kev);
      bool userHandler(XEvent &xev);
      void setMouseState(Event &event, int button);
      void testDraw(cairo_t *cr);
      void setBackgroundColor(const SColor &color);
   };

}}} // namespace crl::gui::cairo

#endif

/* end of cairoxwin.h */
