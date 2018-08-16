/*
 * File name: guilistener.h
 * Date:      2005/11/18 12:19
 * Author:    Jan Faigl
 */

#ifndef __GUILISTENER_H__
#define __GUILISTENER_H__

#include <crl/gui/guievent.h>

namespace crl { namespace gui {

   ///
   /// @brief general class to handle events
   ///
   class CGuiListener {
    public:
      typedef SGuiEvent Event;

      virtual ~CGuiListener() {}
      ///
      /// @brief handle given event
      ///
      /// @param event
      ///
      /// @return  true if event is handled otherwise false
      ///
      virtual bool handle(Event &event) = 0;
   };

}} // namespace crl::gui

#endif

/* end of guilistener.h */
