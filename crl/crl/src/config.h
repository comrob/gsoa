/*
 * File name: config.h
 * Date:      2009/01/24 10:54
 * Author:    Jan Faigl
 */

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <list>
#include <map>
#include <ostream>
#include <string>
#include <typeinfo>

#include <crl/assert.h>

namespace crl {

/// ----------------------------------------------------------------------------
/// @brief General class to support configurations options
/// ----------------------------------------------------------------------------
class CConfig {
 public:
   typedef std::list<std::string> NameList;

 private:
   struct parameter_base_s {
      parameter_base_s()
          : isSet(false), hasDefaultValue(false), name(""), description("")
      {
      }
      parameter_base_s(const std::string &name,
                       const std::string &description = "")
          : isSet(false), hasDefaultValue(false), name(name),
            description(description)
      {
      }
      bool isSet;
      bool hasDefaultValue;
      std::string name;
      std::string description;
   };

   template <class T> struct parameter_s : public parameter_base_s {
      parameter_s() : parameter_base_s() {}
      parameter_s(const std::string &name, const std::string &description = "")
          : parameter_base_s(name, description)
      {
      }
      parameter_s(const std::string &name, const std::string &description,
                  const T &defaultValue)
          : parameter_base_s(name, description)
      {
         default_value = defaultValue;
         hasDefaultValue = true;
         value = default_value;
      }

      void set(T v)
      {
         value = v;
         isSet = true;
      }

      T value;
      T default_value;
   };

   struct directory_base_s {
      virtual ~directory_base_s() {}
   };

   template <class Value> struct directory_s : public virtual directory_base_s {
      typedef Value value_type;
      typedef parameter_s<Value> Parameter;
      typedef std::map<std::string, Parameter> type;
      typedef typename type::iterator iterator;
      type values;

      directory_s() {}
      virtual ~directory_s() {}

      void set(std::string name, Value v)
      {
         iterator it = values.find(name);
         assert_usage(it != values.end(),
                      "Value named '" + name + "' is not known");
         it->second.value = v;
         it->second.isSet = true;
      }

      Parameter &add(std::string name, std::string description = "")
      {
         iterator it = values.find(name);
         assert_usage(it == values.end(),
                      "Value named '" + name + "' has been already added");
         values[name] = Parameter(name, description);
         return values[name];
      }

      Parameter &add(std::string name, std::string description,
                     Value defaultValue)
      {
         iterator it = values.find(name);
         assert_usage(it == values.end(),
                      "Value named '" + name + "' has been already added");
         values[name] = Parameter(name, description, defaultValue);
         return values[name];
      }

      Value &get(std::string name) { return getParameter(name).value; }

      Value *getPtr(std::string name) { return &getParameter(name).value; }

      bool hasDefaultValue(std::string name)
      {
         return getParameter(name).hasDefaultValue;
      }

      Value &getDefaultValue(std::string name)
      {
         return getParameter(name).default_value;
      }

      const std::string &getDescription(std::string name)
      {
         return getParameter(name).description;
      }

      void getNames(NameList &names)
      {
         for (iterator it = values.begin(); it != values.end(); it++) {
            names.push_back(it->second.name);
         }
      }

    private:
      Parameter &getParameter(std::string &name)
      {
         iterator it = values.find(name);
         assert_usage(it != values.end(),
                      "Value named '" + name + "' is not known");
         return it->second;
      }
   };

 public:
   CConfig() {}
   ~CConfig() {}

 protected:
 public:
   template <class T> void add(std::string name, std::string description = "")
   {
      TType type = getDirectoryType<T>(T());
      itemList.push_back(
          SItem(dynamic_cast<directory_s<T> *>(getDirectory<T>(T(), type))
                    ->add(name, description)
                    .name,
                type));
   }

   template <class T>
   void add(std::string name, std::string description, T defaultValue)
   {
      TType type = getDirectoryType<T>(T());
      itemList.push_back(
          SItem(dynamic_cast<directory_s<T> *>(getDirectory<T>(T(), type))
                    ->add(name, description, defaultValue)
                    .name,
                type));
   }

