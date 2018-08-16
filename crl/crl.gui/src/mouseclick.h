/*
 * File name: mouseclick.h
 * Date:      2007/08/29 06:29
 * Author:    Jan Faigl
 */

#ifndef __MOUSECLICK_H__
#define __MOUSECLICK_H__

#include <crl/waitobject.h>

#include <crl/gui/gui.h>
#include <crl/gui/guievent.h>
#include <crl/gui/guilistener.h>

namespace crl { namespace gui {

   class CMouseClick : CGuiListener {
    public:
      typedef SGuiEvent Event;

      /// ----------------------------------------------------------------------------
      /// @brief CWaitForClick
      ///
      /// @param iGui
      /// ----------------------------------------------------------------------------
      CMouseClick(CGui &iGui);

      ~CMouseClick();

      bool handle(Event &event);

      /// ----------------------------------------------------------------------------
      /// @brief getMouseClick return click coords in current real coords
      ///
      /// @param oX
      /// @param oY
      /// ----------------------------------------------------------------------------
      void getMouseClick(double &oX, double &oY);

    private:
      typedef crl::concurrent::CWaitObject::Mutex Mutex;
      typedef crl::concurrent::CWaitObject::ScopedLock ScopedLock;

    private:
      CGui &gui;
      crl::concurrent::CWaitObject clickWait;
      bool enabled;
      Mutex enableMtx;
      double x;
      double y;
   };

}} // namespace crl::gui

#endif

/* end of mouseclick.h */
