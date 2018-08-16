/*
 * File name: tsplibspec.cc
 * Date:      2006/04/27 08:51
 * Author:    Jan Faigl
 */

#include <ostream>

#include <crl/logging.h>
#include <crl/ws.h>

#include <crl/tsp/tsplibspec.h>

using namespace crl::tsp;

/// --------------------------------------------------------------------------
/// Class CTSPLibSpec
/// --------------------------------------------------------------------------
std::string CTSPLibSpec::KeywordsStr[] = {"NAME",
                                          "TYPE",
                                          "COMMENT",
                                          "DIMENSION",
                                          "CAPACITY",
                                          "EDGE_WEIGHT_TYPE",
                                          "EDGE_WEIGHT_FORMAT",
                                          "EDGE_DATA_FORMAT",
                                          "NODE_COORD_TYPE",
                                          "DISPLAY_DATA_TYPE",
                                          "N/A"};

/// --------------------------------------------------------------------------
std::string CTSPLibSpec::DataSectionStr[] = {
    "NODE_COORD_SECTION", "DEPOT_SECTION",       "DEMAND_SECTION",
    "EDGE_DATA_SECTION",  "FIXED_EDGES_SECTION", "DISPLAY_DATA_SECTION",
    "TOUR_SECTION",       "EDGE_WEIGHT_SECTION", "N/A"};

/// --------------------------------------------------------------------------
CTSPLibSpec::CTSPLibSpec()
{
   for (int i = 0; i < Spec::NUMBER; ++i) {
      specValue[i] = "";
   }
}

/// --------------------------------------------------------------------------
CTSPLibSpec::CTSPLibSpec(CTSPLibSpec &iSpec)
{
   for (int i = 0; i < Spec::NUMBER; ++i) {
      specValue[i] = iSpec[(Spec::TSpec)i];
   }
}

/// --------------------------------------------------------------------------
CTSPLibSpec::~CTSPLibSpec() {}

/// --------------------------------------------------------------------------
bool CTSPLibSpec::setSpec(std::string name, const std::string &value)
{
   Spec::TSpec spec;
   bool ret = getSpecification(name, spec);
   if (ret) {
      setSpec(spec, value);
   }
   return ret;
}

/// --------------------------------------------------------------------------
bool CTSPLibSpec::getSpec(std::string name, std::string &value)
{
   Spec::TSpec spec;
   bool ret = getSpecification(name, spec);
   if (ret) {
      value = specValue[spec];
   }
   return ret;
}

/// --------------------------------------------------------------------------
void CTSPLibSpec::setSpec(Spec::TSpec spec, const std::string &value)
{
   specValue[spec] = value;
}

/// --------------------------------------------------------------------------
std::string CTSPLibSpec::getSpec(Spec::TSpec spec) { return specValue[spec]; }

/// --------------------------------------------------------------------------
std::string &CTSPLibSpec::operator[](Spec::TSpec spec)
{
   return specValue[spec];
}

void CTSPLibSpec::print(std::ostream &os)
{
   bool prev = false;
   for (int i = 0; i < Spec::NUMBER; ++i) {
      if (specValue[i] != "") {
         if (prev) {
            os << ";";
         }
         os << KeywordsStr[i] << ":" << specValue[i];
         prev = true;
      }
   }
}

/// --------------------------------------------------------------------------
std::string &CTSPLibSpec::get(DataSection::Type &type)
{
   return DataSectionStr[type];
}

/// --------------------------------------------------------------------------
bool CTSPLibSpec::getDataSection(DataSection::Type &type)
{
   if (lastDataSection) {
      type = dataSection;
   }
   return lastDataSection;
}

/// --------------------------------------------------------------------------
bool CTSPLibSpec::getEof(void) { return eof; }

/// --------------------------------------------------------------------------
/// Private
/// --------------------------------------------------------------------------
bool CTSPLibSpec::getSpecification(std::string name, Spec::TSpec &spec)
{
   bool ret = false;
   for (int i = 0; i < Spec::NUMBER; ++i) {
      if (KeywordsStr[i] == name) {
         spec = (Spec::TSpec)i;
         ret = true;
         break;
      }
   }
   if (!ret) {
      WARN("Unknown TSPLIB specification '" + name + "'");
   }
   return ret;
}

/// --------------------------------------------------------------------------
bool CTSPLibSpec::getDataSection(std::string name, DataSection::Type &type)
{
   bool ret = false;
   for (int i = 0; i < DataSection::NUMBER; ++i) {
      if (DataSectionStr[i] == name) {
         type = (DataSection::Type)i;
         ret = true;
         break;
      }
   }
   if (!ret) {
      WARN("Unknown TSPLIB data section '" + name + "'");
   }
   return ret;
}

/// --------------------------------------------------------------------------
/// Operators
/// --------------------------------------------------------------------------
namespace crl { namespace tsp {

   std::ostream &operator<<(std::ostream &os, CTSPLibSpec &s)
   {
      bool prev = false;
      for (int i = 0; i < Spec::NUMBER; ++i) {
         if (s.specValue[i] != "") {
            if (prev) {
               os << ";";
            }
            os << s.KeywordsStr[i] << ":" << s.specValue[i];
            prev = true;
         }
      }
      return os;
   }

   /// --------------------------------------------------------------------------
   std::istream &operator>>(std::istream &is, CTSPLibSpec &s)
   {
      unsigned char c;
      std::string str;
      std::string value;
      int state = 0;
      bool exit = false;
      s.lastDataSection = false;
      s.eof = false;
      std::ios::fmtflags f = is.flags();
      is.unsetf(std::ios::skipws);
      while (!s.lastDataSection && is >> c) {
         if (state == 0) { // read keyword
            switch (c) {
            case ':': // end of keyword;
               state = 1;
               value = "";
               break;
            case '\n': // data keyword
               if (str == "") {
                  // skip empty line
	       } else {
                  exit = true;
                  if (str == "EOF") {
                     s.eof = true;
		  } else {
                     if (s.getDataSection(str, s.dataSection)) {
                        s.lastDataSection = true;
                     }
                  }
               }
               break;
            default:
               if (!WhiteSpace[c]) {
                  str += c;
               }
               break;
            }
	 } else { // read keyword value
            if (c == '\n') {
               if (!s.setSpec(str, value)) {
                  exit = true;
                  is.setstate(std::ios::failbit);
	       } else {
                  state = 0;
                  value = "";
                  str = "";
               }
               // check state and value
	    } else {
               if (value.size() > 0 || !WhiteSpace[c]) {
                  value += c;
               }
            }
         }
      } // end while loop
      is.flags(f);
      return is;
   }

}} // end namespace crl::tsp

/* end of tsplibspec.cc */
