/*
 * File name: cairoxwinsimple.cc
 * Date:      2007/02/17 19:02
 * Author:    Jan Faigl
 */

#include <cairo-pdf.h>
#include <cairo-ps.h>

#include <crl/logging.h>

#include <crl/gui/markers.h>
#include <crl/gui/cairoxwinsimple.h>

using namespace crl::gui::cairo;

static const double DEFAULT_XTRANS = 0.0;
static const double DEFAULT_YTRANS = 0.0;
static const double DEFAULT_TOLERANCE = .1;
static const double DEFAULT_ZOOM = 1.0;

//----------------------------------------------------------------------------
/// CCairoXWinSimple
/// ----------------------------------------------------------------------------
CCairoXWinSimple::CCairoXWinSimple(CCanvasContainer *iCanvas)
    : CCairoWin(iCanvas)
{
   dpy = XOpenDisplay(0);
   if (dpy == NULL) {
      ERROR("Failed to open display: " + std::string(XDisplayName(0)));
      throw new std::bad_alloc;
   }
   pdf = false;
   width = 640;
   height = 480;
}

/// ----------------------------------------------------------------------------
CCairoXWinSimple::~CCairoXWinSimple()
{
   deinit();
   XCloseDisplay(dpy);
}

/// ----------------------------------------------------------------------------
void CCairoXWinSimple::initialize(void)
{
   init();
   refresh();
}

/// ----------------------------------------------------------------------------
void CCairoXWinSimple::redraw(void)
{
}

/// ----------------------------------------------------------------------------
void CCairoXWinSimple::invalidate(void)
{
}

/// ----------------------------------------------------------------------------
void CCairoXWinSimple::start(void) { handleEvents(); }

/// ----------------------------------------------------------------------------
/// Private methods
/// ----------------------------------------------------------------------------
void CCairoXWinSimple::init(void)
{
   int i;
   Window root;
   XGCValues gcv;

   root = DefaultRootWindow(dpy);
   scr = DefaultScreen(dpy);

   win = XCreateSimpleWindow(dpy, root, 0, 0, width, height, 0,
                             WhitePixel(dpy, scr), WhitePixel(dpy, scr));

   pix = XCreatePixmap(dpy, win, width, height, DefaultDepth(dpy, scr));
   gcv.foreground = WhitePixel(dpy, scr);
   gc = XCreateGC(dpy, pix, GCForeground, &gcv);
   XFillRectangle(dpy, pix, gc, 0, 0, width, height);

   active = 0;
   zoom = DEFAULT_ZOOM;
   xtrans = DEFAULT_XTRANS;
   ytrans = DEFAULT_YTRANS;

   click = 0;
   drag_pt.x = 0.0;
   drag_pt.y = 0.0;

   refresh();
   needs_refresh = 0;
   selectEvents();
   XMapWindow(dpy, win);
}

/// ----------------------------------------------------------------------------
void CCairoXWinSimple::deinit(void)
{
   XFreeGC(dpy, gc);
   XFreePixmap(dpy, pix);
   XDestroyWindow(dpy, win);
}

/// ----------------------------------------------------------------------------
void CCairoXWinSimple::refresh(void)
{
   cairo_surface_t *surface;
   cairo_t *cr;
   cairo_status_t status;

   XFillRectangle(dpy, pix, gc, 0, 0, width, height);

   surface = cairo_xlib_surface_create(dpy, pix, DefaultVisual(dpy, scr), width,
                                       height);

   if (pdf) {
      surface = cairo_pdf_surface_create("file.pdf", width, height);
   }

   cr = cairo_create(surface);
   cairo_set_tolerance(cr, 1.0);
   cairo_set_source_rgb(cr, 0, 0, 0);
   cairo_set_source_rgb(cr, 0, 0, 1);
   cairo_translate(cr, xtrans, ytrans);
   cairo_scale(cr, zoom, zoom);
   testDraw(cr);

   if (canvas) {
      canvas->draw(cr);
   }

   status = cairo_status(cr);
   if (status) {
      ERROR("Cairo is unhappy: " + std::string(cairo_status_to_string(status)));
   }

   if (pdf) {
      cairo_show_page(cr);
   }

   cairo_destroy(cr);
   cairo_surface_destroy(surface);

   XCopyArea(dpy, pix, win, gc, 0, 0, width, height, 0, 0);
}

/// ----------------------------------------------------------------------------
void CCairoXWinSimple::selectEvents(void)
{
   event_mask = ButtonPressMask | ButtonReleaseMask | PointerMotionMask |
                KeyPressMask | StructureNotifyMask | ExposureMask;
   XSelectInput(dpy, win, event_mask);
}

