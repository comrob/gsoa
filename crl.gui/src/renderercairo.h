/*
 * File name: renderercairo.h
 * Date:      2007/02/17 18:38
 * Author:    Jan Faigl
 */

#ifndef __RENDERERCAIRO_H__
#define __RENDERERCAIRO_H__

#include <crl/config.h>
#include <crl/logging.h>

#include <crl/gui/renderer.h>
#include <crl/gui/shapes.h>
#include <crl/gui/view_transform.h>
#include <crl/gui/cairowin.h>
#include <crl/gui/canvascontainer.h>

using crl::gui::cairo::CCairoWin;
using crl::gui::cairo::CCanvasContainer;

namespace crl { namespace gui { namespace render {

   class CRendererCairo : public CRenderer {
      static bool initialized;
      TObjectType state;
      // transformation
      CViewTransform transform;

      int exportWidth;
      int exportHeight;
      int count;
      CCanvasContainer *canvas;
      CCairoWin *win;
      crl::gui::cairo::SCanvasObject *_obj;

    public:
      CRendererCairo(CCanvasContainer *iCanvas, CCairoWin *iWin);
      CRendererCairo(CCanvasContainer *iCanvas, CCairoWin *iWin, CConfig &cfg);
      virtual ~CRendererCairo();

      void setDimension(int width, int height);

      void setArea(double minX, double maxX, double minY, double maxY);

      void canvas2Real(double iX, double iY, double *oX, double *oY);

      void real2Canvas(double iX, double iY, double *oX, double *oY);

      void redraw(void);

      void drawPoint(double iX, double iY, const std::string &tags,
                     const CShape &shape);

      void clearObjects(const std::string &tags);

      void setHandler(Handler *handler);

      bool isInitialized(void);

      bool open(TObjectType object, const std::string &tag,
                const CShape &shape);

      bool close(void);

      void reset(void);

      void addPoint(double x, double y);

      //           void setExportDimension(int width, int height);

      bool save(const std::string &filename);

      unsigned char *getGrayscale(int &width, int &height);

      bool shutdownRequest(void);

      void getObjectsNames(double x, double y, std::list<std::string> &lst);

      void getObjects(double x, double y, Manager::ObjectList &objects);

      Manager &getManager(void);

      void setBackgroundColor(const SColor &color);

      const CViewTransform &getTransform(void);

    protected:
      bool antialiasing;
   };

}}} // end namespace crl::gui::render

#endif

/* end of renderercairo.h */
