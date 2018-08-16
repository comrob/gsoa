/*
 * File name: guihandler.h
 * Date:      2005/11/18 16:21
 * Author:    Jan Faigl
 */

#ifndef __GUIHANDLER_H__
#define __GUIHANDLER_H__

#include <list>

#include <crl/thread.h>

#include <crl/gui/guievent.h>
#include <crl/gui/guilistener.h>

namespace crl { namespace gui {

   ///
   /// @brief general class for event guihandler
   ///
   class CGuiHandler {
    public:
      typedef CGuiListener Listener;
      typedef SGuiEvent Event;

    private:
      Listener **listenersByType;
      std::list<Listener *> listeners;
      typedef std::list<Listener *>::iterator It;

      /* std::list<SEvent*> events;
         typedef std::list<SEvent*>::iterator ItEvent;
         */

      const int MAX_EVENTS; // remove
    public:
      CGuiHandler(int iMaxEvents = 256);

      ///
      /// @brief ~CEventHandler delete all registered listeners
      ///
      ~CGuiHandler();

      ///
      /// @brief registerListener add listener to the queue
      /// a listener is called only if previous listeners do not handle event
      /// the listener should be registered only once
      /// if the listener is registered CEventHandler will deleted in destructor
      ///
      /// @param listener
      ///
      /// @return true if listener is added
      ///
      bool registerListener(Listener *listener);

      ///
      /// @brief registerListener register given listener under given eventType
      /// listener will be called only if event with given type will occur
      ///
      /// @param listener
      /// @param eventType
      ///
      /// @return if listener is registered, false if eventType is already
      /// handled by another listener
      ///
      bool registerListener(Listener *listener, Event::TType eventType);

      void unregisterListener(Listener *listener);

      ///
      /// @brief addEvent add event into guihandler queue
      /// this method should be called from same thread as handle method
      ///
      /// @param event
      ///
      // void addEvent(SEvent & event);

      ///
      /// @brief addEvent same as above but no new event is created
      /// event will be deleted by guihandler
      ///
      /// @param event
      ///
      // void addEvent(SEvent * event);

      ///
      /// @brief handle one events from queue,
      /// this method should be called from same thread as addEvent method
      ///
      /// @return false if no events are in the queue
      ///
      bool handle(Event &event);

    private:
      typedef crl::concurrent::ScopedLock ScopedLock;
      crl::concurrent::Mutex mutex;
   };

}} // namespace crl::gui

#endif

/* end of guihandler.h */
