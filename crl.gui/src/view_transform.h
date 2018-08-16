/*
 * File name: view_transform.h
 * Date:      2009/02/21 16:02
 * Author:    Jan Faigl
 */

#ifndef __VIEW_TRANSFORM_H__
#define __VIEW_TRANSFORM_H__

#include <string>

#include <crl/bbox.h>

namespace crl { namespace gui {

   /// ----------------------------------------------------------------------------
   /// @brief Transform Coords form real to canvas
   ///         real coords      <->          view coords
   /// max y -|                          0   -|
   ///        |                  0+border y  -|
   ///        |                               |
   ///        |            height - border y -|
   ///        |                               |
   /// min y -|-----------------|      height-|----|------------|-----------|
   /// min x                  max x           0  border x  width-border_x width
   ///
   /// ----------------------------------------------------------------------------
   class CViewTransform {
    public:
      CViewTransform();
      ~CViewTransform();

      void setView(double width, double height);

      /// ----------------------------------------------------------------------------
      /// @brief setAspectRation set aspection ration for <= 0 fit to the screen
      ///                        otherwise real coords are recompute according
      ///                        to given aspection ration
      ///
      /// @param arI
      /// ----------------------------------------------------------------------------
      void setAspectRation(double arI);

      /// ----------------------------------------------------------------------------
      /// @brief setViewBorder restrict used view is view - (2*border)
      ///
      /// @param borderX
      /// @param borderY
      /// ----------------------------------------------------------------------------
      void setViewBorder(double borderX, double borderY);

      void setReal(double minx, double maxx, double miny, double maxy);

      void view2real(double x, double y, double &oX, double &oY) const;
      void real2view(double x, double y, double &oX, double &oY) const;

      double view2realX(double x) const;
      double view2realY(double y) const;
      double real2viewX(double x) const;
      double real2viewY(double y) const;

      std::string getParam(void) const;

      crl::CBBox &getBBox(crl::CBBox &bbox) const;

      double getViewX(void) const { return viewX; }
      double getViewY(void) const { return viewY; }

      double getCanvasLengthX(double realD) const;
      double getCanvasLengthY(double realD) const;

      double getRatioX(void) const { return ratioX; }
      double getRatioY(void) const { return ratioY; }

    protected:
      void compute(void);
      double viewX, viewY;
      double viewBorderX, viewBorderY;
      double realMaxX, realMaxY;
      double realMinX, realMinY;
      double kX, kY;
      double ratioX, ratioY;

      double ar;
   };

}} // end namespace crl::gui

#endif

/* end of view_transform.h */
