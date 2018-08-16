/*
 * File name: mouseclick.cc
 * Date:      2007/08/29 06:31
 * Author:    Jan Faigl
 */

#include <crl/gui/mouseclick.h>

using namespace crl::gui;

/// ----------------------------------------------------------------------------
/// Class CWaitForClick
/// ----------------------------------------------------------------------------
CMouseClick::CMouseClick(CGui &iGui) : CGuiListener(), gui(iGui)
{
   enabled = false;
   gui.getHandler()->registerListener(this);
}

/// ----------------------------------------------------------------------------
CMouseClick::~CMouseClick() { gui.getHandler()->unregisterListener(this); }

/// ----------------------------------------------------------------------------
bool CMouseClick::handle(Event &event)
{
   bool ret = false;
   if (enabled) {
      if (event.getType() == Event::MOUSE_PRESS) {
         gui.getRenderer()->canvas2Real(event.mouseX, event.mouseY, &x, &y);
         clickWait.notify();
         ret = true;
         enabled = false;
      }
   }
   return ret;
}

/// ----------------------------------------------------------------------------
void CMouseClick::getMouseClick(double &oX, double &oY)
{
   {
      ScopedLock lk(enableMtx);
      enabled = true;
   }
   clickWait.wait(); // wait for handler finished
   oX = x;
   oY = y;
}

/* end of mouseclick.cc */
