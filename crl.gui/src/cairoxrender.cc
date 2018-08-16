/*
 * File name: cairoxrender.cc
 * Date:      2007/02/17 19:02
 * Author:    Jan Faigl
 */

#include <cmath>

#include <signal.h>

#include <cairo-pdf.h>
#include <cairo-ps.h>

#include <crl/logging.h>

#include <crl/gui/cairoxrender.h>

#include <crl/gui/markers.h>

using namespace crl::gui::cairo;

static const double DEFAULT_XTRANS = 0.0;
static const double DEFAULT_YTRANS = 0.0;
static const double DEFAULT_TOLERANCE = .1;
// static const double DEFAULT_LINE_WIDTH = 10;
static const double DEFAULT_ZOOM = 1.0;
// static const cairo_line_cap_t DEFAULT_LINE_CAP = CAIRO_LINE_CAP_ROUND;
int frame_cnt = 0;

void alarmhandler(int sig)
{
   if (sig == SIGALRM) {
      {
         printf("%d frames in 5.0 seconds = %.3f FPS\n", frame_cnt,
                frame_cnt / 5.0);
      }
      frame_cnt = 0;
   }
   signal(SIGALRM, alarmhandler);
   alarm(5);
}

//----------------------------------------------------------------------------
/// CCairoXRender
/// ----------------------------------------------------------------------------
CCairoXRender::CCairoXRender(int iArgc, char **iArgv, CCanvasContainer *iCanvas)
    : CCairoWin(iCanvas)
{
   argc = iArgc;
   argv = iArgv;
   dpy = XOpenDisplay(0);
   if (dpy == NULL) {
      ERROR("Failed to open display: " + std::string(XDisplayName(0)));
      throw new std::bad_alloc;
   }
   pdf = false;
   width = 640;
   height = 480;
   surface = 0;
   pix = None;
}

/// ----------------------------------------------------------------------------
CCairoXRender::~CCairoXRender()
{
   //  deinit();
   // XCloseDisplay(dpy);
}

/// ----------------------------------------------------------------------------
void CCairoXRender::initialize(void)
{
   init();
   //  refresh();
}

/// ----------------------------------------------------------------------------
void CCairoXRender::redraw(void)
{
}

/// ----------------------------------------------------------------------------
void CCairoXRender::invalidate(void)
{
   cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 1.0); // TODO use background color
   cairo_rectangle(cr, 0, 0, width / zoom, height / zoom);
   cairo_fill(cr);
}

/// ----------------------------------------------------------------------------
void CCairoXRender::start(void) { handleEvents(); }

/// ----------------------------------------------------------------------------
void CCairoXRender::stop(void)
{
   //  if (initialized && !stopped) {
   //    stopped = true;
   XKeyEvent event;
   event.display = dpy;
   event.window = win;
   //  event.root = root;
   event.subwindow = None;
   event.time = CurrentTime;
   event.x = 1;
   event.y = 1;
   event.x_root = 1;
   event.y_root = 1;
   event.same_screen = True;
   event.type = KeyPress;
   event.keycode = XKeysymToKeycode(dpy, XStringToKeysym("q")); // TODO quit
   event.state = 0;
   XSendEvent(event.display, event.window, True, KeyPressMask,
              (XEvent *)&event);
   XSync(dpy, 0);
   //   }
}

/// ----------------------------------------------------------------------------
/// Private methods
/// ----------------------------------------------------------------------------
void CCairoXRender::init(void)
{
   XGCValues gcv;

   xsh.flags = PSize;
   xsh.width = width;
   xsh.height = height;
   xsh.x = 0;
   xsh.y = 0;

   win = XCreateWindow(dpy, RootWindow(dpy, DefaultScreen(dpy)), xsh.x, xsh.y,
                       xsh.width, xsh.height, 0, CopyFromParent, CopyFromParent,
                       CopyFromParent, 0, &xswa);
   selectEvents();

   resize_pixmap(width, height);

   /*
    * CREATE Graphics Contex
    */
   gcv.foreground = WhitePixel(dpy, scr);
   gc = XCreateGC(dpy, pix, GCForeground, &gcv);

   cr = cairo_create(surface);
   cairo_set_tolerance(cr, 0.5);

   active = 0;
   zoom = DEFAULT_ZOOM;
   xtrans = DEFAULT_XTRANS;
   ytrans = DEFAULT_YTRANS;

   click = 0;
   drag_pt.x = 0.0;
   drag_pt.y = 0.0;

   needs_refresh = 0;
   XMapWindow(dpy, win);
   signal(SIGALRM, alarmhandler);
   alarm(5);
}

/// ----------------------------------------------------------------------------
void CCairoXRender::deinit(void)
{
   XFreeGC(dpy, gc);
   XFreePixmap(dpy, pix);
   XDestroyWindow(dpy, win);
}

