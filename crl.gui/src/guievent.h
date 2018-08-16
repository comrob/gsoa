/*
 * File name: guievent.h
 * Date:      2005/11/18 12:20
 * Author:    Jan Faigl
 */

#ifndef __GUIEVENT_H__
#define __GUIEVENT_H__

#include <string>

namespace crl { namespace gui {

   ///
   /// @brief  general gui event structure
   ///
   struct SGuiEvent {
      enum TType {
         ALL = 0, // all events
         EMPTY,
         MOUSE_PRESS,
         MOUSE_RELEASE,
         MOUSE_MOTION,
         KEY_PRESS,
         NUMBER
      };

      enum TMouseState {
         BUTTON_NONE = 0,
         BUTTON_1 = 2,
         BUTTON_2 = 4,
         BUTTON_3 = 8
      };

      static std::string StrEvent[];
      // events::TType type;
      TType type;
      TMouseState mouseState;
      std::string message;
      int mouseX;
      int mouseY;
      unsigned char keyCode;
      SGuiEvent();
      //     SGuiEvent(
      // SGuiEvent(SGuiEvent & event);
      std::string &str(void);
      // static std::string & str(events::TType type);
      static std::string &str(TType type);

      void setType(int iType);
      void setMouseState(int iState);

      TType getType(void) { return type; }

      TMouseState getMouseState(void) { return mouseState; }

      ///
      /// @brief mouseButton return true if  given mouse button flag is 1 in
      /// mouseState
      ///
      /// @param btn
      ///
      /// @return
      ///
      // bool mouseButton(events::TMouseState btn);
      bool mouseButton(TMouseState btn);
   };

}} // namespace crl::gui

#endif

/* end of guievent.h */
