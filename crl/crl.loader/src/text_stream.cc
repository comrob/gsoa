/*
 * File name: text_stream.cc
 * Date:      2008/08/03 15:05
 * Author:    Jan Faigl
 */

//#include <iostream> //DEBUG

//#include <vector>

#include <fcntl.h>
#include <unistd.h>

#include <crl/exceptions.h>
#include <crl/ws.h>

#include <crl/loader/text_stream.h>

using namespace crl::loader;

static const char s_characters[19] = {'9', '8', '7', '6', '5', '4', '3',
                                      '2', '1', '0', '1', '2', '3', '4',
                                      '5', '6', '7', '8', '9'};

static const int MAX_I = 12;
static const double ci[MAX_I] = {1e0, 1e1, 1e2, 1e3, 1e4,  1e5,
                                 1e6, 1e7, 1e8, 1e9, 1e10, 1e11};

static const double fi[MAX_I] = {1e-1, 1e-2, 1e-3, 1e-4,  1e-5,  1e-6,
                                 1e-7, 1e-8, 1e-9, 1e-10, 1e-11, 1e-12};

static bool wsInit = false;
static crl::CWS ws;
// buffer.buffer_size = 1024*32;

/// ----------------------------------------------------------------------------
/// Class CTextStream
/// ----------------------------------------------------------------------------
CTextStream::CTextStream(const std::string filename)
    : filename(filename), state(false), hasLast(false), loadbuf(BUFFER_SIZE)
{
   if (!wsInit) {
      ws['\t'] = true;
      ws[' '] = true;
      ws['\r'] = true;
      ws['\n'] = true;
   }
   open();
}

/// ----------------------------------------------------------------------------
CTextStream::CTextStream(const std::stringstream &ss)
    : filename(""), state(true), hasLast(false), loadbuf(ss.str().size())
{
   if (!wsInit) {
      ws['\t'] = true;
      ws[' '] = true;
      ws['\r'] = true;
      ws['\n'] = true;
   }
   const std::string &str = ss.str();
   for (int i = 0; i < loadbuf.size; i++) {
      loadbuf.buffer[i] = str[i];
   }
   loadbuf.loaded = loadbuf.size;
}

/// ----------------------------------------------------------------------------
CTextStream::~CTextStream() {}

/// ----------------------------------------------------------------------------
void CTextStream::open(void)
{
   loadbuf.fd = ::open(filename.c_str(), O_RDONLY);
   state = loadbuf.fd != -1;
}

/// ----------------------------------------------------------------------------
void CTextStream::close(void)
{
   if (loadbuf.fd != -1) {
      ::close(loadbuf.fd);
   }
}

/// ----------------------------------------------------------------------------
void CTextStream::clear(void)
{
   // std::cout << "CTextStream::clear lastByte:" << lastByte << std::endl;
   state = true;
}

/// ----------------------------------------------------------------------------
void CTextStream::extract(double &v)
{
   int di = 0;
   int i = 0;
   double m = 1.0;
   skipWhite();
   v = 0.0;
   while (state && !ws[lastByte]) {
      if (lastByte >= '0' && lastByte <= '9') {
         if (i < MAX_I) {
            if (di == 0) {
               v = (lastByte) - '0' + v * 10;
	    } else {
               v += (lastByte - '0') * fi[i++];
            }
	 } else {
            //    std::cout << "max value i:" << i << " v:" << v << std::endl;
            throw crl::io_error("max value reached");
         }
      } else {
         switch (lastByte) {
         case '.':
            if (di == 0) {
               di++;
               i = 0;
	    } else {
               state = false;
            }
            break;
         case '-':
            if (v == 0.0) {
               m = -1.0;
	    } else {
               state = false;
            }
            break;
         default:
            state = false;
            break;
         } // end switch
      }    // end else
      lastByte = getByte();
   }
   if (v != 0.0) {
      state = true;
      v = v * m;
   }
}

