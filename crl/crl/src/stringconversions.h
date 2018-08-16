/*
 * File name: stringconversions.h
 * Date:      2006/04/12 07:58
 * Author:    Jan Faigl
 */

#ifndef __STRINGCONVERSIONS_H__
#define __STRINGCONVERSIONS_H__

#include <iostream>
#include <sstream>
#include <string>

namespace crl {

template <class T> std::string string_format(T val)
{
   std::stringstream s;
   s << val;
   return s.str();
}

template <class T> std::string string_format_r(T &val)
{
   std::stringstream s;
   s << val;
   return s.str();
}

template <typename OutType, typename InValue>
OutType stream_cast(const InValue &t)
{
   std::stringstream ss;
   ss << t;
   OutType result;
   ss >> result;
   return result;
}

/// --------------------------------------------------------------------------
/// @brief string_cast convert str to out
///
/// @param str
/// @param out
///
/// @return true if conversion was successfull
/// --------------------------------------------------------------------------
template <typename OutType> bool string_cast(std::string str, OutType &out)
{
   std::stringstream ss(str, std::stringstream::in);
   ss >> out;
   return ss.good() or ss.eof();
}

template <typename T>
inline bool parse_coords(const std::string &str, const std::string sep, T &x,
                         T &y)
{
   std::string::size_type pos = str.find(sep);
   return (pos != std::string::npos) && string_cast<T>(str.substr(0, pos), x) &&
          string_cast<T>(str.substr(pos + 1), y);
}

template <class T>
inline std::string format_array(const T &array, const std::string &sep)
{
   std::stringstream ss;
   int first = true;
   for (typename T::const_iterator it = array.begin(); it != array.end();
        it++) {
      if (!first) {
         ss << sep;
      }
      ss << *it;
      first = false;
   }
   return ss.str();
}

template <class T>
inline T &parse_list(const std::string &str, const std::string &sep, T &list)
{
   typename T::value_type v;
   std::string::size_type prev = 0;
   std::string::size_type pos;
   while ((pos = str.find(sep, prev)) != std::string::npos) {
      if (string_cast(str.substr(prev, pos - prev), v)) {
         list.push_back(v);
      }
      prev = pos + 1;
   }
   if (string_cast(str.substr(prev, pos), v)) {
      list.push_back(v);
   }
   return list;
}

} // namespace crl

#endif

/* end of stringconversions.h */
