/*
 * File name: manager.h
 * Date:      2007/03/05 22:03
 * Author:    Jan Faigl
 */

#ifndef __MANAGER_H__
#define __MANAGER_H__

#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <stdint.h>
#include <vector>

#include <crl/gui/renderer_object.h>
#include <crl/gui/shape.h>

namespace crl { namespace gui { namespace manager {

   typedef std::string Tag;
   typedef std::list<Tag> TagList;

   typedef uint64_t TID;
   typedef int TDepth;

   /// ----------------------------------------------------------------------------
   /// @brief Struct that encapsulate object in managers
   /// ----------------------------------------------------------------------------
   class CObject : public crl::gui::render::SRendererObject {
    public:
      typedef crl::gui::manager::TID TID;
      typedef crl::gui::manager::TagList TagList;

    protected:
      static TID maxID;
      TID id;
      TDepth depth;
      TagList tags;

    public:
      CObject(ObjectType type, std::string tags, const CShape &shape,
              TDepth depth = TDepth(0));
      virtual ~CObject() {}
      inline const TID &getID(void) const { return id; }
      inline const TDepth &getDepth(void) const { return depth; }

      TagList::iterator tags_begin(void) { return tags.begin(); }

      TagList::iterator tags_end(void) { return tags.end(); }

      /// ----------------------------------------------------------------------------
      /// @brief hasTag
      ///
      /// @param tag
      ///
      /// @return true if object has associated tag
      /// ----------------------------------------------------------------------------
      bool hasTag(std::string tag);

      //      virtual std::ostream & operator<<(std::ostream & out);
   };

   typedef std::list<CObject *> ObjectList;
   typedef std::map<TID, CObject *> ObjectMap;

   class CObjectExecutor {
    public:
      virtual ~CObjectExecutor() {}
      virtual void execute(CObject *object) = 0;
   };

   /// ----------------------------------------------------------------------------
   /// @brief
   /// ----------------------------------------------------------------------------
   class CIDTree {
    public:
      void add(CObject *object);
      void remove(const TID &id);
      CObject *first(void);
      void print(std::string prefix = "");

      int size(void) { return objectMap.size(); }

      ObjectMap::iterator find(const TID &id) { return objectMap.find(id); }

      void erase(const TID &id) { objectMap.erase(id); }

      ObjectMap::iterator objects_begin(void) { return objectMap.begin(); }

      ObjectMap::iterator objects_end(void) { return objectMap.end(); }

    private:
      ObjectMap objectMap;
   };

   /// ----------------------------------------------------------------------------
   /// @brief
   /// ----------------------------------------------------------------------------
   class CDepthTree {
    private:
      struct DepthLess {
         bool operator()(const TDepth &d1, const TDepth &d2) const
         {
            return d1 > d2;
         }
      };

    public:
      void add(CObject *object);
      void remove(const TDepth &depth, const TID &id);
      void print(void);
      void execute(CObjectExecutor *executor);
      int size(void) const { return depthMap.size(); }

    private:
      typedef std::map<TDepth, CIDTree *, DepthLess> DepthMap;
      typedef std::map<TDepth, ObjectList, DepthMap> DepthListMap;
      DepthMap depthMap;
      DepthListMap depthObjectList;
   };

   /// ----------------------------------------------------------------------------
   /// @brief  Object Manager
   /// ----------------------------------------------------------------------------
   class CManager {
    public:
      typedef CObject Object;
      typedef crl::gui::manager::ObjectList ObjectList;

      CManager();
      ~CManager();

      /// ----------------------------------------------------------------------------
      /// @brief add
      ///
      /// @param tags   several tags (space separated)
      /// @param object an object
      /// ----------------------------------------------------------------------------
      void add(CObject *object);

      /// ----------------------------------------------------------------------------
      /// @brief remove
      ///
      /// @param tags one tag as string
      /// ----------------------------------------------------------------------------
      void remove(std::string tags);

      /// ----------------------------------------------------------------------------
      /// @brief remove
      ///
      /// @param id
      /// ----------------------------------------------------------------------------
      void remove(Object::TID id);

      void print(void);

      void execute(CObjectExecutor *executor);

      void find(std::string tag, ObjectList &list);

    private:
      void removeByTag(const Tag &tag);
      void removeByTag(const Tag &tag, const TID &id);
      void removeAll(void);

    private:
      typedef std::map<Tag, CIDTree *> Tree;
      CDepthTree depthTree;
      Tree tree;
   };

}}} // namespace crl::gui::manager

#endif

/* end of manager.h */
