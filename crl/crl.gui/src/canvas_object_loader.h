/*
 * File name: canvas_object_loader.h
 * Date:      2009/07/25 22:41
 * Author:    Jan Faigl
 */

#ifndef __CANVAS_OBJECT_LOADER_H__
#define __CANVAS_OBJECT_LOADER_H__

#include <istream>
#include <string>

#include <crl/gui/canvas.h>

namespace crl { namespace gui {

   class CCanvasObjectLoader {
    public:
      CCanvasObjectLoader(const std::string &filename, CCanvasBase &canvas);

    protected:
      void load(std::istream &in, CCanvasBase &canvas);
   };

}} // end namespace crl::gui

#endif

/* end of canvas_object_loader.h */
