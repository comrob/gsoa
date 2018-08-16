/*
 * File name: result_log.cc
 * Date:      2009/03/04 08:59
 * Author:    Jan Faigl
 */

#include <fstream>

#include <crl/stringconversions.h>

#include <crl/alg/result_log.h>

using namespace crl;
using namespace crl::result;

/// ----------------------------------------------------------------------------
/// Class CRecordDescription
/// ----------------------------------------------------------------------------
CRecordDescription::CRecordDescription(const std::string &defaultValue)
    : defaultValue(defaultValue)
{
}

/// ----------------------------------------------------------------------------
CRecordDescription::~CRecordDescription() {}

/// ----------------------------------------------------------------------------
int CRecordDescription::size(void) const { return names.size(); }

/// ----------------------------------------------------------------------------
StringArray::iterator CRecordDescription::columns_begin(void)
{
   return names.begin();
}

/// ----------------------------------------------------------------------------
StringArray::iterator CRecordDescription::columns_end(void)
{
   return names.end();
}

/// ----------------------------------------------------------------------------
bool CRecordDescription::addColumn(const std::string &name)
{
   return addColumn(name, defaultValue);
}

/// ----------------------------------------------------------------------------
bool CRecordDescription::addColumn(const std::string &name,
                                   const std::string &defaultValue)
{
   bool ret = true;
   if (columnNameMap.find(name) == columnNameMap.end()) {
      columnNameMap[name] = names.size();
      names.push_back(name);
      defaultValues.push_back(defaultValue);
   } else {
      ret = false; // column already exists
   }
   return ret;
}

/// ----------------------------------------------------------------------------
bool CRecordDescription::setDefaultValue(int column,
                                         const std::string &defaultValue)
{
   bool ret = true;
   if (column >= 0 && column < defaultValues.size()) {
      defaultValues[column] = defaultValue;
   } else {
      ret = false; // column does not exist
   }
   return ret;
}

/// ----------------------------------------------------------------------------
int CRecordDescription::getColumn(const std::string &columnName)
{
   int ret = -1;
   String2Idx::iterator it = columnNameMap.find(columnName);
   if (it != columnNameMap.end()) {
      ret = it->second;
   }
   return ret;
}

/// ----------------------------------------------------------------------------
const std::string &CRecordDescription::getDefaultValue(int column)
{
   if (defaultValues.size() > 0 && column < defaultValues.size()) {
      return defaultValues[column];
   } else {
      return defaultValue;
   }
}

/// ----------------------------------------------------------------------------
/// Class CResultLog
/// ----------------------------------------------------------------------------
CResultLog::CResultLog()
{
   state = result::newcol;
   curColumn = -1;
   curRow = 0;
   clear();
}

/// ----------------------------------------------------------------------------
CResultLog::~CResultLog() {}

/// ----------------------------------------------------------------------------
void CResultLog::clear(void)
{
   error = false;
   errorDesc = "Every things seems be ok";
}

/// ----------------------------------------------------------------------------
const std::string &CResultLog::getError(void) const { return errorDesc; }

/// ----------------------------------------------------------------------------
void CResultLog::eraseRecords(void) { records.clear(); }

/// ----------------------------------------------------------------------------
CResultLog &CResultLog::operator<<(const char *str)
{
   *this << std::string(str);
   return *this;
}
/// ----------------------------------------------------------------------------
int CResultLog::columnsSize(void) const { return desc.size(); }

/// ----------------------------------------------------------------------------
int CResultLog::recordsSize(void) const { return records.size(); }

/// ----------------------------------------------------------------------------
const result::StringArray::iterator CResultLog::column_names_begin(void)
{
   return desc.columns_begin();
}

/// ----------------------------------------------------------------------------
const result::StringArray::iterator CResultLog::column_names_end(void)
{
   return desc.columns_end();
}

/// ----------------------------------------------------------------------------
const CResultLog::RecordIterator CResultLog::record_at(int i)
{
   return (i > 0 && i < records.size()) ? (records.begin() + i) : records.end();
}

/// ----------------------------------------------------------------------------
const CResultLog::RecordIterator CResultLog::last()
{
   return (records.size() > 0) ? records.end() - 1 : records.end();
}

/// ----------------------------------------------------------------------------
const CResultLog::RecordIterator CResultLog::records_begin()
{
   return records.begin();
}

/// ----------------------------------------------------------------------------
const CResultLog::RecordIterator CResultLog::records_end()
{
   return records.end();
}

/// ----------------------------------------------------------------------------
int CResultLog::getColumnIndex(const std::string &name)
{
   return desc.getColumn(name);
}

