/*
 * File name: loader_module.h
 * Date:      2011/02/15 17:23
 * Author:    Jan Faigl
 */

#ifndef __LOADER_MODULE_H__
#define __LOADER_MODULE_H__

#include <istream>
#include <ostream>
#include <string>
#include <vector>

namespace crl { namespace loader {

   class CModule {
    public:
      virtual ~CModule(){};

      virtual CModule *createInstance(void) const = 0;

      virtual const std::string &getName(void) const = 0;

      virtual const std::string &getFirstSection(void) const = 0;

      /// ----------------------------------------------------------------------------
      /// @brief getSections add defined section of the loader module to the
      /// given
      ///                    list of sections
      ///
      /// @param sections
      ///
      /// @return the filled sections
      /// ----------------------------------------------------------------------------
      virtual std::vector<std::string> &
      getSections(std::vector<std::string> &sections) = 0;

      virtual void clear(void) = 0;

      /// ----------------------------------------------------------------------------
      /// @brief parse
      ///
      /// @param in
      /// @param line if parsed the line contains next new line
      /// ----------------------------------------------------------------------------
      virtual void parse(std::istream &in, std::string &line) = 0;

      virtual void print(std::ostream &out) const = 0;

      virtual void fill(const CModule *module) = 0;
   };

}} // end namespace crl::loader

#endif

/* end of loader_module.h */
