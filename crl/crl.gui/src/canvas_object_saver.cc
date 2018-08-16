/*
 * File name: canvas_object_saver.cc
 * Date:      2009/07/25 21:31
 * Author:    Jan Faigl
 */

#include <fstream>
#include <sstream>

#include <crl/bbox.h>

#include <crl/gui/renderer_object.h>
#include <crl/gui/canvas_object_saver.h>

using namespace crl::gui;

/// ----------------------------------------------------------------------------
/// Class CCanvasObjectSaver
/// ----------------------------------------------------------------------------
CCanvasObjectSaver::CCanvasObjectSaver(
    const std::string &filename, const render::CCoordsTransform *transform)
    : manager::CObjectExecutor(), out_(new std::ofstream(filename.c_str())),
      destroy(true), transform(transform)
{
   header();
}

/// ----------------------------------------------------------------------------
CCanvasObjectSaver::CCanvasObjectSaver(
    std::ostream &out, const render::CCoordsTransform *transform)
    : manager::CObjectExecutor(), out_(&out), destroy(false),
      transform(transform)
{
   header();
}

/// ----------------------------------------------------------------------------
CCanvasObjectSaver::~CCanvasObjectSaver()
{
   out_->flush();
   if (destroy && out_) {
      delete out_;
   }
}

/// ----------------------------------------------------------------------------
void CCanvasObjectSaver::execute(manager::CObject *object)
{
   static std::string prevShape;
   if (out_) {
      *out_ << "[OBJECT]" << std::endl;
      *out_ << "TAGS=";
      bool first = true;
      for (manager::CObject::TagList::iterator it = object->tags_begin();
           it != object->tags_end(); it++) {
         if (!first) {
            *out_ << ",";
         }
         else {
            first = false;
         }
         *out_ << *it;
      } // end tags
      *out_ << std::endl;
      std::stringstream ss;
      ss << object->shape;
      std::string strShape = ss.str();
      if (prevShape != strShape) {
         *out_ << "SHAPE=" << strShape << std::endl;
         prevShape = strShape;
      }
      if (transform) {
         object->simplePrint(*out_, *transform);
      }
      else {
         object->simplePrint(*out_);
      }
      *out_ << std::endl;
   }
}

/// - protected method ---------------------------------------------------------
void CCanvasObjectSaver::header(void)
{
   *out_ << "# CCanvasObjectSaver version 1.0" << std::endl;
   if (transform) {
      *out_ << "# CoordsTransform: " << transform->getName() << std::endl;
      *out_ << "# ViewTransform: " << transform->getTransformParam()
            << std::endl;
      crl::CBBox bbox;
      transform->getBBox(bbox);
      *out_ << "[OBJECT]" << std::endl;
      *out_ << "[AREA]" << std::endl;
      *out_ << bbox.minx() << " " << bbox.miny() << std::endl;
      *out_ << bbox.maxx() << " " << bbox.maxy() << std::endl;
      *out_ << std::endl;
   }
   else {
      *out_ << "# CoordsTransform: N/A" << std::endl;
      *out_ << "# ViewTransform: N/A" << std::endl;
   }
}

/* end of canvas_object_saver.cc */