   template <class T> void set(std::string name, T v)
   {
      dynamic_cast<directory_s<T> *>(getDirectory<T>(v))->set(name, v);
   }

   template <class T> T &get(std::string name)
   {
      return dynamic_cast<directory_s<T> *>(getDirectory<T>(T()))->get(name);
   }

   template <class T> T *getPtr(std::string name)
   {
      return dynamic_cast<directory_s<T> *>(getDirectory<T>(T()))->getPtr(name);
   }

   template <class T> bool hasDefaultValue(std::string name)
   {
      return dynamic_cast<directory_s<T> *>(getDirectory<T>(T()))
          ->hasDefaultValue(name);
   }

   template <class T> T &getDefaultValue(std::string name)
   {
      return dynamic_cast<directory_s<T> *>(getDirectory<T>(T()))
          ->getDefaultValue(name);
   }

   template <class T> const std::string &getDescription(std::string name)
   {
      return dynamic_cast<directory_s<T> *>(getDirectory<T>(T()))
          ->getDescription(name);
   }

   template <class T> void getNames(NameList &names)
   {
      return dynamic_cast<directory_s<T> *>(getDirectory<T>(T()))
          ->getNames(names);
   }

   typedef enum {
      STRING_CRL,
      INT_CRL,
      DOUBLE_CRL,
      FLOAT_CRL,
      BOOL_CRL,
      NUM_TYPES
   } TType;

   struct SItem {
      const std::string &name;
      TType type;
      SItem(const SItem &item) : name(item.name), type(item.type) {}
      SItem(std::string &name, TType type) : name(name), type(type) {}
   };

 protected:
   typedef directory_s<std::string> StringDirectory;
   typedef directory_s<int> IntDirectory;
   typedef directory_s<double> DoubleDirectory;
   typedef directory_s<float> FloatDirectory;
   typedef directory_s<bool> BoolDirectory;
   typedef std::list<SItem> ItemList;

 public:
   typedef ItemList::iterator item_iterator;

   item_iterator items_begin(void) { return itemList.begin(); }

   item_iterator items_end(void) { return itemList.end(); }

   void print(std::ostream &out)
   {
      for (CConfig::item_iterator it = items_begin(); it != items_end(); it++) {
         out << it->name << "=";
         switch (it->type) {
         case STRING_CRL:
            out << get<std::string>(it->name);
            break;
         case INT_CRL:
            out << get<int>(it->name);
            break;
         case DOUBLE_CRL:
            out << get<double>(it->name);
            break;
         case FLOAT_CRL:
            out << get<float>(it->name);
            break;
         case BOOL_CRL:
            out << get<bool>(it->name);
            break;
         default:
            break;
         } // end switch
         out << std::endl;
      } // end for loop
   }

 protected:
   template <class T> TType getDirectoryType(const T &v)
   {
      const std::type_info &t = typeid(v);
      if (t == typeid(StringDirectory::value_type)) {
         return STRING_CRL;
      }
      if (t == typeid(IntDirectory::value_type)) {
         return INT_CRL;
      }
      if (t == typeid(DoubleDirectory::value_type)) {
         return DOUBLE_CRL;
      }
      if (t == typeid(FloatDirectory::value_type)) {
         return FLOAT_CRL;
      }
      if (t == typeid(BoolDirectory::value_type)) {
         return BOOL_CRL;
      }
      return NUM_TYPES;
   }

   template <class T>
   directory_base_s *getDirectory(const T &v, TType type = NUM_TYPES)
   {
      switch ((type == NUM_TYPES) ? getDirectoryType<T>(v) : type) {
      case STRING_CRL:
         return &strings;
      case INT_CRL:
         return &ints;
      case DOUBLE_CRL:
         return &doubles;
      case FLOAT_CRL:
         return &floats;
      case BOOL_CRL:
         return &bools;
      default:
         return 0;
      } // end switch
   }

   ItemList itemList;

   StringDirectory strings;
   IntDirectory ints;
   DoubleDirectory doubles;
   FloatDirectory floats;
   BoolDirectory bools;
};

} // end namespace crl

#endif

/* end of config.h */
