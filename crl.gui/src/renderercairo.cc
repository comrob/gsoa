/*
 * File name: renderercairo.cc
 * Date:      2007/02/18 19:17
 * Author:    Jan Faigl
 */

#include <cairo-pdf.h>
#include <cairo-ps.h>
#include <cairo-svg.h>
#include <cairo.h>

#include <crl/gui/canvas_object_saver.h>
#include <crl/gui/renderer_object.h>

#include <crl/gui/renderercairo.h>

using namespace crl::gui::render;

using namespace crl::gui::cairo;

/// ----------------------------------------------------------------------------
/// Class CRendererCairo
/// ----------------------------------------------------------------------------
bool CRendererCairo::initialized = true;

/// ----------------------------------------------------------------------------
CRendererCairo::CRendererCairo(CCanvasContainer *iCanvas, CCairoWin *iWin)
    : canvas(iCanvas), win(iWin)
{
   transform.setView(win->getWidth(), win->getHeigth());
   transform.setViewBorder(10.0, 10.0);
   setArea(0, win->getWidth(), 0, win->getHeigth());
   exportWidth = win->getWidth();
   exportHeight = win->getHeigth();
   //  setExportDimension((int)win->getWidth(), (int)win->getHeigth());
   _obj = 0;
   state = NUMBER;
   antialiasing = true;
}

/// ----------------------------------------------------------------------------
CRendererCairo::CRendererCairo(CCanvasContainer *iCanvas, CCairoWin *iWin,
                               CConfig &cfg)
    : canvas(iCanvas), win(iWin)
{
   transform.setAspectRation(cfg.get<double>("gui-aspect-ratio"));
   transform.setView(win->getWidth(), win->getHeigth());
   transform.setViewBorder(cfg.get<int>("gui-border-x"),
                           cfg.get<int>("gui-border-y"));
   setArea(0, win->getWidth(), 0, win->getHeigth());
   antialiasing = cfg.get<bool>("gui-antialiasing");
   exportWidth = win->getWidth();
   exportHeight = win->getHeigth();
   //   setExportDimension(cfg.get<int>("gui-export-width"),
   //   cfg.get<int>("gui-export-height"));
   _obj = 0;
   state = NUMBER;
}

/// ----------------------------------------------------------------------------
CRendererCairo::~CRendererCairo()
{
   if (_obj) {
      delete _obj; // delete unclose object
   }
}

/// ----------------------------------------------------------------------------
void CRendererCairo::setDimension(int width, int height)
{
   WARN("setDimension is not supported");
}

/// ----------------------------------------------------------------------------
void CRendererCairo::setArea(double minX, double maxX, double minY, double maxY)
{
   transform.setReal(minX, maxX, minY, maxY);
}

/// ----------------------------------------------------------------------------
void CRendererCairo::canvas2Real(double iX, double iY, double *oX, double *oY)
{
   transform.view2real(iX, iY, *oX, *oY);
}

/// ----------------------------------------------------------------------------
void CRendererCairo::real2Canvas(double iX, double iY, double *oX, double *oY)
{
   transform.real2view(iX, iY, *oX, *oY);
}

/// ----------------------------------------------------------------------------
void CRendererCairo::redraw(void) { win->redraw(); }

/// ----------------------------------------------------------------------------
void CRendererCairo::drawPoint(double iX, double iY, const std::string &tags,
                               const CShape &shape)
{
   double x, y;
   real2Canvas(iX, iY, &x, &y);
   SCanvasObject *obj = new SCanvasObject(POINT, tags, shape);
   obj->addPoint(x, y);
   canvas->addObject(obj);
}

/// ----------------------------------------------------------------------------
void CRendererCairo::clearObjects(const std::string &tags)
{
   canvas->clearObjects(tags);
   win->invalidate(); // TODO a local invalidation should be helpfull to
                      // increase performance
}

/// ----------------------------------------------------------------------------
void CRendererCairo::setHandler(Handler *handler)
{
   //  RendererTK::set_handler(handler);
}

/// ----------------------------------------------------------------------------
bool CRendererCairo::isInitialized(void) { return initialized; }

