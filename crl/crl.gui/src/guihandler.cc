/*
 * File name: guihandler.cc
 * Date:      2005/11/18 16:30
 * Author:    Jan Faigl
 */

#include <crl/logging.h>

#include <crl/gui/guihandler.h>

using namespace crl::gui;

//----------------------------------------------------------------------------
// Class CGuiHandler
//----------------------------------------------------------------------------
CGuiHandler::CGuiHandler(int iMaxEvents) : MAX_EVENTS(iMaxEvents)
{
   listenersByType = new Listener *[Event::NUMBER];
   for (int i = 0; i < Event::NUMBER; ++i) {
      listenersByType[i] = 0;
   }
}

//----------------------------------------------------------------------------
CGuiHandler::~CGuiHandler()
{
   for (int i = 0; i < Event::NUMBER; ++i) {
      if (listenersByType[i] != 0) {
         delete listenersByType[i];
      }
   }
   It end = listeners.end();
   for (It it = listeners.begin(); it != end; ++it) {
      delete *it;
   }
}

//----------------------------------------------------------------------------
bool CGuiHandler::registerListener(Listener *listener)
{
   ScopedLock lk(mutex);
   bool ret = true;
   listeners.push_back(listener);
   return ret;
}

//----------------------------------------------------------------------------
bool CGuiHandler::registerListener(Listener *listener, Event::TType eventType)
{
   ScopedLock lk(mutex);
   bool ret = false;
   if (eventType > 0) {
      if (listenersByType[eventType] == 0) {
         listenersByType[eventType] = listener;
         ret = true;
      } else {
         WARN("Event type:" + Event::str(eventType) + " is already handled");
      }
   } else {
      WARN("Listener can not be registered with specified event type " +
           Event::str(eventType));
   }
   return ret;
}

//----------------------------------------------------------------------------
void CGuiHandler::unregisterListener(Listener *listener)
{
   ScopedLock lk(mutex);
   for (It it = listeners.begin(); it != listeners.end(); it++) {
      if ((*it) == listener) {
         listeners.erase(it);
         break;
      }
   }
}

//----------------------------------------------------------------------------
bool CGuiHandler::handle(Event &event)
{
   bool handled = false;
   if (listenersByType[event.type] != 0) {
      ScopedLock lk(mutex);
      handled = listenersByType[event.type]->handle(event);
   }
   if (!handled) {
      It it, end = listeners.end();
      ScopedLock lk(mutex);
      for (it = listeners.begin(); it != end; ++it) {
         if ((*it)->handle(event)) {
            handled = true;
            break;
         }
      }
   }
   return handled;
}

/* end of guihandler.cc */
