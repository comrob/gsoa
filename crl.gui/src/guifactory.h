/*
 * File name: guifactory.h
 * Date:      2007/03/07 21:49
 * Author:    Jan Faigl
 */

#ifndef __GUIFACTORY_H__
#define __GUIFACTORY_H__

#include <string>

#include <crl/config.h>

#include <crl/gui/gui.h>

#include <crl/gui/guievent.h>
#include <crl/gui/guihandler.h>
#include <crl/gui/guilistener.h>

namespace crl { namespace gui {

   typedef crl::gui::SGuiEvent Event;
   typedef crl::gui::CGuiListener Listener;
   typedef crl::gui::CGuiHandler Handler;
   typedef crl::gui::CGui Gui;

   /// ----------------------------------------------------------------------------
   /// @brief
   /// ----------------------------------------------------------------------------
   class CGuiFactory {
    public:
      enum TGuiType {
         NOTDEFINED,
#ifdef GUITK
         TCLTK,
#endif
         CAIRO
      };
      static CConfig &getConfig(CConfig &config);

      /// @deprecated
      /// ----------------------------------------------------------------
      static CGui *createGui(int argc, char **argv);

      /// @deprecated
      /// ----------------------------------------------------------------
      static CGui *createGui(const char *guiName);

      static CGui *createGui(CConfig &cfg);

      static std::string getGuiStrType(void);

    private:
      static TGuiType getGuiType(int argc, char **argv);
   };

}} // namespace crl::gui

#endif

/* end of guifactory.h */
