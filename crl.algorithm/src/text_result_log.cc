/*
 * File name: text_result_log.cc
 * Date:      2009/03/07 07:20
 * Author:    Jan Faigl
 */

#include <crl/logging.h>

#include <crl/alg/text_result_log.h>

using namespace crl;

/// ----------------------------------------------------------------------------
/// Class CTextLog
/// ----------------------------------------------------------------------------
CTextLog::CTextLog(CResultLog &log, char nameSep, char valueSep)
    : log(log), nSep(nameSep), vSep(valueSep)
{
}

/// ----------------------------------------------------------------------------
CTextLog::~CTextLog() {}

/// ----------------------------------------------------------------------------
void CTextLog::print(std::ostream &out)
{
   for (CResultLog::RecordIterator record = log.records_begin();
        record != log.records_end(); record++) {
      printRecord(record, out);
   } // end records loop
}

/// ----------------------------------------------------------------------------
void CTextLog::printRecord(int i, std::ostream &out)
{
   printRecord(log.record_at(i), out);
}

/// ----------------------------------------------------------------------------
void CTextLog::printRecord(CResultLog::RecordIterator record, std::ostream &out,
                           bool endLine)
{
   if (record != log.records_end()) {
      result::StringArray::iterator name = log.column_names_begin();
      for (result::StringArray::iterator value = record->begin();
           value != record->end(); name++, value++) {
         out << *name << nSep << *value << vSep;
      } // end record loop
      if (endLine) {
         out << std::endl;
      }
   }
}

/// ----------------------------------------------------------------------------
void CTextLog::load(std::istream &in)
{
   int columnNumber = log.columnsSize();
   bool firstColumn = true;

   std::string line;
   while (std::getline(in, line)) {
      if (!line.empty()) {
         log << result::newrec;
         std::string::size_type itemStart = 0;
         std::string::size_type itemEnd = line.find(vSep);
         int c = 0;
         while (itemEnd != std::string::npos) {
            std::string value = line.substr(itemStart, itemEnd - itemStart);
            std::string::size_type name = value.find(nSep);
            if (name != std::string::npos) {
               std::string n = value.substr(0, name);
               std::string v = value.substr(name + 1, value.size() - name - 1);
               assert_io(c < columnNumber, "Too much columns in file");
               log << v;
               if (firstColumn) { // check column name
                  assert_io(c == log.getColumnIndex(n),
                            "Column name '" + n +
                                "' does not match with column number '" +
                                crl::string_format<int>(c) + "'");
               }
               c++;
	    } else {
               WARN("Name/value separator has not been found for value '"
                    << value << "'");
            }
            itemStart = itemEnd + 1;
            itemEnd = line.find(vSep, itemStart); // move to next item
         }                                        // end while item end
         log << result::endrec;
         firstColumn = false;
      }
   } // end while in
   DEBUG("log records:" << log.recordsSize());
}

/* end of text_result_log.cc */
