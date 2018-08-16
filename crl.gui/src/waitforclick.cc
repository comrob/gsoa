/*
 * File name: waitforclick.cc
 * Date:      2007/08/24 11:26
 * Author:    Jan Faigl
 */

#include <crl/gui/waitforclick.h>

using namespace crl::gui;

/// ----------------------------------------------------------------------------
/// Class CWaitForClick
/// ----------------------------------------------------------------------------
CWaitForClick::CWaitForClick(CGui &iGui, bool perform)
    : CGuiListener(), gui(iGui)
{
   if (perform && gui.hasUserInput()) {
      wait();
   }
}

/// ----------------------------------------------------------------------------
CWaitForClick::~CWaitForClick() {}

/// ----------------------------------------------------------------------------
bool CWaitForClick::handle(Event &event)
{
   bool ret = false;
   if (event.getType() == Event::MOUSE_PRESS) {
      clickWait.notify();
      ret = true;
   }
   return ret;
}

/// ----------------------------------------------------------------------------
void CWaitForClick::wait(void)
{
   gui.getHandler()->registerListener(this);
   clickWait.wait(); // wait for handler finished
   gui.getHandler()->unregisterListener(this);
}

/* end of waitforclick.cc */