/// ----------------------------------------------------------------------------
bool CRendererCairo::open(TObjectType objectType, const std::string &tags,
                          const CShape &shape)
{
   bool ret = false;
   if (state == NUMBER) {
      count = 0;
      switch (objectType) {
      case AREA:
         state = objectType;
         minx = std::numeric_limits<double>::max();
         maxx = -std::numeric_limits<double>::max();
         miny = std::numeric_limits<double>::max();
         maxy = -std::numeric_limits<double>::max();
         break;
      case POLYGON:
      case LINE:
      case ARC:
      case SEGMENT_SET:
         state = objectType;
         if (_obj != 0) {
            delete _obj;
         }
         _obj = new SCanvasObject(objectType, tags, shape);
         ret = true;
         break;
      default:
         WARN("Drawing object is not implemented type " << objectType);
         break;
      }
   } else {
      WARN("!Previous object was not closed");
   }
   return ret;
}

/// ----------------------------------------------------------------------------
bool CRendererCairo::close(void)
{
   bool ret = false;
   if (state != NUMBER) {
      switch (state) {
      case AREA:
         if (count > 0) {
            setArea(minx, maxx, miny, maxy);
         }
         ret = true;
         break;
      case POLYGON:
      case LINE:
      case ARC:
      case SEGMENT_SET:
         canvas->addObject(_obj);
         _obj = 0;
         ret = true;
         break;
      default:
         ERROR("Renderer is opened for not implemented object type " << state);
         break;
      }
   } else {
      WARN("No object is opened");
   }
   state = NUMBER;
   return ret;
}

/// ----------------------------------------------------------------------------
void CRendererCairo::reset(void)
{
   if (state != NUMBER) { // an object is opened
      if (_obj != 0) {
         delete _obj;
      }
      _obj = 0;
      state = NUMBER;
   }
}

/// ----------------------------------------------------------------------------
void CRendererCairo::addPoint(double x, double y)
{
   if (state != NUMBER) {
      switch (state) {
      case AREA:
         minx = (minx > x) ? x : minx;
         maxx = (maxx < x) ? x : maxx;
         miny = (miny > y) ? y : miny;
         maxy = (maxy < y) ? y : maxy;
         count++;
         break;
      case POLYGON:
      case LINE:
      case SEGMENT_SET: {
         double _x, _y;
         real2Canvas(x, y, &_x, &_y);
         _obj->addPoint(_x, _y);
      } break;
      case ARC: {
         const int N = _obj->getCoords().size();
         if (N == 0) { // center of the arc
            double _x, _y;
            real2Canvas(x, y, &_x, &_y);
            _obj->addPoint(_x, _y);
	 } else if (N == 1) { // radius
            double r1 = x * transform.getRatioX();
            double r2 = transform.getRatioY() / transform.getRatioX();
            _obj->addPoint(r1, r2);
	 } else { // angles do not transform
            _obj->addPoint(x, y);
         }
      } break;
      default:
         ERROR("Renderer is opened for not implemented object type " << state);
         break;
      } // end switch
   } else {
      WARN("No object is opened");
   }
}

/// ----------------------------------------------------------------------------
/*
void CRendererCairo::setExportDimension(int width, int height) {
   exportWidth = width;
   exportHeight = height;
}*/

