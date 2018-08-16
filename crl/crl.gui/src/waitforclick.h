/*
 * File name: waitforclick.h
 * Date:      2007/08/24 10:59
 * Author:    Jan Faigl
 */

#ifndef __WAITFORCLICK_H__
#define __WAITFORCLICK_H__

#include <crl/waitobject.h>

#include <crl/gui/gui.h>
#include <crl/gui/guievent.h>
#include <crl/gui/guilistener.h>

namespace crl { namespace gui {

   /// ----------------------------------------------------------------------------
   /// @brief
   /// TODO user configuration of click event
   /// ----------------------------------------------------------------------------
   class CWaitForClick : public CGuiListener {
    public:
      typedef SGuiEvent Event;

      /// ----------------------------------------------------------------------------
      /// @brief CWaitForClick
      ///
      /// @param iGui
      /// @param perform if true wait immediately
      /// ----------------------------------------------------------------------------
      CWaitForClick(CGui &iGui, bool perform = true);

      ~CWaitForClick();

      bool handle(Event &event);

      void wait(void);

    private:
      CGui &gui;
      crl::concurrent::CWaitObject clickWait;
   };

}} // namespace crl::gui

#endif

/* end of waitforclick.h */