/// ----------------------------------------------------------------------------
void CCairoXWinSimple::handleEvents(void)
{
   bool done = false;
   XEvent xev;
   while (!done) {
      if (!XPending(dpy) && needs_refresh) {
         refresh();
         needs_refresh = 0;
      }
      XNextEvent(dpy, &xev);
      switch (xev.type) {
      case ButtonPress:
         INFO("Button press");
         click = 1;
         drag_pt.x = xev.xbutton.x;
         drag_pt.y = xev.xbutton.y;
         needs_refresh = 1;
         break;
      case MotionNotify:
         break;
      case ButtonRelease:
         click = 0;
         active = 0;
         INFO("ButtonRelease");
         break;
      case KeyPress:
         // INFO("KeyPress");
         done = handleKeyPress(&xev.xkey) == 1;
         break;
      case ConfigureNotify:
         handleConfigure(&xev.xconfigure);
         //    INFO("ConfigureNotify");
         break;
      case Expose:
         INFO("Expose");
         handleExpose(&xev.xexpose);
         //    win_handle_expose(win, &xev.xexpose);
         break;
      default:
         INFO("Unknown event");
         break;
      }
   }
}

/// ----------------------------------------------------------------------------
void CCairoXWinSimple::handleConfigure(XConfigureEvent *cev)
{
   int has_grown = 0;
   if (cev->width > width || cev->height > height) {
      has_grown = 1;
   }

   width = cev->width;
   height = cev->height;

   if (has_grown) {
      growPixmap();
   }
}

/// ----------------------------------------------------------------------------
void CCairoXWinSimple::handleExpose(XExposeEvent *eev)
{
   XCopyArea(dpy, pix, win, gc, eev->x, eev->y, eev->width, eev->height, eev->x,
             eev->y);
}

/// ----------------------------------------------------------------------------
void CCairoXWinSimple::growPixmap(void)
{
   Pixmap newPix;

   newPix = XCreatePixmap(dpy, win, width, height, DefaultDepth(dpy, scr));
   XFillRectangle(dpy, newPix, gc, 0, 0, width, height);
   XCopyArea(dpy, pix, newPix, gc, 0, 0, width, height, 0, 0);
   XFreePixmap(dpy, pix);
   pix = newPix;
   needs_refresh = 1;
}

/// ----------------------------------------------------------------------------
int CCairoXWinSimple::handleKeyPress(XKeyEvent *kev)
{
   int ret = 0;
   KeyCode keyQ = XKeysymToKeycode(dpy, XStringToKeysym("q"));
   KeyCode keyUp = XKeysymToKeycode(dpy, XStringToKeysym("Up"));
   KeyCode keyDown = XKeysymToKeycode(dpy, XStringToKeysym("Down"));
   KeyCode keyLeft = XKeysymToKeycode(dpy, XStringToKeysym("Left"));
   KeyCode keyRight = XKeysymToKeycode(dpy, XStringToKeysym("Right"));
   KeyCode keyPlus = XKeysymToKeycode(dpy, XStringToKeysym("plus"));
   KeyCode keyMinus = XKeysymToKeycode(dpy, XStringToKeysym("minus"));
   KeyCode keyP = XKeysymToKeycode(dpy, XStringToKeysym("p"));
   for (int i = 0; i < 1; i++) {
      if (kev->keycode == keyQ) {
         INFO("KEY Q HAS BEEN PRESSED");
         ret = 1;
         break;
      }
      if (kev->keycode == keyUp) {
         ytrans -= height / 16.0;
         needs_refresh = 1;
      }
      if (kev->keycode == keyDown) {
         ytrans += height / 16.0;
         needs_refresh = 1;
      }
      if (kev->keycode == keyLeft) {
         xtrans -= width / 16.0;
         needs_refresh = 1;
      }
      if (kev->keycode == keyRight) {
         xtrans += height / 16.0;
         needs_refresh = 1;
      }
      if (kev->keycode == keyPlus) {
         zoom *= 1.1;
         needs_refresh = 1;
      }
      if (kev->keycode == keyMinus) {
         zoom /= 1.1;
         needs_refresh = 1;
      }
      if (kev->keycode == keyP) {
         pdf = !(pdf == true);
         needs_refresh = 1;
         std::cout << " PDF:" << pdf << std::endl;
      }
   }
   return ret;
}

/// ----------------------------------------------------------------------------
void CCairoXWinSimple::testDraw(cairo_t *cr)
{
   double drag_user_x = drag_pt.x;
   double drag_user_y = drag_pt.y;
   std::cout << " drag user:[" << drag_user_x << "," << drag_user_y << "]"
             << std::endl;
   cairo_device_to_user(cr, &drag_user_x, &drag_user_y);
   std::cout << " device to user:[" << drag_user_x << "," << drag_user_y << "]"
             << std::endl;
   cairo_save(cr);

   cairo_set_source_rgb(cr, 0, 0, 1);
   cairo_set_line_width(cr, 10);
   cairo_move_to(cr, width / 2.0, height / 2.0);
   cairo_line_to(cr, 0.0, 0.0);
   if (cairo_in_stroke(cr, drag_user_x, drag_user_y)) {
      INFO("LINE1 SELECTED");
   }
   cairo_stroke(cr);
   cairo_set_source_rgb(cr, 0, 1, 0);
   cairo_move_to(cr, width / 2.0, height / 2.0);
   cairo_line_to(cr, 0.0, height / 2.0);
   if (cairo_in_stroke(cr, drag_user_x, drag_user_y)) {
      INFO("LINE2 SELECTED");
   }
   cairo_stroke(cr);
   cairo_restore(cr);
}

/* end of cairoxwinsimple.cc */
