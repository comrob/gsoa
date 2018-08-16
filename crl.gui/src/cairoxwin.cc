/*
 * File name: cairoxwin.cc
 * Date:      2007/02/17 19:02
 * Author:    Jan Faigl
 */


#include <crl/logging.h>
#include <crl/stringconversions.h>

#include <crl/gui/cairoxwin.h>
#include <crl/gui/markers.h>

using namespace crl::gui::cairo;

/*
   static const double DEFAULT_XTRANS = 0.0;
   static const double DEFAULT_YTRANS = 0.0;
   static const double DEFAULT_TOLERANCE = .1;
   static const double DEFAULT_ZOOM = 1.0;
   static const double DEFAULT_LINE_WIDTH = 10;
   static const cairo_line_cap_t DEFAULT_LINE_CAP = CAIRO_LINE_CAP_ROUND;
   */

//----------------------------------------------------------------------------
/// CCairoXWin
/// ----------------------------------------------------------------------------
CCairoXWin::CCairoXWin(CCanvasContainer *iCanvas) : CCairoWin(iCanvas)
{
   if (XInitThreads() == 0) {
      WARN("Can not init X to support threads");
   }
   dpy = XOpenDisplay(0);
   if (dpy == NULL) {
      ERROR("Failed to open display: " + std::string(XDisplayName(0)));
      throw new std::bad_alloc;
   }
   width = 640;
   height = 480;
   zoom = 1.;
   xtrans = 0.;
   ytrans = 0.;
   initialized = false;
   stopped = false;
   surface = 0;
   pix = 0;
}

/// ----------------------------------------------------------------------------
CCairoXWin::~CCairoXWin() { deinit(); }

/// ----------------------------------------------------------------------------
void CCairoXWin::setSize(int iWidth, int iHeigth)
{
   if (initialized) {
   } else {
      width = iWidth;
      height = iHeigth;
   }
}

/// ----------------------------------------------------------------------------
void CCairoXWin::initialize(void)
{
   if (!initialized) {
      init();
      refresh();
   }
   initialized = true;
}

/// ----------------------------------------------------------------------------
void CCairoXWin::redraw(void)
{
   needs_refresh = 1;
   XExposeEvent event;
   event.display = dpy;
   event.window = win;
   event.type = Expose;
   event.x = 0;
   event.y = 0;
   event.width = width;
   event.height = height;
   event.send_event = False;
   event.count = 0;
   XSendEvent(dpy, win, True, (long)0, (XEvent *)&event);
   XSync(dpy, False);
}

/// ----------------------------------------------------------------------------
void CCairoXWin::invalidate(void)
{
   canvas->drawBackground(cr, width, height, zoom);
   /*   cairo_set_source_rgba (cr, backgroundColor.red, backgroundColor.green,
      backgroundColor.blue, backgroundColor.alpha); cairo_rectangle (cr, 0, 0,
      width/zoom, height/zoom); cairo_fill(cr);*/
}

/// ----------------------------------------------------------------------------
void CCairoXWin::start(void)
{
   static bool started = false;
   if (!started) {
      started = true;
      handleEvents();
   } else {
      throw std::runtime_error("CCairoXWin already started");
   }
}

/// ----------------------------------------------------------------------------
void CCairoXWin::stop(void)
{
   if (initialized && !stopped) {
      stopped = true;
      XKeyEvent event;
      event.display = dpy;
      event.window = win;
      event.root = root;
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
   }
}

