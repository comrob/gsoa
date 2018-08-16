/*
 * File name: cairogui.cc
 * Date:      2007/02/17 18:35
 * Author:    Jan Faigl
 */

#include <crl/logging.h>

#include <crl/gui/guievent.h>
#include <crl/gui/renderercairo.h>
#include <crl/gui/cairowin_offscreen.h>
#include <crl/gui/cairoxwin.h>

#include <crl/gui/cairogui.h>

using namespace crl::gui::cairo;

/*
   config.add<int>("gui-width", "width of default canvas view", 640);
   config.add<int>("gui-height", "height of default canvas view", 480);
   config.add<int>("gui-border-x", "x coords of default canvas view", 10);
   config.add<int>("gui-border-y", "y coords of default canvas view", 10);
   */

using crl::logger;

class CCanvasListener : public crl::gui::CGuiListener {
 public:
   CCanvasListener(crl::gui::render::CRenderer *renderer,
                   const std::string &saveFile)
       : crl::gui::CGuiListener(), saveFile(saveFile), renderer(renderer)
   {
   }
   virtual ~CCanvasListener() {}

   bool handle(crl::gui::SGuiEvent &event)
   {
      bool ret = false;
      if (event.type == crl::gui::SGuiEvent::KEY_PRESS) {
         if (event.keyCode == 68 && !saveFile.empty()) { // F2
            ret = renderer->save(saveFile);
            if (ret) {
               INFO("Canvas has been saved into '" << saveFile << "'");
	    } else {
	       WARN("Error occurs during saving canvas into '" << saveFile
		     << "'");
	    }
	 }
      }
      return ret;
   }

 private:
   std::string saveFile;
   crl::gui::render::CRenderer *renderer;
};

/// ----------------------------------------------------------------------------
// /Class CCairoGui
/// - @deprecated --------------------------------------------------------------
CCairoGui::CCairoGui(int iArgc, char **iArgv, bool offscreen)
    : crl::concurrent::CThread(), userInput(!offscreen)
{
   mtx = new Mutex();
   canvas = new CCanvasContainer;
   if (offscreen) {
      win = new CCairoWinOffScreen(canvas);
   } else {
      win = new CCairoXWin(canvas);
      //  win = new CCairoXRender(iArgc, iArgv, canvas);
   }
   win->setSize(640, 480);
   renderer = new crl::gui::render::CRendererCairo(canvas, win);
   handler = new Handler;
   win->setHandler(handler);
   // win->initialize();
}

/// ----------------------------------------------------------------------------
CCairoGui::CCairoGui(CConfig &cfg, bool offscreen)
    : crl::concurrent::CThread(), userInput(!offscreen)
{
   mtx = new Mutex();
   canvas = new CCanvasContainer;
   if (offscreen) {
      win = new CCairoWinOffScreen(canvas);
      offscreen = true;
   } else {
      win = new CCairoXWin(canvas);
      offscreenWin = false;
   }
   win->setWMClass(cfg.get<std::string>("gui-wm_class-name"),
	 cfg.get<std::string>("gui-wm_class"));
   win->setSize(cfg.get<int>("gui-width"), cfg.get<int>("gui-height"));
   canvas->setBackgroundColor(
	 SColor(cfg.get<std::string>("gui-background-color")),
	 cfg.get<double>("gui-background-alpha"));
   canvas->setBackgroundImage(cfg.get<std::string>("gui-background-image"));
   renderer = new crl::gui::render::CRendererCairo(canvas, win, cfg);
   handler = new Handler;
   win->setHandler(handler);
   listener = new CCanvasListener(renderer,
	 cfg.get<std::string>("gui-canvas-save-file"));
   handler->registerListener(listener);
   // win->initialize();
}

/// ----------------------------------------------------------------------------
CCairoGui::~CCairoGui()
{
   stop();
   join();
   //  std::cout << "destructor" << std::endl;
   if (win) {
      delete win;
   }
   if (renderer) {
      delete renderer;
   }
   handler->unregisterListener(listener);
   delete listener;
   delete handler;
   delete canvas;
   delete mtx;
}

/// ----------------------------------------------------------------------------
CCairoGui::Renderer *CCairoGui::getRenderer(void) { return renderer; }

/// ----------------------------------------------------------------------------
CCairoGui::Handler *CCairoGui::getHandler(void) { return handler; }

/// ----------------------------------------------------------------------------
void CCairoGui::redraw(void)
{
   if (renderer) {
      renderer->redraw();
   }
}

/// ----------------------------------------------------------------------------
void CCairoGui::execute(void)
{
   win->initialize();
   if (offscreenWin) {
      win->start();
   } else {
      ThreadBase::execute();
   }
}

/// ----------------------------------------------------------------------------
void CCairoGui::stop(void)
{
   {
      ScopedLock lk(*mtx);
      win->stop();
   }
}

/// ----------------------------------------------------------------------------
/// Private methods
/// ----------------------------------------------------------------------------
void CCairoGui::threadBody(void) { win->start(); }

/* end of cairogui.cc */
