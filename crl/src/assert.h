/*
 * File name: assert.h
 * Date:      2008/08/05 20:23
 * Author:    Jan Faigl
 */

#ifndef __ASSERT_H__
#define __ASSERT_H__

#include <crl/exceptions.h>

namespace crl {

#define ASSERT_ARGUMENT(cond, msg)                                             \
   if (!(cond))                                                                \
      throw crl::argument_error(msg);
#define ASSERT_IO(cond, msg)                                                   \
   if (!(cond))                                                                \
      throw crl::io_error(msg);

template <class T>
inline void assert_exception(bool cond, const std::string &msg)
    __attribute__((always_inline));

inline void assert_argument(bool cond, const std::string &msg)
    __attribute__((always_inline));

template <class T>
inline void assert_exception(bool cond, const std::string &msg)
{
   if (!cond) {
      throw T(msg);
   }
}

inline void assert_argument(bool cond, const std::string &msg)
{
   assert_exception<crl::argument_error>(cond, msg);
}

inline void assert_usage(bool cond, const std::string &msg)
{
   assert_exception<crl::bad_usage>(cond, msg);
}

inline void assert_io(bool cond, const std::string &msg)
{
   assert_exception<crl::io_error>(cond, msg);
}

} // end namespace crl

#endif

/* end of assert.h */
