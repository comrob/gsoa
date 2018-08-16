/*
 * File name: renderer.h
 * Date:      2005/12/11 22:30
 * Author:    Jan Faigl
 */

#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <string>

#include <crl/assert.h>

#include <crl/gui/guihandler.h>
#include <crl/gui/shape.h>

#include <crl/gui/manager.h>
#include <crl/gui/renderer_object.h>
#include <crl/gui/view_transform.h>


namespace crl { namespace gui { namespace render {

   ///
   /// @brief general interface for a renderers
   /// to support complex geometric structures like polygons a
   /// polyline points have to be sequentialy added (like openGL pipe)
   /// this implementations is not thread safe, so just one thread
   /// should draw by renderer used this functions
   ///
   ///
   class CRenderer {
    public:
      typedef crl::gui::CGuiHandler Handler;
      typedef crl::gui::manager::CManager Manager;
      typedef crl::gui::manager::CObject Object;

      virtual ~CRenderer() {}

      ///
      /// @brief setDimension of the renderer
      ///
      /// @param width
      /// @param height
      ///
      virtual void setDimension(int width, int height) = 0;

      double getMaxX(void) { return maxx; }
      double getMaxY(void) { return maxy; }
      double getMinX(void) { return minx; }
      double getMinY(void) { return miny; }

      ///
      /// @brief setArea set dimenstion of drawable area
      ///
      /// @param minX
      /// @param maxX
      /// @param minY
      /// @param maxY
      ///
      virtual void setArea(double minX, double maxX, double minY,
                           double maxY) = 0;

      ///
      /// @brief canvas2Real convert canvas coords to real coords
      ///
      /// @param iX
      /// @param iY
      /// @param oX
      /// @param oY
      virtual void canvas2Real(double iX, double iY, double *oX,
                               double *oY) = 0;

      ///
      /// @brief real2Canvas convert real coords to canvas coord
      ///
      /// @param iX
      /// @param iY
      /// @param oX
      /// @param oY
      virtual void real2Canvas(double iX, double iY, double *oX,
                               double *oY) = 0;

      virtual void redraw(void) = 0;

      virtual void drawPoint(double iX, double iY, const std::string &tags,
                             const CShape &shape) = 0;

      virtual void clearObjects(const std::string &tags) = 0;

      ///
      /// @brief setHandler to handle events from renderer or gui
      ///
      /// @param handler
      ///
      virtual void setHandler(Handler *handler) = 0;

      ///
      /// @brief isInitialized
      ///
      /// @return true if initialization method of renderer was already called
      ///
      virtual bool isInitialized(void) = 0;

      ///
      /// @brief open to render complex structures with
      /// many points, it is similar to openGL pipe, points
      /// can be add by addPoint, given object is created
      /// after close command
      ///
      /// It is not thread save
      ///
      /// @param object type of object
      /// @param tag
      /// @param shape
      ///
      /// @return true if given object was successfully opend
      ///
      virtual bool open(TObjectType object, const std::string &tag,
                        const CShape &shape) = 0;

      ///
      /// @brief close current object opened by open
      ///
      /// @return true if given object was successfully close (e.g. false is
      /// return if object require more than one poits, ...)
      ///
      virtual bool close(void) = 0;

      ///
      /// @brief  reset current opened object
      ///
      virtual void reset(void) = 0;

      virtual void addPoint(double x, double y) = 0;

      //              virtual void setExportDimension(int width, int height) =
      //              0;

      virtual bool save(const std::string &filename) = 0;

      virtual unsigned char *getGrayscale(int &width, int &height) = 0;

      /// ----------------------------------------------------------------------------
      /// @brief quit
      ///
      /// @return true if shutdown has been requested
      /// ----------------------------------------------------------------------------
      virtual bool shutdownRequest(void) = 0;

      /// ----------------------------------------------------------------------------
      /// @brief getObjectsNames return list of objects on coordinates x, y
      ///
      /// @param x
      /// @param y
      /// @param lst
      /// ----------------------------------------------------------------------------
      virtual void getObjectsNames(double x, double y,
                                   std::list<std::string> &lst) = 0;

      /// ----------------------------------------------------------------------------
      /// @brief getObjects return list of objects on coordinates x, y
      ///
      /// @param x
      /// @param y
      /// @param lst
      /// ----------------------------------------------------------------------------
      virtual void getObjects(double x, double y,
                              Manager::ObjectList &objects) = 0;

      /// ----------------------------------------------------------------------------
      /// @brief getManager
      ///
      /// @return
      /// ----------------------------------------------------------------------------
      virtual Manager &getManager(void) = 0;

      virtual void setBackgroundColor(const SColor &color)
      {
         assert_usage(false,
                      "Background color is not supported by this rendered");
      }

      virtual const CViewTransform &getTransform(void) = 0;

    protected:
      double minx;
      double maxx;
      double miny;
      double maxy;
   };

}}} // end namespace crl::gui::render

#endif

/* end of renderer.h */
