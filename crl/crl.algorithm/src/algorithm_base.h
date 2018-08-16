/*
 * File name: algorithm_base.h
 * Date:      2010/03/30 18:17
 * Author:    Jan Faigl
 */

#ifndef __ALGORITHM_BASE_H__
#define __ALGORITHM_BASE_H__

#include <crl/config.h>

#include <crl/alg/algorithm.h>

namespace crl { namespace alg {

   class CAlgorithmBase : public crl::CAlgorithm {
    public:
      static crl::CConfig &getConfig(CConfig &config);
      CAlgorithmBase(crl::CConfig &config);
      ~CAlgorithmBase();

      std::string getVersion(void);
      std::string getRevision(void);

    protected:
      void load(void);
      void initialize(void);
      void after_init(void){}; // nothing to do
      void iterate(int iter);
      void save(void);
      void release(void);

      void defineResultLog(void);
      void fillResultRecord(int iter, double coverage, double distCoverage);
   };

}} // end namespace crl::alg

#endif

/* end of algorithm_base.h */