/// ----------------------------------------------------------------------------
/// Private methods
/// ----------------------------------------------------------------------------
void CCairoXWin::init(void)
{
   xsh.flags = PSize;
   xsh.width = width;
   xsh.height = height;
   xsh.x = 0;
   xsh.y = 0;

   win = XCreateWindow(dpy, RootWindow(dpy, DefaultScreen(dpy)), xsh.x, xsh.y,
                       xsh.width, xsh.height, 0, CopyFromParent, CopyFromParent,
                       CopyFromParent, 0, &xswa);

   XClassHint hints;
   hints.res_name = (char *)wmName.c_str();
   hints.res_class = (char *)wmClass.c_str();
   XSetClassHint(dpy, win, &hints);
   selectEvents();
   growPixmap(width, height);

   cr = cairo_create(surface);
   cairo_set_tolerance(cr, tolerance);
   active = 0;

   click = 0;
   drag_pt.x = 0.0;
   drag_pt.y = 0.0;
   needs_refresh = 0;
   XMapWindow(dpy, win);
}

/// ----------------------------------------------------------------------------
void CCairoXWin::deinit(void)
{
   if (cr) {
      cairo_destroy(cr);
      cr = 0;
   }
   if (surface) {
      cairo_surface_destroy(surface);
      surface = 0;
   }
   if (pix) {
      XFreePixmap(dpy, pix);
      pix = 0;
   }
   if (win) {
      XDestroyWindow(dpy, win);
      win = 0;
   }
   if (dpy) {
      XCloseDisplay(dpy);
      dpy = 0;
   }
}

/// ----------------------------------------------------------------------------
void CCairoXWin::refresh(void)
{
   cairo_status_t status;
   if (canvas) {
      invalidate();
      canvas->draw(cr);
   }

   status = cairo_status(cr);
   if (status) {
      ERROR("Cairo is unhappy: " + std::string(cairo_status_to_string(status)));
   }
   XSetWindowBackgroundPixmap(dpy, win, pix);
   XClearWindow(dpy, win);
   XSync(dpy, 0);
}

/// ----------------------------------------------------------------------------
void CCairoXWin::selectEvents(void)
{
   event_mask = ButtonPressMask | ButtonReleaseMask | PointerMotionMask |
                KeyPressMask | StructureNotifyMask | ExposureMask
       //     | ResizeRedirectMask
       //| SubstructureRedirectMask;
       ;
   XSelectInput(dpy, win, event_mask);
}

/// ----------------------------------------------------------------------------
void CCairoXWin::handleEvents(void)
{
   bool done = false;
   XEvent xev;
   while (!done) {
      if (!XPending(dpy) && needs_refresh) {
         // INFO("CCairoXWin::handleEvents XPending call refresh()");
         refresh();
         needs_refresh = 0;
      }
      if (!XPending(dpy)) {
         usleep(1000 * 20); // TBD
         continue;
      }
      // TODO fps can be specified
      XNextEvent(dpy, &xev);
      switch (xev.type) {
      case ButtonPress:
         if (handler && userHandler(xev)) {
            // event has been handled by user handler
            // needs_refresh = 1; //user click but
	 } else {
            //       INFO("Button press");
            click = 1;
            drag_pt.x = xev.xbutton.x;
            drag_pt.y = xev.xbutton.y;
            needs_refresh = 1;
            //    win_handle_button_press(win, &xev.xbutton);
         }
         break;
      case MotionNotify:
         if (handler && userHandler(xev)) {
         }
         //	    INFO("Motion notify");
         //    win_handle_motion(win, &xev.xmotion);
         break;
      case ButtonRelease:
         if (handler && userHandler(xev)) {
            // event has been handled by user handler
            // needs_refresh = 1; //user click but
         } else {
            click = 0;
            active = 0;
         }
         //    INFO("ButtonRelease");
         break;
      case KeyPress:
         // INFO("KeyPress");
         done = handleKeyPress(&xev.xkey) == 1;
         if (!done && needs_refresh == 1) {
            cairo_destroy(cr);
            cr = cairo_create(surface);
            cairo_set_tolerance(cr, tolerance);

            canvas->drawBackground(cr, width, height, zoom);

            cairo_translate(cr, xtrans, ytrans);
            cairo_scale(cr, zoom, zoom);
            refresh();
         }
         XSync(dpy, false);
         break;
      case ConfigureNotify:
         DEBUG("Configure");
         handleConfigure(&xev.xconfigure);
         break;
      case Expose:
         //	    INFO("Expose");
         if (xev.xexpose.count > 0) {
            break;
         }
         refresh();
         // INFO("Expose");
         //    handleExpose(&xev.xexpose);
         //    win_handle_expose(win, &xev.xexpose);
         break;
      case ClientMessage:
         DEBUG("Client message");
         break;
      case ResizeRequest:
         DEBUG("Resize request");
         break;
      case ConfigureRequest:
         DEBUG("Configure request");
         break;
      case UnmapNotify:
         DEBUG("Unmap notify");
         //    done = false;
         break;
      case MapNotify:
         DEBUG("Map notify");
         break;
      case MappingNotify:
         DEBUG("Mapping notify");
         break;
      case ReparentNotify:
         DEBUG("Reparent notify");
         break;
      default:
         DEBUG("Unknown event '" + crl::string_format<int>(xev.type) + "'");
         break;
      }
   } // end while loop
}

