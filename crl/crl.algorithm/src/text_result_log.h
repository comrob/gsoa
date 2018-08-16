/*
 * File name: text_result_log.h
 * Date:      2009/03/07 07:19
 * Author:    Jan Faigl
 */

#ifndef __TEXT_RESULT_LOG_H__
#define __TEXT_RESULT_LOG_H__

#include <fstream>
#include <string>

#include <crl/alg/result_log.h>

namespace crl {

class CTextLog {
 public:
   CTextLog(CResultLog &log, char nameSep = ':', char valueSep = ';');
   ~CTextLog();

   void print(std::ostream &out);
   void printRecord(int i, std::ostream &out);

   void printRecord(CResultLog::RecordIterator record, std::ostream &out,
                    bool endLine = true);

   // TODO loader and missing results finder
   void load(std::istream &in);

 private:
   char nSep;
   char vSep;
   CResultLog &log;
};

} // end namespace crl

#endif

/* end of text_result_log.h */