/// ----------------------------------------------------------------------------
CResultLog &CResultLog::operator<<(const std::string &str)
{
   if (!error) {
      switch (state) {
      case result::addval: // add value to row, and move to next column or row
         if (desc.size() > 0) {
            if (curRow == 0 || curRow->size() == desc.size()) {
               records.push_back(result::StringArray());
               curRow = &records.back();
            }
            curRow->push_back(str);
	 } else {
            setError("At least one column must be defined");
         }
         break;
      case result::newcol: // create new column named str, next state will be
                           // defval (setting default value)
         if (desc.addColumn(str)) {
            extendAllRecord();
            state = result::defval;
	 } else {
            setError("Column '" + str + "' already exist");
         }
         break;
      case result::defval: // set new default value of current column
         if (desc.setDefaultValue(curColumn, str)) {
	 } else {
            setError("A column has not been selected");
         }
         break;
      case result::colcur: // move cursor to column named str
         if ((curColumn = desc.getColumn(str)) != -1) {
	 } else {
            setError("Column '" + str + "' has not been defined");
         }
         break;
      case result::newrec: // nothing to do for newrec
      case result::endrec: // nothing to do for endrec
      default:
         break;
      } // end switch
   }
   return *this;
}

/// ----------------------------------------------------------------------------
CResultLog &CResultLog::operator<<(const result::TResultLogOp op)
{
   if (!error) {
      switch (op) {
      case result::addval:
         state = op;
         break;
      case result::newcol:
         finishCurrentRow();
         state = op;
         break;
      case result::defval:
      case result::colcur:
         state = op;
         break;
      case result::newrec:
         finishCurrentRow();
         records.push_back(result::StringArray());
         curRow = &records.back();
         state = result::addval;
         break;
      case result::endrec:
         finishCurrentRow();
         state = result::addval;
         break;
      case result::nextcol: // add default value of current column
         if (state == result::addval) {
            if (curRow == 0 || curRow->size() == desc.size()) {
               *this << desc.getDefaultValue(0);
	    } else {
               *this << desc.getDefaultValue(curRow->size());
            }
         }
         break;
      default:
         break;
      }
   }
   return *this;
}

/// ----------------------------------------------------------------------------
CResultLog &CResultLog::operator<<(const AddColumn &op)
{
   if (desc.addColumn(op.name, op.value)) {
      extendAllRecord();
   } else {
      setError("Column '" + op.name + "' already exist");
   }
   return *this;
}

/// ----------------------------------------------------------------------------
CResultLog &CResultLog::operator<<(const SetDefaultValue &op)
{
   if (desc.setDefaultValue(desc.getColumn(op.name), op.value)) {
   } else {
      setError("Can not set default value of column '" + op.name + "'");
   }
   return *this;
}

/// ----------------------------------------------------------------------------
CResultLog &CResultLog::operator<<(const SetColumnValue &op)
{ /// TODO check for errors
   if (curRow == 0) {
      records.push_back(result::StringArray());
      curRow = &records.back();
   }
   curColumn = desc.getColumn(op.name);
   if (curColumn >= 0) {
      if (curColumn == curRow->size()) {
         curRow->push_back(op.value);
      } else if (curColumn > curRow->size()) {
         for (int i = curRow->size(); i < curColumn; i++) {
            curRow->push_back(
                desc.getDefaultValue(i)); // fill columns by default value
         }                                // end for loop
         curRow->push_back(op.value);
      } else { // just change value of column
         curRow->at(curColumn) = op.value;
      }
   } else {
      setError("Column '" + op.name + "' does not exist");
   }
   return *this;
}

/// ----------------------------------------------------------------------------
CResultLog &CResultLog::operator<<(const int v)
{
   return *this << crl::string_format<int>(v);
}

/// ----------------------------------------------------------------------------
CResultLog &CResultLog::operator<<(const unsigned int v)
{
   return *this << crl::string_format<unsigned int>(v);
}

/// ----------------------------------------------------------------------------
CResultLog &CResultLog::operator<<(const long v)
{
   return *this << crl::string_format<long>(v);
}

/// ----------------------------------------------------------------------------
CResultLog &CResultLog::operator<<(const unsigned long v)
{
   return *this << crl::string_format<unsigned long>(v);
}

/// ----------------------------------------------------------------------------
CResultLog &CResultLog::operator<<(const double v)
{
   return *this << crl::string_format<const double>(v);
}

/// ----------------------------------------------------------------------------
CResultLog &CResultLog::operator<<(const float v)
{
   return *this << crl::string_format<float>(v);
}

/// ----------------------------------------------------------------------------
CResultLog::operator bool() const { return !error; }

/// - protected method ---------------------------------------------------------
void CResultLog::setError(std::string desc)
{
   errorDesc = desc;
   error = true;
}

/// - protected method ---------------------------------------------------------
void CResultLog::finishCurrentRow(void)
{
   if (curRow && curRow->size() > 0) {
      for (int i = curRow->size(); i < desc.size(); i++) {
         curRow->push_back(desc.getDefaultValue(i));
      }
   }
}

/// - protected method ---------------------------------------------------------
void CResultLog::extendAllRecord(void)
{
   if (records.size() > 0) { // extened all record
      const std::string &defValue = desc.getDefaultValue(desc.size() - 1);
      for (RecordIterator it = records.begin(); it != records.end(); it++) {
         it->push_back(defValue);
      }
   }
}

/* end of result_log.cc */
