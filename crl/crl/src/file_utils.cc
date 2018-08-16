/*
 * File name: file_utils.cc
 * Date:      2009/01/25 13:06
 * Author:    Jan Faigl
 */

#include <fstream>
#include <sstream>

#include <cstring>
#include <sys/stat.h>

#include <crl/file_utils.h>
#include <crl/assert.h>

namespace crl {

/// ----------------------------------------------------------------------------
std::string &readFile(const std::string &filename,
                      std::string &content) throw()
{
   std::ifstream ifs(filename.c_str());
   std::stringstream oss;
   assert_io(not ifs.fail(), "Can not open file '" + filename + "'");
   oss << ifs.rdbuf();
   assert_io(ifs || ifs.eof(), "Error reading file '" + filename + "'");
   content = oss.str();
   return content;
}

/// ----------------------------------------------------------------------------
std::string firstFileToken(const std::string &filename) throw()
{
   std::string ret;
   std::ifstream ifs(filename.c_str());
   assert_io(not ifs.fail(), "Can not open file '" + filename + "'");
   ifs >> ret;
   assert_io(not ifs.fail(), "Error reading file '" + filename + "'");
   ifs.close();
   return ret;
}

/// ----------------------------------------------------------------------------
std::string firstFileLine(const std::string &filename) throw()
{
   std::string ret;
   std::ifstream ifs(filename.c_str());
   crl::assert_io(not ifs.fail(), "Can not open file '" + filename + "'");
   std::getline(ifs, ret);
   assert_io(not ifs.fail(), "Error reading file '" + filename + "'");
   ifs.close();
   return ret;
}

/// ----------------------------------------------------------------------------
std::string getBasename(const std::string &filename)
{
   std::string::size_type pos = filename.rfind('/');
   return pos == std::string::npos ? filename
                                   : filename.substr(pos + 1, filename.size());
}

/// ----------------------------------------------------------------------------
std::string getDirname(const std::string &filename)
{
   std::string::size_type pos = filename.rfind('/');
   return pos == std::string::npos ? "" : filename.substr(0, pos);
}

/// ----------------------------------------------------------------------------
bool isFile(const std::string &path)
{
   bool ret = false;
   struct stat buf;
   if (lstat(path.c_str(), &buf) == 0) {
      ret = S_ISREG(buf.st_mode);
   }
   return ret;
}

/// ----------------------------------------------------------------------------
bool isDirectory(const std::string &path)
{
   bool ret = false;
   struct stat buf;
   if (stat(path.c_str(), &buf) == 0) {
      ret = S_ISDIR(buf.st_mode) != 0;
   }
   return ret;
}

/// ----------------------------------------------------------------------------
bool checkPath(const std::string &path)
{
   struct stat buf;
   return lstat(path.c_str(), &buf) == 0;
}

/// ----------------------------------------------------------------------------
bool createDirectory(const std::string &path)
{
   bool ret = isDirectory(path);
   if (!ret && !checkPath(path) &&
       path.size() > 0) { // given path does not exits
      const char *cpath = path.c_str();
      int len = strlen(cpath);
      char *path = new char[len + 1];
      int i;
      for (i = 0; i < len; i++) {
         path[i] = cpath[i];
         if (path[i] == '/') {
            path[i + 1] = '\0';
            if (!isDirectory(path) &&
                (mkdir(path, S_IRWXU | S_IRWXG | S_IRWXO) != 0)) {
               break;
            }
         }
      } // end for loop
      if (i == len) {
         path[i] = '\0';
         ret = isDirectory(path) ||
               (mkdir(path, S_IRWXU | S_IRWXG | S_IRWXO) == 0);
      }
      delete[] path;
   }
   return ret;
}

/// ----------------------------------------------------------------------------
bool isBZ2(const std::string &path)
{
   std::ifstream in(path.c_str());
   char h[3];
   return (in >> h[0] >> h[1] >> h[2] and h[0] == 'B' and h[1] == 'Z' and
           h[2] == 'h');
}

} // namespace crl

/* end of file_utils.cc */
