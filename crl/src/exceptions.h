/*
 * File name: exceptions.h
 * Date:      2008/07/24 10:44
 * Author:    Jan Faigl
 */

#ifndef __EXCEPTIONS_H__
#define __EXCEPTIONS_H__

#include <stdexcept>
#include <string>

namespace crl {

class exception : public std::runtime_error {
 public:
   explicit exception(const std::string &what_arg)
       : std::runtime_error(what_arg)
   {
   }
};

template <int T> class exception_base : public exception {
 public:
   explicit exception_base(const std::string &what_arg) : exception(what_arg) {}
};

typedef exception_base<0> argument_error; //
typedef exception_base<1> io_error;       // error related to i/o operations
typedef exception_base<2> packet_error;
typedef exception_base<3> datagram_error;
typedef exception_base<4> serialization_error; // error related to serialization
typedef exception_base<5> delivery_error;
typedef exception_base<6> bad_code;  // to inform about wrong usage
typedef exception_base<7> bad_usage; // to inform about wrong usage

} // end namespace crl

#endif

/* end of exceptions.h */
