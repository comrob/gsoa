/*
 * File name: win_adjust_size.cc
 * Date:      2009/08/19 15:21
 * Author:    Jan Faigl
 */

#include <cmath>

#include <crl/gui/win_adjust_size.h>

using namespace crl::gui;

/// ----------------------------------------------------------------------------
int getWidth(double ar, int height, int borderX, int borderY,
             const crl::CBBox &bbox)
{
   // ar  width / height
   const double realAspect = bbox.width() / bbox.height();
   return (int)ceil(realAspect * (height - 2 * borderY) / ar) + 2 * borderX;
}

/// ----------------------------------------------------------------------------
int getHeight(double ar, int width, int borderX, int borderY,
              const crl::CBBox &bbox)
{
   // ar  width / height
   const double realAspect = bbox.width() / bbox.height();
   return (int)ceil(ar * (width - 2 * borderX) / realAspect) + 2 * borderY;
}

/// ----------------------------------------------------------------------------
/// Class CWinAdjustSize

/// - static method ------------------------------------------------------------
crl::CConfig &CWinAdjustSize::getConfig(CConfig &config)
{
   config.add<bool>(
       "gui-win-size-adjust",
       "If true window size is change (if CWinAdjustSize::adjust is called)",
       false);
   config.add<bool>("gui-win-width-preference",
                    "If true width is prefered otherwise height is prefered in "
                    "win size adjust",
                    true);
   return config;
}

/// - static method ------------------------------------------------------------
crl::CConfig &CWinAdjustSize::adjust(const crl::CBBox &bbox, CConfig &cfg)
{
   const double ar = cfg.get<double>("gui-aspect-ratio");
   if (cfg.get<bool>("gui-win-size-adjust") && ar > 0.) {
      if (cfg.get<bool>("gui-win-width-preference")) {
         // set height
         int height = getHeight(ar, cfg.get<int>("gui-width"),
                                cfg.get<int>("gui-border-x"),
                                cfg.get<int>("gui-border-y"), bbox);
         cfg.set<int>("gui-height", height);
         // INFO("Adjust Gui height to " << height);
      } else {
         // set width
         int width = getWidth(ar, cfg.get<int>("gui-height"),
                              cfg.get<int>("gui-border-x"),
                              cfg.get<int>("gui-border-y"), bbox);
         cfg.set<int>("gui-width", width);
         // INFO("Adjust Gui width to " << width);
      }
   }
   return cfg;
}

/* end of win_adjust_size.cc */
