/*
 * File name: canvas.h
 * Date:      2008/09/07 10:26
 * Author:    Jan Faigl
 */

#ifndef __CANVAS_H__
#define __CANVAS_H__

#include <string>

#include <crl/bbox.h>
#include <crl/param_saver.h>

#include <crl/gui/shape.h>
#include <crl/gui/gui.h>

namespace crl { namespace gui {

   template <class T, int t> class ShapeParameter {
      enum { value = t };

    public:
      ShapeParameter() {}
      ShapeParameter(T v) : v(v) {}
      const T operator()(void) const { return v; }
      void set(T iV) { v = iV; }

    protected:
      T v;
   };

   namespace canvas {
      typedef enum {
         PEN_COLOR,
         FILL_COLOR,
         FILL,
         OUTLINE,
         WIDTH,
         RADIUS,
         POINT,
         POLYGON,
         LINE, // TODO LINE->SEGMENT
         LINESTRING,
         AREA,
         CLICK,
         CLEAR,
         // TODO POLYLINE ??? LINESTRING
         END,
         REDRAW,
         DEPTH,
         DEPTH_CHANGE,
         PEN_ALPHA,
         FILL_ALPHA,
         ARROW_LINE, // like the line but arrow is drawn at the end
         ARROW_LENGTH,
         ARROW_ANGLE,
         ARROW_MIN_LENGTH,
         ARC,
         SEGMENT_SET,
         NUMBER_PARAMS
      } TParam;
   } // end namespace canvas

   typedef ShapeParameter<std::string, canvas::PEN_COLOR> PenColor;
   typedef ShapeParameter<std::string, canvas::FILL_COLOR> FillColor;
   typedef ShapeParameter<bool, canvas::FILL> Fill;
   typedef ShapeParameter<bool, canvas::OUTLINE> Outline;
   typedef ShapeParameter<int, canvas::WIDTH> Width;
   typedef ShapeParameter<int, canvas::RADIUS> Radius;
   typedef ShapeParameter<int, canvas::DEPTH> Depth;
   typedef ShapeParameter<int, canvas::DEPTH_CHANGE> DepthChange;
   typedef ShapeParameter<double, canvas::PEN_ALPHA> PenAlpha;
   typedef ShapeParameter<double, canvas::FILL_ALPHA> FillAlpha;
   typedef ShapeParameter<double, canvas::ARROW_ANGLE> ArrowAngle;
   typedef ShapeParameter<int, canvas::ARROW_LENGTH> ArrowLength;
   typedef ShapeParameter<int, canvas::ARROW_MIN_LENGTH> ArrowMinLength;

   template <class T> class AddCoords {
    public:
      typedef typename T::const_iterator const_iterator;
      typedef typename T::iterator iterator;
      AddCoords(const typename T::CoordsArray &coordsArray,
                const_iterator begin, const_iterator end)
          : accessor(coordsArray), begin(begin), end(end)
      {
      }
      const double getX(const_iterator &it) const { return accessor.getX(it); }
      const double getY(const_iterator &it) const { return accessor.getY(it); }
      const double getX(iterator &it) const { return accessor.getX(it); }
      const double getY(iterator &it) const { return accessor.getY(it); }
      const T accessor;
      const_iterator begin;
      const_iterator end;
   };

   /// ----------------------------------------------------------------------------
   /// @brief General implementation of canvas
   /// ----------------------------------------------------------------------------
   class CCanvasBase {
    public:
      typedef CParamSaver<CShape> ShapeSaver;

      CCanvasBase(CGui &gui);
      ~CCanvasBase();

      CRenderer *getRenderer(void);

      void redraw(void);

      void click(void);

      void click(double &x, double &y);

      void clear(const std::string &tags);

      void drawPoint(const double x, const double y);

      void drawLine(const double x1, const double y1, const double x2,
                    const double y2);

      CShape getShape(void);

      std::string getTags(void);

      CCanvasBase &operator<<(const char *str);

      CCanvasBase &operator<<(const std::string &str);

      CCanvasBase &operator<<(const CShape &iShape);

      CCanvasBase &operator<<(const PenColor &penColor);

      CCanvasBase &operator<<(const FillColor &fillColor);

      CCanvasBase &operator<<(const Fill &fill);

      CCanvasBase &operator<<(const Outline &outline);

      CCanvasBase &operator<<(const Width &width);

      CCanvasBase &operator<<(const Radius &radius);

      CCanvasBase &operator<<(const Depth &depth);

      CCanvasBase &operator<<(const DepthChange &depth);

      CCanvasBase &operator<<(const PenAlpha &alpha);

      CCanvasBase &operator<<(const FillAlpha &alpha);

      CCanvasBase &operator<<(const ArrowAngle &angle);

      CCanvasBase &operator<<(const ArrowLength &length);

      CCanvasBase &operator<<(const ArrowMinLength &minLength);

      CCanvasBase &operator<<(const canvas::TParam &p);

      CCanvasBase &operator<<(const double &d);

      CCanvasBase &operator<<(const SColor &color);

      CCanvasBase &operator<<(const CBBox &bbox);

      CCanvasBase &operator>>(Depth &depth);

      bool save(const std::string &filename);

      template <class T, class CoordsAccessor>
      void addCoords(T begin, T end, const CoordsAccessor &accessor)
      {
         for (T it = begin; it != end; it++) {
            *this << accessor.getX(it) << accessor.getY(it);
         }
      }

      template <class T> CCanvasBase &operator<<(const AddCoords<T> &accessor)
      {
         for (typename AddCoords<T>::const_iterator it = accessor.begin;
              it != accessor.end; it++) {
            *this << accessor.getX(it) << accessor.getY(it);
         }
         return *this;
      }

    protected:
      void drawArrow(void);

    protected:
      CGui &gui;
      CRenderer *renderer;

      CShape shape;
      std::string tags;
      bool renderOpen;
      bool clearFlag;
      double v[4];
      double x;
      int dNum; // number of doubles
      canvas::TParam param;
      canvas::TParam colorParam;
      double arrowAngle;
      int arrowLength;
      int arrowMinLength;
   };

   template <class T> class CCanvas : public CCanvasBase {
    public:
      typedef T Point;
      CCanvas(CGui &gui) : CCanvasBase(gui) {}
      ~CCanvas() {}

      void drawPoint(const Point &pt)
      {
         renderer->drawPoint(pt.x, pt.y, tags, shape);
      };

      void drawLine(const Point &pt1, const Point &pt2)
      {
         renderer->open(render::LINE, tags, shape);
         renderer->addPoint(pt1.x, pt1.y);
         renderer->addPoint(pt2.x, pt2.y);
         renderer->close();
      }
   };

}} // end namespace crl::gui

crl::gui::CCanvasBase *operator<<(crl::gui::CCanvasBase *canvas,
                                  const std::string &iTags);
crl::gui::CCanvasBase *operator<<(crl::gui::CCanvasBase *canvas,
                                  const crl::gui::CShape &shape);

#endif

/* end of canvas.h */
