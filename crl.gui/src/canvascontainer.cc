/*
 * File name: canvascontainer.cc
 * Date:      2007/02/18 17:55
 * Author:    Jan Faigl
 */

#include <crl/logging.h>

#include <crl/gui/canvascontainer.h>
#include <crl/gui/markers.h>

using namespace crl::gui::cairo;

inline void setColor(const SColor &color, cairo_t *cr)
{
   cairo_set_source_rgba(cr, color.red, color.green, color.blue, color.alpha);
}

/// ----------------------------------------------------------------------------
/// Struct SCanvasObject
/// ----------------------------------------------------------------------------
SCanvasObject::SCanvasObject(ObjectType type, const std::string &tags,
                             const CShape &shape)
    : CObject(type, tags, shape), name(tags)
{
}

/// ----------------------------------------------------------------------------
SCanvasObject::~SCanvasObject() {}

/// ----------------------------------------------------------------------------
void SCanvasObject::draw(cairo_t *cr)
{
   cairo_save(cr);
   if (size == 1) {
      // TODO introduce zoom
      setColor(shape.getPenColor(), cr);
      crl::gui::cairo::markers::CCirclePoint::draw(cr, coords.begin()->x,
                                                   coords.begin()->y, shape);
   } else {
      if (size > 0) {
         switch (type) {
         case crl::gui::render::POLYGON: {
            cairo_new_path(cr);
            if (shape.getOutline()) {
               cairo_set_line_width(cr, shape.getPenWidth());
               setColor(shape.getPenColor(), cr);
	    } else {
               cairo_set_line_width(cr, 0);
               setColor(shape.getFillColor(), cr);
            }
            Coords::iterator it = coords.begin();
            cairo_move_to(cr, it->x, it->y);
            for (it++; it != coords.end(); it++) {
               cairo_line_to(cr, it->x, it->y);
            }
            cairo_close_path(cr);
            if (shape.getFill()) {
               cairo_save(cr);
               setColor(shape.getFillColor(), cr);
               cairo_fill_preserve(cr);
               cairo_restore(cr);
            }
            if (shape.getOutline()) {
               setColor(shape.getPenColor(), cr);
            }
            cairo_stroke(cr);
         } break;
         case crl::gui::render::LINE: {
            cairo_save(cr);
            cairo_set_line_width(cr, shape.getPenWidth());
            setColor(shape.getPenColor(), cr);
            Coords::iterator it = coords.begin();
            cairo_move_to(cr, it->x, it->y);
            for (it++; it != coords.end(); it++) {
               cairo_line_to(cr, it->x, it->y);
            }
            cairo_stroke(cr);
            cairo_restore(cr);
         } break;
         case crl::gui::render::ARC: {
            if (shape.getOutline()) {
               cairo_set_line_width(cr, shape.getPenWidth());
               setColor(shape.getPenColor(), cr);
	    } else {
               cairo_set_line_width(cr, 0);
               setColor(shape.getFillColor(), cr);
            }
            Coords::const_iterator it = coords.begin();
            const double x = it->x;
            const double y = it->y;
            it++;
            const double r1 = it->x;
            const double r2 = it->y;
            it++;
            const double a1 = it->x;
            const double a2 = it->y;
            cairo_save(cr);
            cairo_translate(cr, x, y);
            cairo_scale(cr, 1.0, r2);
            cairo_arc(cr, 0, 0, r1, a1, a2);
            if (shape.getFill()) {
               cairo_save(cr);
               setColor(shape.getFillColor(), cr);
               cairo_fill_preserve(cr);
               cairo_restore(cr);
            }
            if (shape.getOutline()) {
               setColor(shape.getPenColor(), cr);
            }
            cairo_stroke(cr);
            cairo_restore(cr);
         } break;
         case crl::gui::render::SEGMENT_SET: {
            cairo_save(cr);
            cairo_set_line_width(cr, shape.getPenWidth());
            setColor(shape.getPenColor(), cr);
            Coords::iterator it = coords.begin();
            for (; it != coords.end(); it++) {
               cairo_move_to(cr, it->x, it->y);
               it++;
               if (it != coords.end()) {
                  cairo_line_to(cr, it->x, it->y);
               }
            }
            cairo_stroke(cr);
            cairo_restore(cr);
         } break;
         default:
            ERROR("Unsupported object type:" << type);
            break;
         } // end switch
      }
   }
   cairo_restore(cr);
}

