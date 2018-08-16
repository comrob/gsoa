/*
 * File name: file_utils.h
 * Date:      2009/01/25 13:06
 * Author:    Jan Faigl
 */

#ifndef __FILE_UTILS_H__
#define __FILE_UTILS_H__

#include <string>

#include <crl/exceptions.h>

namespace crl {

/// ----------------------------------------------------------------------------
/// @brief readFile  read whole file into string
///
/// @param filename
/// @param content
///
/// @return
/// ----------------------------------------------------------------------------
std::string &readFile(const std::string &filename,
                      std::string &content) throw();

/// ----------------------------------------------------------------------------
/// @brief firstToken open file and read first string token
///
/// @param filename
///
/// @return
/// ----------------------------------------------------------------------------
std::string firstFileToken(const std::string &filename) throw();

/// ----------------------------------------------------------------------------
/// @brief firstFileLine open file and read first line
///
/// @param filename
///
/// @return
/// ----------------------------------------------------------------------------
std::string firstFileLine(const std::string &filename) throw();

/// ----------------------------------------------------------------------------
/// @brief getBasename
///
/// @param filename
///
/// @return filename as strings after last '/'
/// ----------------------------------------------------------------------------
std::string getBasename(const std::string &filename);

/// ----------------------------------------------------------------------------
/// @brief getDirname
///
/// @param filename
///
/// @return directory name to last '/'
/// ----------------------------------------------------------------------------
std::string getDirname(const std::string &filename);

bool isFile(const std::string &path);

bool isDirectory(const std::string &path);

/// ----------------------------------------------------------------------------
/// @brief checkFile
///
/// @param std::string
///
/// @return true if file or directory path exists
/// ----------------------------------------------------------------------------
bool checkPath(const std::string &path);

/// ----------------------------------------------------------------------------
/// @brief createDirectory
///
/// @param path
///
/// @return true if given directory exits or it has been created
/// ----------------------------------------------------------------------------
bool createDirectory(const std::string &path);

/// ----------------------------------------------------------------------------
/// @brief isBZ2
///
/// @param path
///
/// @return true if the file exist and its magic header is 'BZh'
/// ----------------------------------------------------------------------------
bool isBZ2(const std::string &path);

} // namespace crl

#endif

/* end of file_utils.h */
