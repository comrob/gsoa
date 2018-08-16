/*
 * File name: cairogui.h
 * Date:      2007/02/17 18:28
 * Author:    Jan Faigl
 */

#ifndef __CAIROGUI_H__
#define __CAIROGUI_H__

#include <crl/config.h>
#include <crl/thread.h>

#include <crl/gui/gui.h>
#include <crl/gui/renderer.h>

#include <crl/gui/cairowin.h>
#include <crl/gui/canvascontainer.h>
#include <crl/gui/guilistener.h>

namespace crl { namespace gui { namespace cairo {

   class CCairoGui : public crl::concurrent::CThread, public crl::gui::CGui {
      typedef crl::concurrent::CThread ThreadBase;

    public:
      typedef crl::gui::render::CRenderer Renderer;

    public:
      //@deprecated
      CCairoGui(int iArgc, char **iArgv, bool offscreen = false);
      CCairoGui(CConfig &cfg, bool offscreen = false);
      ~CCairoGui();
      void stop(void);
      bool hasUserInput(void) { return userInput; }
      Renderer *getRenderer(void);
      Handler *getHandler(void);
      void redraw(void);

      void execute(void);

    private:
      const bool userInput;
      Mutex *mtx;
      CCanvasContainer *canvas;
      Renderer *renderer;
      Handler *handler;
      CCairoWin *win;

    private:
      void threadBody(void);
      crl::gui::CGuiListener *listener;
      bool offscreenWin;
   };

}}} // namespace crl::gui::cairo

#endif

/* end of cairogui.h */
