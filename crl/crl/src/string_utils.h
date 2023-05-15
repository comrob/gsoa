/*
 * File name: string_utils.h
 * Date:      2009/07/25 13:20
 * Author:    Jan Faigl
 */

#ifndef __STRING_UTILS_H__
#define __STRING_UTILS_H__

#include <string>
#include <vector>

namespace crl {

typedef std::vector<std::string> StringVector;

inline StringVector &string_split(const std::string &str, char sep,
                           StringVector &tokens)
{
   tokens.clear();
   std::string::size_type cur = 0;
   std::string::size_type next;
   do {
      next = str.find(sep, cur);
      tokens.push_back(str.substr(cur, next - cur));
      cur = next + 1;
   } while (next != std::string::npos);
   return tokens;
}

} // end namespace crl

#endif

/* end of string_utils.h */
