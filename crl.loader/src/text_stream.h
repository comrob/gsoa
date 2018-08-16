/*
 * File name: text_stream.h
 * Date:      2008/08/03 15:01
 * Author:    Jan Faigl
 */

#ifndef __TEXT_STREAM_H__
#define __TEXT_STREAM_H__

#include <inttypes.h>
#include <sstream>
#include <string>

namespace crl { namespace loader {

   class CTextStream {
      enum { BUFFER_SIZE = 1024 * 32 };

      struct SBufferLoadFile {
         int fd;
         const int size;
         int offset;
         int loaded;
         uint8_t *buffer;

         SBufferLoadFile(const int size)
             : fd(-1), size(size), offset(0), loaded(0)
         {
            buffer = new uint8_t[size];
         }

         ~SBufferLoadFile() { delete[] buffer; }
      };

    public:
      CTextStream(const std::string filename);
      CTextStream(const std::stringstream &ss);
      ~CTextStream();

      void open(void);
      void close(void);

      void clear(void);

      operator bool() const { return state; }

      template <class T> CTextStream &operator>>(T &v)
      {
         extract(v);
         return *this;
      }

      void extract(double &v);
      void extract(int &v);
      void extract(std::string &v);
      bool getLine(std::string &line);

    protected:
      void skipWhite(void);
      uint8_t getByte(void);

    protected:
      const std::string filename;
      bool state;
      bool hasLast;
      SBufferLoadFile loadbuf;
      uint8_t lastByte;
   };

}} // namespace crl::loader

bool getline(crl::loader::CTextStream &in, std::string &line);

//     __istream_type&
//	  226       operator>>(void*& __p)
//  227       { return _M_extract(__p); }

// operator bool()

#endif

/* end of text_stream.h */
