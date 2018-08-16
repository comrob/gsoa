/*
 * File name: colors.h
 * Date:      2006/04/30 15:57
 * Author:    Jan Faigl
 */

#ifndef __COLORS_H__
#define __COLORS_H__

#include <string>
#include <vector>

namespace crl { namespace gui {

   typedef std::string Color;
   typedef std::vector<Color> TColors;

   class CColors : public TColors {
      int i;

    public:
      CColors();
      ~CColors();
      void reset(void);

      ///
      /// @brief cur
      ///
      /// @return current color
      ///
      Color &cur(void);

      ///
      /// @brief next
      ///
      /// @return
      ///
      Color &next(void);
   };

   namespace Colors {
      extern CColors DEFAULT;
   }

}} // namespace crl::gui

#endif

/* end of colors.h */