/// ----------------------------------------------------------------------------
void CCairoXRender::refresh(void)
{
   int i;
   int j;
   static int x = 0;
   cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 1.0);
   cairo_rectangle(cr, 0, 0, width / zoom, height / zoom);
   cairo_fill(cr);
   cairo_stroke(cr);
   cairo_set_source_rgba(cr, 0.75, 0.75, 0.75, 1.0);
   for (j = 0; j < 32; j++) {
      for (i = 0; i < 32; i++) {
         cairo_arc(cr, i * 25.0 + x + 15, j * 25.0 + 15, 10, 0., 2 * M_PI);
         cairo_fill(cr);
         cairo_stroke(cr);
      }
      x = (x + 1) % 10;
   }
   frame_cnt++;
}

/// ----------------------------------------------------------------------------
void CCairoXRender::selectEvents(void)
{
   event_mask = ButtonPressMask | ButtonReleaseMask | PointerMotionMask |
                KeyPressMask | StructureNotifyMask | ExposureMask;
   XSelectInput(dpy, win, event_mask);
}

/// ----------------------------------------------------------------------------
void CCairoXRender::handleEvents(void)
{
   bool done = false;
   XEvent xev;
   while (!done) {
      if (XPending(dpy)) {
         XNextEvent(dpy, &xev);
         switch (xev.type) {
            case ConfigureNotify:
               width = xev.xconfigure.width;
               height = xev.xconfigure.height;
               resize_pixmap(width, height);
               cairo_destroy(cr);
               cr = cairo_create(surface);
               cairo_set_tolerance(cr, 1);
            case KeyPress:
               done = handleKeyPress(&xev.xkey) == 1;
               break;
         }
      } else {
         refresh();
         XSetWindowBackgroundPixmap(dpy, win, pix);
         XClearWindow(dpy, win);
         XSync(dpy, 0);
      }
   }
}

/// ----------------------------------------------------------------------------
void CCairoXRender::handleConfigure(XConfigureEvent *cev)
{
}

/// ----------------------------------------------------------------------------
void CCairoXRender::handleExpose(XExposeEvent *eev)
{
}

/// ----------------------------------------------------------------------------
void CCairoXRender::growPixmap(void)
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
int CCairoXRender::handleKeyPress(XKeyEvent *kev)
{
   int ret = 0;
   static KeyCode keyQ = XKeysymToKeycode(dpy, XStringToKeysym("q"));
   static KeyCode keyUp = XKeysymToKeycode(dpy, XStringToKeysym("Up"));
   static KeyCode keyDown = XKeysymToKeycode(dpy, XStringToKeysym("Down"));
   static KeyCode keyLeft = XKeysymToKeycode(dpy, XStringToKeysym("Left"));
   static KeyCode keyRight = XKeysymToKeycode(dpy, XStringToKeysym("Right"));
   static KeyCode keyPlus = XKeysymToKeycode(dpy, XStringToKeysym("plus"));
   static KeyCode keyMinus = XKeysymToKeycode(dpy, XStringToKeysym("minus"));
   static KeyCode keyP = XKeysymToKeycode(dpy, XStringToKeysym("p"));
   for (int i = 0; i < 1; i++) {
      if (kev->keycode == keyQ) {
         INFO("KEY Q HAS BEEN PRESSED, CCairoXRender::shutdownRequest ");
         quitRequest = true;
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
         //	 xtrans += (height * 0.1) / 2.0;
         //	 ytrans += (width * 0.1) / 2.0;
         needs_refresh = 1;
      }
      if (kev->keycode == keyMinus) {
         zoom /= 1.1;
         //	 xtrans -= (height * 0.1) / 2.0;
         //	 ytrans -= (width * 0.1) / 2.0;
         needs_refresh = 1;
      }
      if (kev->keycode == keyP) {
         pdf = !(pdf == true);
         needs_refresh = 1;
         std::cout << " PDF:" << pdf << std::endl;
      }
   }
   if (needs_refresh) {
      resize_pixmap(width, height);
      cairo_destroy(cr);
      cr = cairo_create(surface);
      cairo_translate(cr, xtrans, ytrans);
      cairo_scale(cr, zoom, zoom);
   }
   return ret;
}

/// ----------------------------------------------------------------------------
void CCairoXRender::testDraw(cairo_t *cr)
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

/// ----------------------------------------------------------------------------
int CCairoXRender::resize_pixmap(int w, int h)
{
   if (surface)
      cairo_surface_destroy(surface);

   if (pix)
      XFreePixmap(dpy, pix);

   pix = XCreatePixmap(dpy, DefaultRootWindow(dpy), w, h,
                       DefaultDepth(dpy, DefaultScreen(dpy)));

   surface = cairo_xlib_surface_create(
       dpy, pix, DefaultVisual(dpy, DefaultScreen(dpy)), w, h);
   return !surface;
}

/* end of cairoxrender.cc */
