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
      const double INIT_LEARNING_GAIN;
      const double GAIN_DECREASING_RATE;
      const double NEIGHBORHOOD_FACTOR;
      const bool REWARD_LEARNING_RATE;
      const bool FORCE_ALREADY_COVERED;
      const double GAIN_NEIGHBORHOOD_DECREASE;

      const double MIN_GAIN;

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