/// ----------------------------------------------------------------------------
bool CRendererCairo::save(const std::string &filename)
{ // check creation
   bool ret = false;
   bool png = false;
   cairo_surface_t *surface = 0;
   if (filename.find(".pdf") != std::string::npos) {
      surface =
          cairo_pdf_surface_create(filename.c_str(), exportWidth, exportHeight);
   } else if (filename.find(".png") != std::string::npos) {
      surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, exportWidth,
                                           exportHeight);
      png = true;
   } else if (filename.find(".svg") != std::string::npos) {
      surface =
          cairo_svg_surface_create(filename.c_str(), exportWidth, exportHeight);
   } else if (filename.find(".eps") != std::string::npos) {
      surface =
          cairo_ps_surface_create(filename.c_str(), exportWidth, exportHeight);
   } else if (filename.find(".txt") != std::string::npos) {
      try {
         CView2Real tr(getTransform());
         CCanvasObjectSaver saver(filename, &tr);
         canvas->getManager().execute(&saver);
         ret = true;
      }
      catch (crl::exception &e) {
         ERROR("Exception " << e.what() << "!");
      }
      catch (std::exception &e) {
         ERROR("Runtime error " << e.what() << "!");
      }
   } else {
      ERROR("File suffix is not support, can not save canvas into '" +
            filename + "'");
   }
   if (surface) {
      cairo_t *cr = cairo_create(surface);
      cairo_set_tolerance(cr, 0.1);
      if (!antialiasing) {
         cairo_set_antialias(cr, CAIRO_ANTIALIAS_NONE);
      }
      canvas->drawBackground(cr, exportWidth, exportHeight, 1.0);
      canvas->draw(cr);
      cairo_show_page(cr);
      ret = cairo_status(cr) == CAIRO_STATUS_SUCCESS;
      cairo_destroy(cr);
      if (png) {
         ret = cairo_surface_write_to_png(surface, filename.c_str()) ==
               CAIRO_STATUS_SUCCESS;
      }
      cairo_surface_destroy(surface);
   }
   return ret;
}

/// ----------------------------------------------------------------------------
unsigned char getARGB2Gray(uint32_t data)
{
   unsigned char r = (data & 0xff0000) >> 16;
   unsigned char g = (data & 0x00ff00) >> 8;
   unsigned char b = (data & 0x0000ff) >> 0;
   return (.299 * r + .587 * g + .114 * b);
}

/// ----------------------------------------------------------------------------
unsigned char *CRendererCairo::getGrayscale(int &width, int &height)
{
   unsigned char *ret = 0;
   cairo_surface_t *surface = cairo_image_surface_create(
       CAIRO_FORMAT_ARGB32, exportWidth, exportHeight);
   cairo_t *cr = cairo_create(surface);
   cairo_set_antialias(cr, CAIRO_ANTIALIAS_NONE);
   cairo_set_tolerance(cr, 0.1);
   canvas->drawBackground(cr, exportWidth, exportHeight, 1.0);
   canvas->draw(cr);
   cairo_show_page(cr);
   assert_argument(cairo_status(cr) == CAIRO_STATUS_SUCCESS,
                   "CRendererCairo::getGrayscale draw fail");
   cairo_destroy(cr);
   width = cairo_image_surface_get_width(surface);
   height = cairo_image_surface_get_height(surface);
   int surfstride = cairo_image_surface_get_stride(surface);
   unsigned char *data = cairo_image_surface_get_data(surface);
   ret = new unsigned char[width * height];
   for (int y = 0; y < height; y++) {
      for (int x = 0; x < width; x++) {
         uint32_t *a = (uint32_t *)&data[x * 4 + y * surfstride];
         ret[x + y * width] = getARGB2Gray(*a);
      }
   }
   cairo_surface_destroy(surface);
   return ret;
}

/// ----------------------------------------------------------------------------
bool CRendererCairo::shutdownRequest(void) { return win->shutdownRequest(); }

/// ----------------------------------------------------------------------------
void CRendererCairo::getObjectsNames(double x, double y,
                                     std::list<std::string> &lst)
{
   canvas->getObjectsNames(x, y, lst);
}

/// ----------------------------------------------------------------------------
void CRendererCairo::getObjects(double x, double y,
                                Manager::ObjectList &objects)
{
   canvas->getObjects(x, y, objects);
}

/// ----------------------------------------------------------------------------
CRenderer::Manager &CRendererCairo::getManager(void)
{
   return canvas->getManager();
}

/// ----------------------------------------------------------------------------
void CRendererCairo::setBackgroundColor(const SColor &color)
{
   win->setBackgroundColor(color);
}

/// ----------------------------------------------------------------------------
const crl::gui::CViewTransform &CRendererCairo::getTransform(void)
{
   return transform;
}

/* end of renderercairo.cc */
