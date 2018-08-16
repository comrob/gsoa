/*
 * File name: canvas_object_saver.h
 * Date:      2009/07/25 21:29
 * Author:    Jan Faigl
 */

#ifndef __CANVAS_OBJECT_SAVER_H__
#define __CANVAS_OBJECT_SAVER_H__

#include <ostream>
#include <string>

#include <crl/gui/manager.h>
#include <crl/gui/renderer_object.h>

namespace crl { namespace gui {
   class CCanvasObjectSaver : public manager::CObjectExecutor {
    public:
      CCanvasObjectSaver(const std::string &filename,
                         const render::CCoordsTransform *transform = 0);
      CCanvasObjectSaver(std::ostream &out,
                         const render::CCoordsTransform *transform = 0);
      virtual ~CCanvasObjectSaver();

      virtual void execute(manager::CObject *object);

    protected:
      void header(void);

    protected:
      std::ostream *out_;
      bool destroy;
      const render::CCoordsTransform *transform;
   };

}} // end namespace crl::gui

#endif

/* end of canvas_object_saver.h */
