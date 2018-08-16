/*
 * File name: guievent.cc
 * Date:      2005/11/18 16:36
 * Author:    Jan Faigl
 */

#include <crl/logging.h>

#include <crl/gui/guievent.h>

using namespace crl::gui;

//----------------------------------------------------------------------------
// Struct SGuiEvent
//----------------------------------------------------------------------------
std::string SGuiEvent::StrEvent[] = {
    "all", "empty", "mouse press", "mouse release", "mouse motion", "number"};

//----------------------------------------------------------------------------
SGuiEvent::SGuiEvent()
{
   // type = events::EMPTY;
   type = EMPTY;
}

//----------------------------------------------------------------------------
// SGuiEvent::SGuiEvent(SGuiEvent & event) {

//}

//----------------------------------------------------------------------------
std::string &SGuiEvent::str(void) { return StrEvent[type]; }

//----------------------------------------------------------------------------
// std::string & SGuiEvent::str(events::TType type) {
std::string &SGuiEvent::str(TType type) { return StrEvent[type]; }

//----------------------------------------------------------------------------
void SGuiEvent::setType(int iType)
{
   // if (iType >= events::ALL && iType < events::NUMBER) {
   if (iType >= ALL && iType < NUMBER) {
   } else {
      ERROR("Unknown event type, use EMPTY instead");
      //     iType = events::EMPTY;
      iType = EMPTY;
   }
   // type = (events::TType)iType;
   type = (TType)iType;
}

//----------------------------------------------------------------------------
void SGuiEvent::setMouseState(int iState) { mouseState = (TMouseState)iState; }

//----------------------------------------------------------------------------
// bool SGuiEvent::mouseButton(events::TMouseState btn) {
bool SGuiEvent::mouseButton(TMouseState btn) { return mouseState & btn; }

/* end of guievent.cc */
