/*
 * File name: param_saver.h
 * Date:      2009/02/20 18:43
 * Author:    Jan Faigl
 */

#ifndef __PARAM_SAVER_H__
#define __PARAM_SAVER_H__

namespace crl {

template <class T> class CParamSaver {
 public:
   CParamSaver(T &param) : param(param), paramSave(param), save(true) {}

   ~CParamSaver()
   {
      if (save) {
         param = paramSave;
      }
   }

   void release(void) { save = false; }

 private:
   T &param;
   T paramSave;
   bool save;
};

} // end namespace crl

#endif

/* end of param_saver.h */
