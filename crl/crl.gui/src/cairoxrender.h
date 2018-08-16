/*
 * File name: cairoxrender.h
 * Date:      2007/02/17 18:56
 * Author:    Jan Faigl
 */

#ifndef __CAIROXRENDER_H__
#define __CAIROXRENDER_H__

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <cairo-xlib.h>
#include <cairo.h>

#include <crl/gui/cairowin.h>

namespace crl { namespace gui { namespace cairo {

   class CCairoXRender : public CCairoWin {
      int argc;
      char **argv;
      Display *dpy;
      int scr;
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

      // TODO split refresh to handle pdf
      bool pdf;

    public:
      CCairoXRender(int iArgc, char **iArgv, CCanvasContainer *iCanvas = 0);
      ~CCairoXRender();

      void initialize(void);
      void redraw(void);
      void invalidate(void);

      void start(void); // TODO rename
      void stop(void);
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

      int resize_pixmap(int w, int h);
   };

}}} // namespace crl::gui::cairo

#endif

/* end of cairoxrender.h */
