/*
 * File name: renderer_object.h
 * Date:      2009/02/21 07:33
 * Author:    Jan Faigl
 */

#ifndef __RENDERER_OBJECT_H__
#define __RENDERER_OBJECT_H__

#include <list>

#include <crl/bbox.h>

#include <crl/gui/shape.h>
#include <crl/gui/view_transform.h>

namespace crl { namespace gui { namespace render {

   typedef enum {
      AREA,
      POLYGON,
      LINE,
      POINT,
      ARC,
      SEGMENT_SET,
      NUMBER
   } TObjectType;

   class CCoordsTransform {
    public:
      virtual double getX(double x) const { return x; }
      virtual double getY(double y) const { return y; }
      virtual std::string getName(void) const { return "1:1"; }
      virtual std::string getTransformParam(void) const { return "N/A"; }

      virtual crl::CBBox &getBBox(crl::CBBox &bbox) const { return bbox; }
   };

   /// ----------------------------------------------------------------------------
   /// @brief
   /// ----------------------------------------------------------------------------
   class CView2Real : public CCoordsTransform {
    public:
      CView2Real(const CViewTransform &transform)
          : CCoordsTransform(), transform(transform)
      {
      }
      virtual ~CView2Real() {}
      double getX(double x) const { return transform.view2realX(x); }
      double getY(double y) const { return transform.view2realY(y); }
      std::string getName(void) const { return "View2Real"; }
      virtual std::string getTransformParam(void) const
      {
         return transform.getParam();
      }
      virtual crl::CBBox &getBBox(crl::CBBox &bbox) const
      {
         return transform.getBBox(bbox);
      }

    private:
      const CViewTransform &transform;
   };

   /// ----------------------------------------------------------------------------
   /// @brief
   /// ----------------------------------------------------------------------------
   class CReal2View : public CCoordsTransform {
    public:
      CReal2View(const CViewTransform &transform)
          : CCoordsTransform(), transform(transform)
      {
      }
      virtual ~CReal2View() {}
      double getX(double x) const { return transform.real2viewX(x); }
      double getY(double y) const { return transform.real2viewY(y); }
      std::string getName(void) const { return "Real2View"; }
      virtual std::string getTransformParam(void) const
      {
         return transform.getParam();
      }
      virtual crl::CBBox &getBBox(crl::CBBox &bbox) const
      {
         return transform.getBBox(bbox);
      }

    private:
      const CViewTransform &transform;
   };

   /// ----------------------------------------------------------------------------
   /// @brief
   /// ----------------------------------------------------------------------------
   struct SRendererObject {

      struct Coord {
         double x;
         double y;
         Coord(double x, double y) : x(x), y(y) {}
      };

      typedef crl::gui::CShape CShape;
      typedef crl::gui::render::TObjectType ObjectType;
      typedef std::list<Coord> Coords;

      /// ----------------------------------------------------------------------------
      /// @brief SRendererObject
      ///
      /// @param type
      /// ----------------------------------------------------------------------------
      SRendererObject(ObjectType type, const CShape &shape);

      /// ----------------------------------------------------------------------------
      /// @brief addPoint
      ///
      /// @param x
      /// @param y
      /// ----------------------------------------------------------------------------
      void addPoint(const double &x, const double &y);

      //            std::ostream & operator<<(std::ostream & out);

      void simplePrint(std::ostream &out);

      void simplePrint(std::ostream &out, const CCoordsTransform &transform);

      const Coords &getCoords(void) const { return coords; }

      CShape shape;
      ObjectType type;
      Coords coords;
      int size;
   };

}}} // end namespace crl::gui::render

#endif

/* end of renderer_object.h */
