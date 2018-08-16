/*
 * File name: win_adjust_size.h
 * Date:      2009/08/19 15:19
 * Author:    Jan Faigl
 */

#ifndef __WIN_ADJUST_SIZE_H__
#define __WIN_ADJUST_SIZE_H__

#include <crl/bbox.h>
#include <crl/config.h>

namespace crl { namespace gui {

   /// ----------------------------------------------------------------------------
   /// @brief
   /// ----------------------------------------------------------------------------
   class CWinAdjustSize {
    public:
      static CConfig &getConfig(CConfig &config);

      static CConfig &adjust(const crl::CBBox &bbox, CConfig &cfg);

      template <class T> static CConfig &adjust(const T &array, CConfig &cfg)
      {
         crl::CBBox bbox;
         for (typename T::const_iterator it = array.begin(); it != array.end();
              it++) {
            bbox.add(it->x, it->y);
         }
         return adjust(bbox, cfg);
      }
   };

}} // end namespace crl::gui

#endif

/* end of win_adjust_size.h */