/// ----------------------------------------------------------------------------
void CCairoXWin::handleConfigure(XConfigureEvent *cev)
{
   growPixmap(cev->width, cev->height);
   cairo_destroy(cr);
   cr = cairo_create(surface);
   cairo_set_tolerance(cr, tolerance);

   canvas->drawBackground(cr, width, height, zoom);

   cairo_translate(cr, xtrans, ytrans);
   cairo_scale(cr, zoom, zoom);
}

/// ----------------------------------------------------------------------------
void CCairoXWin::growPixmap(int iWidth, int iHeigth)
{
   if (surface) {
      cairo_surface_destroy(surface);
   }
   if (pix) {
      XFreePixmap(dpy, pix);
   }
   width = iWidth;
   height = iHeigth;
   pix = XCreatePixmap(dpy, DefaultRootWindow(dpy), width, height,
                       DefaultDepth(dpy, DefaultScreen(dpy)));
   surface = cairo_xlib_surface_create(
       dpy, pix, DefaultVisual(dpy, DefaultScreen(dpy)), width, height);
   needs_refresh = 1;
}

/// ----------------------------------------------------------------------------
int CCairoXWin::handleKeyPress(XKeyEvent *kev)
{
   int ret = 0;
   static KeyCode keyQ = XKeysymToKeycode(dpy, XStringToKeysym("q"));
   static KeyCode keyUp = XKeysymToKeycode(dpy, XStringToKeysym("Up"));
   static KeyCode keyDown = XKeysymToKeycode(dpy, XStringToKeysym("Down"));
   static KeyCode keyLeft = XKeysymToKeycode(dpy, XStringToKeysym("Left"));
   static KeyCode keyRight = XKeysymToKeycode(dpy, XStringToKeysym("Right"));
   static KeyCode keyPlus = XKeysymToKeycode(dpy, XStringToKeysym("plus"));
   static KeyCode keyMinus = XKeysymToKeycode(dpy, XStringToKeysym("minus"));
   static KeyCode keyR = XKeysymToKeycode(dpy, XStringToKeysym("r"));
   for (int i = 0; i < 1; i++) {
      if (kev->keycode == keyQ) {
         INFO("KEY Q HAS BEEN PRESSED, CCairoXWin::shutdownRequest");
         quitRequest = true;
         if (stopped) {
            ret = 1;
         } else {
            needs_refresh = 1;
         }
         break;
      } else if (kev->keycode == keyUp) {
	 ytrans -= height / 16.0;
	 needs_refresh = 1;
      } else if (kev->keycode == keyDown) {
	 ytrans += height / 16.0;
	 needs_refresh = 1;
      } else if (kev->keycode == keyLeft) {
	 xtrans -= width / 16.0;
	 needs_refresh = 1;
      } else if (kev->keycode == keyRight) {
	 xtrans += width / 16.0;
	 needs_refresh = 1;
      } else if (kev->keycode == keyPlus) {
	 xtrans -= (zoom * (width - 20) * (0.5 / 1.5)) / 2;
	 ytrans -= (zoom * (height - 20) * (0.5 / 1.5)) / 2;
	 zoom *= 1.5;
	 // xtrans -= (zoom * (width - 20) * (0.1/1.1) ) / 2;
	 //	 ytrans -= (zoom * (height - 20) * (0.1/1.1) ) / 2;
	 // zoom *= 1.1;
	 needs_refresh = 1;
      } else if (kev->keycode == keyMinus) {
	 zoom /= 1.5;
	 xtrans += (zoom * (width - 20) * (0.5 / 1.5)) / 2;
	 ytrans += (zoom * (height - 20) * (0.5 / 1.5)) / 2;
	 // zoom /= 1.1;
	 // xtrans += (zoom * (width - 20)* (0.1/1.1) ) / 2;
	 //	 ytrans += (zoom * (height - 20) * (0.1/1.1) ) / 2;
	 needs_refresh = 1;
      } else if (kev->keycode == keyR) {
	 needs_refresh = 1;
	 zoom = 1.0;
	 ytrans = 0;
	 xtrans = 0;
      } else if (handler) {
	 _guiEvent.setType(Event::KEY_PRESS);
	 _guiEvent.keyCode = kev->keycode;
	 handler->handle(_guiEvent);
      }
   }
   return ret;
}

