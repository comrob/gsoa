/*
 * File name: shape.h
 * Date:      2005/10/21 16:36
 * Author:    Jan Faigl
 */

#ifndef __SHAPE_H__
#define __SHAPE_H__

#include <string>

#include <crl/exceptions.h>

namespace crl { namespace gui {

   struct SColor {
      double red;
      double green;
      double blue;
      double alpha; // 0.0 - transparet, 1.0 - full cover
      void set(double r, double g, double b, double a)
      {
         red = r;
         green = g;
         blue = b;
         alpha = a;
      }

      SColor() : red(0.0), green(0.0), blue(0.0), alpha(0.0) {}

      SColor(const std::string &colorname);

      SColor &operator=(const SColor &other)
      {
         if (this != &other) {
            set(other.red, other.green, other.blue, other.alpha);
         }
         return *this;
      }
   };

   /// ----------------------------------------------------------------------------
   /// @brief Class to represent graphical shape
   /// default values for optionals parameters are
   /// penAlpha = 1.0;
   /// fillAlpha = 1.0;
   /// zoom = false;
   /// fill = true
   ///
   /// if fillColor == "" => fill = false;
   /// if fillColor is missing than fill can not be set to true
   /// ----------------------------------------------------------------------------
   class CShape {
    protected:
      std::string strPenColor;
      std::string strFillColor;
      SColor penColor;
      SColor fillColor;
      bool zoom;
      bool fill;
      bool outline;
      int width;
      int radius;
      int depth;
      static const bool ZOOM;
      static const bool FILL;
      static const bool OUTLINE;
      static const double PEN_ALPHA;
      static const double FILL_ALPHA;
      enum { DEFAULT_DEPTH = 50 };

    public:
      static std::string getStringOptions(void);
      static SColor getColor(std::string colorname);

      CShape();
      CShape(const CShape &shape);
      CShape(CShape &shape);
      CShape(const char *iPenColor, const char *iFillColor, int iWidth,
             int iRadius, const double penAlpha = PEN_ALPHA,
             const double fillAlpha = PEN_ALPHA, int depth = DEFAULT_DEPTH);
      CShape(const std::string &shape);
      //           CShape(std::string& iPenColor, std::string& iFillColor, int
      //           iWidth, int iRadius, const double& penAlpha = PEN_ALPHA,
      //           const double& fillAlpha = PEN_ALPHA);
      //         CShape(std::string& iPenColor, std::string& iFillColor, int
      //         iWidth, int iRadius, bool iZoom = ZOOM, bool iFill = FILL, bool
      //         iOutline = OUTLINE);
      //       CShape(std::string& iPenColor, std::string& iFillColor, int
      //       iWidth, int iRadius, const double& penAlpha = PEN_ALPHA, const
      //       double& fillAlpha = FILL_ALPHA, bool iZoom = ZOOM, bool iFill =
      //       FILL, bool iOutline = OUTLINE);

      CShape &operator=(const CShape &other);

      std::string operator()(void) const;

      void setPenColor(const std::string &color);
      void setFillColor(const std::string &color);
      void setPenColor(const SColor &color);
      void setFillColor(const SColor &color);
      void setPenAlpha(double alpha);
      void setFillAlpha(double alpha);
      void setPenWidth(const int iWidth);
      void setRadius(const int iRadius);
      void setZoom(bool iZoom);
      void setFill(bool iFill);
      void setOutline(bool iOutline);
      void setDepth(int iDepth);
      // TODO line style

      inline const std::string &getPenColorStr(void) const
      {
         return strPenColor;
      }

      inline const std::string &getFillColorStr(void) const
      {
         return strFillColor;
      }

      inline const SColor &getPenColor(void) const { return penColor; }

      inline const SColor &getFillColor(void) const { return fillColor; }

      inline double getPenAlpha(void) const { return penColor.alpha; }

      inline double getFillAlpha(void) const { return fillColor.alpha; }

      inline int getPenWidth(void) const { return width; }

      inline int getRadius(void) const { return radius; }

      inline bool getZoom(void) const { return zoom; }

      inline bool getFill(void) const { return fill; }

      inline bool getOutline(void) const { return outline; }

      inline int getDepth(void) const { return depth; }

      CShape &setShape(const std::string &shape);

    private:
      void init(const std::string &iPenColor, const std::string &iFillColor,
                const int iWidth, const int iRadius, const double penAlpha,
                const double fillAlpha, const bool iZoom, const bool iFill,
                const bool iOutline, int iDepth);
      void init(const SColor &iPenColor, const SColor &iFillColor,
                const int iWidth, const int iRadius, const double penAlpha,
                const double fillAlpha, const bool iZoom, const bool iFill,
                const bool iOutline, int iDepth);
   };

}} // end namespace crl::gui

std::ostream &operator<<(std::ostream &os, const crl::gui::CShape &shape);

/// ----------------------------------------------------------------------------
/// @brief operator>> read shape from string representation, each paramters is
///                   read as unique, parsing is end after all shape attributues
///                   has been parsed or a attribute is repeated
///
/// @param os
/// @param shape
///
/// @return
/// ----------------------------------------------------------------------------
std::istream &operator>>(std::istream &os,
                         crl::gui::CShape &shape) throw();

#endif

/* end of shape.h */
