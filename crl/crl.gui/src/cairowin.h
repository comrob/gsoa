/*
 * File name: cairowin.h
 * Date:      2007/02/19 10:14
 * Author:    Jan Faigl
 */

#ifndef __CAIROWIN_H__
#define __CAIROWIN_H__

#include <crl/gui/guievent.h>
#include <crl/gui/guihandler.h>
#include <crl/gui/canvascontainer.h>

namespace crl { namespace gui { namespace cairo {

   struct color_t {
      double red;
      double green;
      double blue;
   };

   struct pt_t {
      double x;
      double y;
   };

   class CCairoWin {
    protected:
      typedef crl::gui::CGuiHandler Handler;
      typedef crl::gui::SGuiEvent Event;
      CCanvasContainer *canvas;
      Handler *handler;
      double tolerance;
      int width, height;
      Event _guiEvent;
      bool quitRequest;
      std::string wmName;
      std::string wmClass;

    public:
      CCairoWin(CCanvasContainer *iCanvas = 0);
      virtual ~CCairoWin();
      void setHandler(Handler *iHandler);
      void setSize(int iWidth, int iHeigth);
      void setWMClass(const std::string &wmN, const std::string &wmC);
      virtual void initialize(void) = 0;
      int getWidth(void);
      int getHeigth(void);
      void setTolerance(double iTolerance);
      virtual void redraw(void) = 0;
      virtual void invalidate(void) = 0;
      virtual void start(void) = 0;
      virtual void stop(void) = 0;
      virtual void handleEvents(void) = 0;

      virtual void setBackgroundColor(const SColor &color) = 0;

      bool shutdownRequest(void);
   };

}}} // namespace crl::gui::cairo

#endif

/* end of cairowin.h */
