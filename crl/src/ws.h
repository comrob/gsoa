/*
 * File name: ws.h
 * Date:      2006/04/27 22:32
 * Author:
 */

#ifndef __WS_H__
#define __WS_H__

namespace crl {
///
/// @brief white spaces
///
class CWS {
   static const int NUMBER;
   static bool initialized;
   static bool initVal[256];
   bool local[256];

 public:
   CWS();
   ~CWS();

   ///
   /// @brief clone
   ///
   /// @return new clone of CWS
   ///
   CWS *clone(void);

   ///
   /// @brief operator[]
   ///
   /// @param c
   ///
   /// @return true if c is out of 0-255 or it is marked as white space
   ///
   bool &operator[](unsigned char c);

   ///
   /// @brief clear local array all chars set false
   ///
   void clear(void);

   ///
   /// @brief init use default values of white spaces
   ///
   void init(void);

   ///
   /// @brief set array from to to (to is also set) to val
   /// if from and to is not in range nothing is done
   ///
   /// @param from 0-255; < to
   /// @param to 0-255; >= from
   /// @param val
   /// @param array
   ///
   static void set(int from, int to, bool val, bool array[256]);
};

/// --------------------------------------------------------------------------
/// General WaitSpace
/// --------------------------------------------------------------------------
extern CWS WhiteSpace;

} // namespace crl

#endif

/* end of ws.h */
