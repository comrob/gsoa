/*
 * File name: eventqueue.h
 * Date:      2006/09/11 12:30
 * Author:    Jan Faigl
 */

#ifndef __EVENTQUEUE_H__
#define __EVENTQUEUE_H__

#include <vector>

#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/utility.hpp>

namespace crl { namespace concurrent {

   /// ----------------------------------------------------------------------------
   /// @brief  General even queue for one reader and several senders
   /// if messages are pointers, memory management is not part of eventqueue
   /// T  message type
   /// TT buffer for message
   /// ----------------------------------------------------------------------------
   // template <class T, class TT = std::vector<T*> >
   template <class T>
   // class CEventQueue : private boost::nocopyable {
   class CEventQueue {
    public:
      typedef boost::mutex::scoped_lock lock;
      typedef boost::mutex Mutex;
      typedef boost::condition Condition;

    public:
      typedef T Message;
      CEventQueue(int maxMsg) : _maxMsg(maxMsg), _idx(0), _number(0)
      {
         _buf = new Message[_maxMsg];
      }

      ~CEventQueue() { delete[] _buf; }

      /// ----------------------------------------------------------------------------
      /// @brief cleanup if pointers to active objects are stored as
      /// message delete all these object in buffer
      /// ----------------------------------------------------------------------------
      void cleanup(void)
      {
         while (_number > 0) {
            delete _buf[_idx];
            _number--;
            if (_number > 0) {
               _idx = (_idx + 1) % _maxMsg;
            }
         }
      }

      /// ----------------------------------------------------------------------------
      /// @brief send
      ///
      /// @param iMessage
      ///
      /// @return
      /// ----------------------------------------------------------------------------
      bool send(Message &iMessage)
      {
         lock lk(_mtx);
         bool ret = false;
         if (_number < _maxMsg) {
            //_buf.push_back(iMessage);
            _buf[(_idx + _number) % _maxMsg] = iMessage;
            _number++;
            ret = true;
            _cond.notify_one();
         }
         return ret;
      }

      bool sendBlock(Message &iMessage)
      {
         lock lk(_mtx);
         bool ret = false;
         if (_number == _maxMsg) {
            _condFree.wait(lk);
         }
         if (_number < _maxMsg) {
            int i = (_idx + _number) % _maxMsg;
            _buf[(_idx + _number) % _maxMsg] = iMessage;
            _number++;
            ret = true;
            _cond.notify_one();
         }
         return ret;
      }

      /// ----------------------------------------------------------------------------
      /// @brief receive
      ///
      /// @param oMessage
      ///
      /// @return
      /// ----------------------------------------------------------------------------
      bool receive(Message &oMessage)
      {
         lock lk(_mtx);
         bool ret = false;
         if (_number == 0) {
            _cond.wait(lk);
         }
         if (_number > 0) {
            oMessage = _buf[_idx];
            _number--;
            if (_number > 0) {
               _idx = (_idx + 1) % _maxMsg;
            }
            ret = true;
            _condFree.notify_one();
         }
         return ret;
      }

      void kill(void)
      {
         _cond.notify_all();
         _condFree.notify_all();
      }

    protected:
      int _maxMsg;
      int _idx;
      int _number;
      boost::mutex _mtx;
      boost::condition _cond;
      boost::condition _condFree;
      Message *_buf;
   };

}} // namespace crl::concurrent

#endif

/* end of eventqueue.h */