/// ----------------------------------------------------------------------------
bool SCanvasObject::in_stroke(cairo_t *cr, double x, double y)
{
   cairo_bool_t ret = 0;
   cairo_save(cr);
   if (size == 1) {
      // TODO introduce zoom
      setColor(shape.getPenColor(), cr);
      ret |= crl::gui::cairo::markers::CCirclePoint::in_stroke(
          cr, coords.begin()->x, coords.begin()->y, x, y, shape);
   } else {
      if (size > 0) {
         switch (type) {
         case crl::gui::render::POLYGON: {
            cairo_new_path(cr);
            if (shape.getOutline()) {
               cairo_set_line_width(cr, shape.getPenWidth());
               setColor(shape.getPenColor(), cr);
	    } else {
               cairo_set_line_width(cr, 0);
               setColor(shape.getFillColor(), cr);
            }
            Coords::iterator it = coords.begin();
            cairo_move_to(cr, it->x, it->y);
            for (it++; it != coords.end(); it++) {
               cairo_line_to(cr, it->x, it->y);
            }
            cairo_close_path(cr);
            if (shape.getFill()) {
               cairo_save(cr);
               setColor(shape.getFillColor(), cr);
               cairo_fill_preserve(cr);
               ret |= cairo_in_fill(cr, x, y);
               cairo_restore(cr);
            }
            if (shape.getOutline()) {
               setColor(shape.getPenColor(), cr);
            }
            ret |= cairo_in_stroke(cr, x, y);
            cairo_stroke(cr);
         } break;
         case crl::gui::render::LINE: {
            cairo_save(cr);
            cairo_set_line_width(cr, shape.getPenWidth());
            setColor(shape.getPenColor(), cr);
            Coords::iterator it = coords.begin();
            cairo_move_to(cr, it->x, it->y);
            for (it++; it != coords.end(); it++) {
               cairo_line_to(cr, it->x, it->y);
            }
            ret |= cairo_in_stroke(cr, x, y);
            cairo_stroke(cr);
            cairo_restore(cr);
         } break;

         case crl::gui::render::ARC: {
            cairo_save(cr);
            if (shape.getOutline()) {
               cairo_set_line_width(cr, shape.getPenWidth());
               setColor(shape.getPenColor(), cr);
	    } else {
               cairo_set_line_width(cr, 0);
               setColor(shape.getFillColor(), cr);
            }
            Coords::const_iterator it = coords.begin();
            const double x = it->x;
            const double y = it->y;
            it++;
            const double r1 = it->x;
            const double r2 = it->y;
            it++;
            const double a1 = it->x;
            const double a2 = it->y;
            cairo_save(cr);
            cairo_translate(cr, x, y);
            cairo_scale(cr, 1.0, r2);
            cairo_arc(cr, 0, 0, r1, a1, a2);
            if (shape.getFill()) {
               cairo_save(cr);
               setColor(shape.getFillColor(), cr);
               cairo_fill_preserve(cr);
               cairo_restore(cr);
            }
            if (shape.getOutline()) {
               setColor(shape.getPenColor(), cr);
            }
            cairo_restore(cr);
            ret |= cairo_in_stroke(cr, x, y);
            cairo_stroke(cr);
            cairo_restore(cr);
         } break;
         case crl::gui::render::SEGMENT_SET: {
            cairo_save(cr);
            cairo_set_line_width(cr, shape.getPenWidth());
            setColor(shape.getPenColor(), cr);
            Coords::iterator it = coords.begin();
            for (; it != coords.end(); it++) {
               cairo_move_to(cr, it->x, it->y);
               it++;
               if (it != coords.end()) {
                  cairo_line_to(cr, it->x, it->y);
               }
            }
            ret |= cairo_in_stroke(cr, x, y);
            cairo_stroke(cr);
            cairo_restore(cr);
         } break;

         default:
            ERROR("Unsupported object type");
            break;
         }
      }
   }
   cairo_restore(cr);
   return ret != 0;
}

/// ----------------------------------------------------------------------------
/// CCanvasDrawer
/// ----------------------------------------------------------------------------
CCanvasDrawer::CCanvasDrawer(cairo_t *iCr) : cr(iCr) {}

/// ----------------------------------------------------------------------------
void CCanvasDrawer::setCairo(cairo_t *iCr) { cr = iCr; }

/// ----------------------------------------------------------------------------
void CCanvasDrawer::execute(crl::gui::manager::CObject *obj)
{
   SCanvasObject *o = dynamic_cast<SCanvasObject *>(obj);
   if (o) {
      o->draw(cr);
   }
}

/// ----------------------------------------------------------------------------
/// Class CCanvasObjectNameFinder
/// ----------------------------------------------------------------------------
class CCanvasObjectNameFinder : public crl::gui::manager::CObjectExecutor {
   cairo_t *cr;
   double x;
   double y;
   std::list<std::string> &lst;

