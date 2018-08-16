/*
 * File name: result_log.h
 * Date:      2009/03/04 08:40
 * Author:    Jan Faigl
 */

#ifndef __RESULT_LOG_H__
#define __RESULT_LOG_H__

#include <fstream>
#include <map>
#include <string>
#include <vector>

#include <crl/assert.h>
#include <crl/stringconversions.h>

namespace crl {
namespace result {
   typedef enum {
      addval,  // adding values
      newcol,  // create new column
      defval,  // set default column value
      colcur,  // set column cursor
      newrec,  // finalize current row nad create new one
      nextcol, // use default value of current column and move to next column
      endrec   // finalize current row
   } TResultLogOp;

   typedef std::vector<std::string> StringArray;

   class CRecordDescription {
      typedef std::map<std::string, int> String2Idx;

    public:
      CRecordDescription(const std::string &defaultValue = "-");
      ~CRecordDescription();

      /// ----------------------------------------------------------------------------
      /// @brief size
      ///
      /// @return
      /// ----------------------------------------------------------------------------
      int size(void) const;

      /// ----------------------------------------------------------------------------
      /// @brief columns_begin
      ///
      /// @return begin iterator to column names
      /// ----------------------------------------------------------------------------
      StringArray::iterator columns_begin(void);

      /// ----------------------------------------------------------------------------
      /// @brief columns_end
      ///
      /// @return end iterator to column names
      /// ----------------------------------------------------------------------------
      StringArray::iterator columns_end(void);

      /// ----------------------------------------------------------------------------
      /// @brief addColumn
      ///
      /// @param name
      ///
      /// @return false is column named name already exits
      /// ----------------------------------------------------------------------------
      bool addColumn(const std::string &name);

      /// ----------------------------------------------------------------------------
      /// @brief addColumn
      ///
      /// @param name
      /// @param defaultValue
      ///
      /// @return false is column named name already exits
      /// ----------------------------------------------------------------------------
      bool addColumn(const std::string &name, const std::string &defaultValue);

      /// ----------------------------------------------------------------------------
      /// @brief setDefaultValue
      ///
      /// @param column
      /// @param defaultValue
      ///
      /// @return false if column does not exits
      /// ----------------------------------------------------------------------------
      bool setDefaultValue(int column, const std::string &defaultValue);

      /// ----------------------------------------------------------------------------
      /// @brief getColumn
      ///
      /// @param columnName
      ///
      /// @return -1 if columnName has not been found
      /// ----------------------------------------------------------------------------
      int getColumn(const std::string &columnName);

      /// ----------------------------------------------------------------------------
      /// @brief getDefaultValue
      ///
      /// @param column
      ///
      /// @return defaultValue of column or implicit defaultValue
      /// ----------------------------------------------------------------------------
      const std::string &getDefaultValue(int column);

    private:
      std::string defaultValue;
      String2Idx columnNameMap;
      StringArray names; // column names
      StringArray defaultValues;
   };

} // end namespace result

/// ----------------------------------------------------------------------------
/// @brief
///
///        Definition of result log structure should be created before adding
///        values to log, modification after that are not recommaned, as it has
///        following issues:
///            - adding column leads to extent all records by new column with
///            default
///              value, after settings new default value it is not possible to
///              distinguish and set default values to new one
///
///
/// ----------------------------------------------------------------------------
class CResultLog {
   typedef enum {
      ADD_COLUMN,
      SET_DEFAULT_VALUE,
      SET_COLUMN_VALUE
   } TLogOperation;

   template <TLogOperation op> struct Operation {
      Operation(const std::string &name, const std::string &value)
          : name(name), value(value)
      {
      }
      const std::string &name;
      const std::string &value;
   };

 public:
   typedef std::vector<result::StringArray> RecordArray;
   typedef RecordArray::iterator RecordIterator;

   typedef Operation<ADD_COLUMN> AddColumn;
   typedef Operation<SET_DEFAULT_VALUE> SetDefaultValue;
   typedef Operation<SET_COLUMN_VALUE> SetColumnValue;

   CResultLog();
   ~CResultLog();

   /// ----------------------------------------------------------------------------
   /// @brief clear clear ok status
   /// ----------------------------------------------------------------------------
   void clear(void);

   const std::string &getError(void) const;

   /// ----------------------------------------------------------------------------
   /// @brief eraseRecords erase all stored records, columns description
   ///                     is preserved
   /// ----------------------------------------------------------------------------
   void eraseRecords(void);

   /// ----------------------------------------------------------------------------
   /// @brief columns
   ///
   /// @return number of columns in record
   /// ----------------------------------------------------------------------------
   int columnsSize(void) const;

   /// ----------------------------------------------------------------------------
   /// @brief records
   ///
   /// @return number of records
   /// ----------------------------------------------------------------------------
   int recordsSize(void) const;

   const result::StringArray::iterator column_names_begin(void);
   const result::StringArray::iterator column_names_end(void);

   const RecordIterator record_at(int i);
   const RecordIterator last();
   const RecordIterator records_begin();
   const RecordIterator records_end();

   int getColumnIndex(const std::string &name);

   template <class T> T getColumnValueAs(const RecordIterator iterator, int i)
   {
      T v;
      ::crl::assert_argument(i >= 0 && i < iterator->size(),
                             "Wrong column index");
      ::crl::assert_argument(crl::string_cast<T>(iterator->at(i), v),
                             "Can not cast string value to requested type");
      return v;
   }

   CResultLog &operator<<(const char *str);
   CResultLog &operator<<(const std::string &str);
   CResultLog &operator<<(const result::TResultLogOp op);
   CResultLog &operator<<(const AddColumn &op);
   CResultLog &operator<<(const SetDefaultValue &op);
   CResultLog &operator<<(const SetColumnValue &op);
   CResultLog &operator<<(const int v);
   CResultLog &operator<<(const unsigned int v);
   CResultLog &operator<<(const long v);
   CResultLog &operator<<(const unsigned long v);
   CResultLog &operator<<(const double v);
   CResultLog &operator<<(const float v);
   operator bool() const;

 protected:
   void setError(std::string desc);
   void finishCurrentRow(void);
   void extendAllRecord(void);

 protected:
   result::TResultLogOp state;
   bool error;
   int curColumn;

   std::string errorDesc;
   result::CRecordDescription desc;
   RecordArray records;
   result::StringArray *curRow;
};

} // end namespace crl

#endif

/* end of result_log.h */