/// ----------------------------------------------------------------------------
bool CCairoXWin::userHandler(XEvent &xev)
{
   bool ret = false;
   bool callHandler = true;
   switch (xev.type) {
   case ButtonPress: {
      _guiEvent.setType(Event::MOUSE_PRESS);
      _guiEvent.mouseX = xev.xbutton.x;
      _guiEvent.mouseY = xev.xbutton.y;
      setMouseState(_guiEvent, xev.xbutton.button);
   } break;
   case ButtonRelease: {
      _guiEvent.setType(Event::MOUSE_RELEASE);
      _guiEvent.mouseX = xev.xbutton.x;
      _guiEvent.mouseY = xev.xbutton.y;
      setMouseState(_guiEvent, xev.xbutton.button);
   } break;
   case MotionNotify: {
      _guiEvent.setType(Event::MOUSE_MOTION);
      _guiEvent.mouseX = xev.xbutton.x;
      _guiEvent.mouseY = xev.xbutton.y;
      _guiEvent.mouseState = Event::BUTTON_NONE;
   } break;
   default:
      callHandler = false;
      break;
   }
   if (callHandler) {
      ret = handler->handle(_guiEvent);
   }
   return ret;
}

/// ----------------------------------------------------------------------------
void CCairoXWin::setMouseState(Event &event, int button)
{
   switch (button) {
   case Button1:
      event.mouseState = Event::BUTTON_1;
      break;
   case Button2:
      event.mouseState = Event::BUTTON_2;
      break;
   case Button3:
      event.mouseState = Event::BUTTON_3;
      break;
   default:
      event.mouseState = Event::BUTTON_NONE;
      break;
   }
}

/// ----------------------------------------------------------------------------
void CCairoXWin::testDraw(cairo_t *cr)
{
   double drag_user_x = drag_pt.x;
   double drag_user_y = drag_pt.y;
   std::cout << " drag user:[" << drag_user_x << "," << drag_user_y << "]"
             << std::endl;
   cairo_device_to_user(cr, &drag_user_x, &drag_user_y);
   std::cout << " device to user:[" << drag_user_x << "," << drag_user_y << "]"
             << std::endl;
   cairo_save(cr);

   //  cairo_set_source_rgb(cr, 0, 0, 1);
   //  cairo_set_line_width(cr, 10);
   //  markers::CCirclePoint::draw(cr, width/2.0, height/2.0, 10);
   //  markers::CCirclePoint::draw(cr, width/2.0, height/2.0, 10/zoom);
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
void CCairoXWin::setBackgroundColor(const SColor &color)
{
   canvas->setBackgroundColor(color);
   invalidate();
   refresh();
}

/* end of cairoxwin.cc */
