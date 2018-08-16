/*
 * File name: tsplibspec.h
 * Date:      2006/04/27 08:46
 * Author:    Jan Faigl
 */

#ifndef __TSPLIBSPEC_H__
#define __TSPLIBSPEC_H__

#include <iostream>
#include <string>

namespace crl { namespace tsp {

   namespace Spec {
      typedef enum {
         NAME,
         TYPE,
         COMMENT,
         DIMENSION,
         CAPACITY,
         EDGE_WEIGHT_TYPE,
         EDGE_WEIGHT_FORMAT,
         EDGE_DATA_FORMAT,
         NODE_COORD_TYPE,
         DISPLAY_DATA_TYPE,
         NUMBER
      } TSpec;
   } // namespace Spec

   namespace DataSection {
      typedef enum {
         NODE_COORD_SECTION,
         DEPOT_SECTION,
         DEMAND_SECTION,
         EDGE_DATA_SECTION,
         FIXED_EDGES_SECTION,
         DISPLAY_DATA_SECTION,
         TOUR_SECTION,
         EDGE_WEIGHT_SECTION,
         NUMBER
      } Type;
   } // namespace DataSection

   /// --------------------------------------------------------------------------
   /// @brief specification of TSP lib problem
   /// --------------------------------------------------------------------------
   class CTSPLibSpec {
      static std::string KeywordsStr[];
      static std::string DataSectionStr[];
      std::string specValue[Spec::NUMBER];

      bool eof;
      bool lastDataSection;
      DataSection::Type dataSection;

    public:
      CTSPLibSpec();
      CTSPLibSpec(CTSPLibSpec &iSpec);
      ~CTSPLibSpec();

      /// --------------------------------------------------------------------------
      /// @brief setSpec set specification with name and value
      ///
      /// @param name
      /// @param value
      ///
      /// @return false if specification with given name does not exists
      /// --------------------------------------------------------------------------
      bool setSpec(std::string name, const std::string &value);

      /// --------------------------------------------------------------------------
      /// @brief getSpec
      ///
      /// @param name name of specification
      /// @param value return value
      ///
      /// @return false if specification with given name does not exists
      /// --------------------------------------------------------------------------
      bool getSpec(std::string name, std::string &value);

      /// --------------------------------------------------------------------------
      /// @brief setSpec set specification value
      ///
      /// @param spec
      /// @param value
      /// --------------------------------------------------------------------------
      void setSpec(Spec::TSpec spec, const std::string &value);

      /// --------------------------------------------------------------------------
      /// @brief getSpec get specification value
      ///
      /// @param spec
      ///
      /// @return
      /// --------------------------------------------------------------------------
      std::string getSpec(Spec::TSpec spec);

      std::string &operator[](Spec::TSpec spec);

      void print(std::ostream &os);

      std::string &get(DataSection::Type &type);

      ///
      /// @brief dataSection
      ///
      /// @param type last data section keyword translated to enum
      ///
      /// @return true if last key in stream was data section keyword
      ///
      bool getDataSection(DataSection::Type &type);

      /// --------------------------------------------------------------------------
      /// @brief getEof
      ///
      /// @return true if eof was read
      /// --------------------------------------------------------------------------
      bool getEof(void);

    private:
      /// --------------------------------------------------------------------------
      /// @brief getSpecification
      ///
      /// @param name
      /// @param spec
      ///
      /// @return false if std::string is not valid specification
      /// -------------------------------------------------------------------------
      bool getSpecification(std::string name, Spec::TSpec &spec);

      ///
      /// @brief getDataSection
      ///
      /// @param name
      /// @param type
      ///
      /// @return false is std::string is not valid datas section
      ///
      bool getDataSection(std::string name, DataSection::Type &type);

      friend std::ostream &operator<<(std::ostream &os, CTSPLibSpec &s);
      friend std::istream &operator>>(std::istream &is, CTSPLibSpec &s);
   };

}} // end namespace crl::tsp

#endif

/* end of tsplibspec.h */