 public:
   CCanvasObjectNameFinder(cairo_t *cr, double x, double y,
                           std::list<std::string> &lst)
       : cr(cr), x(x), y(y), lst(lst)
   {
   }
   void execute(crl::gui::manager::CObject *obj)
   {
      SCanvasObject *o = dynamic_cast<SCanvasObject *>(obj);
      if (o && o->in_stroke(cr, x, y)) {
         lst.push_back(o->name);
      }
   }
};

/// ----------------------------------------------------------------------------
/// Class CCanvasObjectFinder
/// ----------------------------------------------------------------------------
class CCanvasObjectFinder : public crl::gui::manager::CObjectExecutor {
   cairo_t *cr;
   double x;
   double y;
   crl::gui::manager::CManager::ObjectList &lst;

 public:
   CCanvasObjectFinder(cairo_t *cr, double x, double y,
                       crl::gui::manager::CManager::ObjectList &lst)
       : cr(cr), x(x), y(y), lst(lst)
   {
   }
   void execute(crl::gui::manager::CObject *obj)
   {
      SCanvasObject *o = dynamic_cast<SCanvasObject *>(obj);
      if (o && o->in_stroke(cr, x, y)) {
         lst.push_back(o);
      }
   }
};

/// ----------------------------------------------------------------------------
/// Class CCanvasContainer
/// ----------------------------------------------------------------------------
CCanvasContainer::CCanvasContainer()
{
   backgroundColor.set(1.0, 1.0, 1.0, 1.0);
   bs = 0; // disable background surface
   drawer = 0;
   cr = 0;
}

/// ----------------------------------------------------------------------------
CCanvasContainer::~CCanvasContainer()
{
   if (drawer) {
      delete drawer;
   }
   if (bs) {
      cairo_surface_destroy(bs);
   }
}

/// ----------------------------------------------------------------------------
void CCanvasContainer::draw(cairo_t *cr)
{
   ScopedLock lk(mtx);
   if (drawer == 0) {
      drawer = new CCanvasDrawer(cr);
   } else {
      drawer->setCairo(cr);
   }
   this->cr = cr;
   manager.execute(drawer);
}

/// ----------------------------------------------------------------------------
void CCanvasContainer::drawBackground(cairo_t *cr, int width, int height,
                                      double zoom)
{
   cairo_save(cr);
   if (not bs) {
      cairo_set_source_rgba(cr, backgroundColor.red, backgroundColor.green,
                            backgroundColor.blue, backgroundColor.alpha);
   } else {
      cairo_set_source_surface(cr, bs, 1, 1);
      // cairo_rectangle(cr, 0, 0, width, height); cairo_clip(cr);
   }
   cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
   cairo_paint(cr);
   cairo_restore(cr);
}

/// ----------------------------------------------------------------------------
void CCanvasContainer::setBackgroundColor(const SColor &color, double alpha)
{
   backgroundColor = color;
   backgroundColor.alpha = alpha;
}

/// ----------------------------------------------------------------------------
void CCanvasContainer::setBackgroundImage(const std::string &filename)
{
   if (filename.empty()) {
      return;
   }
   if (bs) {
      cairo_surface_destroy(bs);
   }
   bs = cairo_image_surface_create_from_png(filename.c_str());
   if (not bs) {
      WARN("Cannot load background image from '" << filename << "'");
   }
}

/// ----------------------------------------------------------------------------
void CCanvasContainer::addObject(SCanvasObject *object)
{
   ScopedLock lk(mtx);
   manager.add(object);
}

/// ----------------------------------------------------------------------------
void CCanvasContainer::clearObjects(const std::string &tags)
{
   ScopedLock lk(mtx);
   manager.remove(tags);
}

/// ----------------------------------------------------------------------------
void CCanvasContainer::getObjectsNames(double x, double y,
                                       std::list<std::string> &lst)
{
   ScopedLock lk(mtx);
   if (cr) {
      CCanvasObjectNameFinder finder(cr, x, y, lst);
      manager.execute(&finder);
   } else { // TODO cairo contex
      ERROR("CCanvasContainer missing cairo context");
   }
}

/// ----------------------------------------------------------------------------
void CCanvasContainer::getObjects(double x, double y,
                                  Manager::ObjectList &objects)
{
   ScopedLock lk(mtx);
   if (cr) {
      CCanvasObjectFinder finder(cr, x, y, objects);
      manager.execute(&finder);
   } else { // TODO cairo contex
      ERROR("CCanvasContainer missing cairo context");
   }
}

/* end of canvascontainer.cc */
