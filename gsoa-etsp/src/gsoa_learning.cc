/*
 * File name: gsoa_learning.cc
 * Date:      2016/12/09 22:54
 * Author:    Jan Faigl
 */

#include <cmath>

#include "gsoa_learning.h"

#include <crl/logging.h>
using crl::logger;

namespace gsoa {

   Schema *schema; // global variable

}

using namespace gsoa;

/// ----------------------------------------------------------------------------
/// Struct Schema
/// ----------------------------------------------------------------------------

/// - static method ------------------------------------------------------------
crl::CConfig& Schema::getConfig(crl::CConfig &config) 
{
   config.add<double>("init-learning-gain", "Initial value of the learning gain", 10.0);
   config.add<double>("gain-decreasing-rate", "Decrasing rate, higher value means faster convergence", 1e-4);
   config.add<double>("neighborhood-factor",
         "factor to compute number of neighborhood neurons as division of number of neurons, neighborhood are considered on both sides (left, right) of winning neuron so d*2 neurons are moved ",
         5);
   config.add<double>("learning-rate", "neuron adaptation parametr in activation function mi*exp(d)", 0.6); //0.6
   config.add<bool>("enable-reward-learning-rate", "Enable/disable learning rate according to the reward, if enabled, it disable the GCD", true);
   config.add<bool>("force-already-covered", "Enable/disable force quiet of winner selection when the target is already covered from the ring", false);
   config.add<double>("gain-neighborhood-decrease", "Adjustment of the learning gain due to low number of neurons", 1.0); // 0.25 beucase neurons are not placed in the middle of two winners
   config.add<double>("min-gain", "Minimal value of the gain", 1e-5);
   //
   config.add<bool>("best-solution", "Enable/disable considering best found solution the evaluation", false);
   config.add<int>("termination-max-steps", "Stop adaptation if number of steps is large than given", 180);
   config.add<double>("termination-error", "Stop adaptation when the current error is less than this value", 0.001);
   config.add<bool>("termination-change", "Stop adaptation when the current route is the same as the previous one (route as a sequence of target labels)", false); 
   return config;
}

/// - constructor --------------------------------------------------------------
Schema::Schema(crl::CConfig& config) :
   INIT_LEARNING_GAIN(config.get<double>("init-learning-gain")),
   GAIN_DECREASING_RATE(config.get<double>("gain-decreasing-rate")),
   NEIGHBORHOOD_FACTOR(config.get<double>("neighborhood-factor")),
   REWARD_LEARNING_RATE(config.get<bool>("enable-reward-learning-rate")),
   FORCE_ALREADY_COVERED(config.get<bool>("force-already-covered")),
   GAIN_NEIGHBORHOOD_DECREASE(config.get<double>("gain-neighborhood-decrease")),
   MIN_GAIN(config.get<double>("min-gain"))
{
   exps = 0;
   expN = 0;
}


/// - destructor ---------------------------------------------------------------
Schema::~Schema()
{
   if (exps) {
      delete exps;
   }
}

/// - public method ------------------------------------------------------------
void Schema::updateExp(int n, int step)
{
   if (expN < 2*n) {
      if (exps) { delete[] exps; }
      expN = 2 * n;
      exps = new double[expN];
   }
   const double GG = GAIN_NEIGHBORHOOD_DECREASE * G * G;
   double dd = 1.0;
   explen = expN;
   exps[0] = mi * exp((-dd * dd) / GG);
   dd += 1.0;
   for (int i = 1; i < expN; ++i, dd += 1.0) {
      exps[i] = mi * exp((-dd * dd) / GG);
      if (exps[i] < MIN_GAIN) {
         explen = i - 1;
         break;
      }

   }
}

/* end of gsoa_learning.cc */