/// ----------------------------------------------------------------------------
void CTextStream::extract(int &v)
{
   int m = 1;
   skipWhite();
   v = 0;
   while (state && !ws[lastByte]) {
      if (lastByte >= '0' && lastByte <= '9') {
         v = (lastByte) - '0' + v * 10;
      } else {
         switch (lastByte) {
         case '-':
            if (v == 0) {
               m = -1;
	    } else {
               state = false;
            }
            break;
         default:
            state = false;
            break;
         } // end switch
      }    // end else
      if (state) {
         lastByte = getByte();
      }
   }
   if (v != 0) {
      state = true;
      v = v * m;
   }
   // std::cout << "extract int : '" << v << "' state:" << state << std::endl;
}

/// ----------------------------------------------------------------------------
void CTextStream::extract(std::string &v)
{
   const int s = 512;
   char buf[s + 1];
   int i = 0;
   //   std::cout << "extract(string) start lastByte:" << lastByte << std::endl;
   skipWhite();
   //   std::cout << "extract(string) skipWhite lastByte:" << lastByte <<
   //   std::endl;
   v = "";
   while (state && !ws[lastByte]) {
      buf[i++] = lastByte;
      lastByte = getByte();
      if (i == s) {
         v.reserve(v.size() + s);
         for (int j = 0; j < s; j++) {
            v.push_back(buf[j]);
         }
         i = 0;
      }
   } // end while loop
   if (i > 0) {
      buf[i] = '\0';
      v.reserve(v.size() + i);
      for (int j = 0; j < i; j++) {
         v.push_back(buf[j]);
      }
   }
   //  std::cout << "extract(string): '" << v<< "' state:" << state <<
   //  std::endl;
}

/// ----------------------------------------------------------------------------
bool CTextStream::getLine(std::string &line)
{
   const int s = 512;
   char buf[s + 1];
   int i = 0;
   line.clear();
   while (state && lastByte != '\n' && lastByte != '\r') {
      buf[i++] = lastByte;
      lastByte = getByte();
      if (i == s) {
         line.reserve(line.size() + s);
         for (int j = 0; j < s; j++) {
            line.push_back(buf[j]);
         }
         i = 0;
      }
   } // end while loop
   if (i > 0) {
      buf[i] = '\0';
      line.reserve(line.size() + i);
      for (int j = 0; j < i; j++) {
         line.push_back(buf[j]);
      }
   }
   if (state && (lastByte == '\n' || lastByte == '\r')) {
      lastByte = getByte();
      state = true; // TODO end of file
   }
   //   std::cout << "getLine: '" << line << "' state:" << state << std::endl;
   return *this;
}

/// ----------------------------------------------------------------------------
/// Protected methods
/// ----------------------------------------------------------------------------
void CTextStream::skipWhite(void)
{
   if (state) {
      if (!hasLast || ws[lastByte]) {
         do {
            hasLast = false;
            lastByte = getByte();
            hasLast = true;
         } while (state && ws[lastByte]);
      }
   }
}

/// ----------------------------------------------------------------------------
uint8_t CTextStream::getByte(void)
{
   uint8_t ret;
   // return byte from buffer if an end of file occur state is set to false
   if (state) {
      if (loadbuf.loaded == 0 || loadbuf.offset == loadbuf.loaded) { // try load
         if (loadbuf.fd != -1) {
            loadbuf.loaded = ::read(loadbuf.fd, loadbuf.buffer, loadbuf.size);
            loadbuf.offset = 0;
         }
      }
      if (loadbuf.loaded > 0 && loadbuf.offset < loadbuf.loaded) {
         ret = loadbuf.buffer[loadbuf.offset++];
      } else {
         state = false;
      }
   }
   return ret;
}

/// ----------------------------------------------------------------------------
/// Functions
/// ----------------------------------------------------------------------------
bool getline(crl::loader::CTextStream &in, std::string &line)
{
   return in.getLine(line);
}

/* end of text_stream.cc */
