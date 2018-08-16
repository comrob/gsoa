/*
 * File name: gsoa_learning.h
 * Date:      2016/12/09 22:53
 * Author:    Jan Faigl
 */

#ifndef __GSOA_LEARNING_H__
#define __GSOA_LEARNING_H__

#include <string> 

#include <crl/config.h>

namespace gsoa {

   struct Schema {
      const double GAIN_DECREASING_RATE;
      const double NEIGHBORHOOD_FACTOR;

      const double MIN_GAIN;

      const double COMM_RADIUS;
      const double COMM_RADIUS2;
      const double COMM_RADIUS_SMALLER;

      static crl::CConfig &getConfig(crl::CConfig &config);
      Schema(crl::CConfig &config);
      ~Schema();

      void updateExp(int n, int step);

      double mi;
      double G; //variable
      int d; //number neighborhood neurons

      double *exps;
      int expN;
      int explen;

   };

   extern Schema *schema;

} // end namespace gsoa

#endif

/* end of gsoa_learning.h */
