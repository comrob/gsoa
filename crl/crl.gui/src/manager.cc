/*
 * File name: manager.cc
 * Date:      2007/03/05 22:09
 * Author:    Jan Faigl
 */

#include <boost/tokenizer.hpp>

#include <crl/gui/manager.h>

using namespace crl::gui::manager;

typedef boost::tokenizer<boost::char_separator<char>> Tokenizer;
static boost::char_separator<char> separator(" ,");

/// ----------------------------------------------------------------------------
/// Class CObject
/// ----------------------------------------------------------------------------
TID crl::gui::manager::CObject::maxID = 0;

/// ----------------------------------------------------------------------------
CObject::CObject(ObjectType type, std::string tags, const CShape &shape,
                 TDepth depth)
    : crl::gui::render::SRendererObject(type, shape), id(maxID++), depth(depth)
{
   this->depth = shape.getDepth();
   Tokenizer tok(tags, separator);
   for (Tokenizer::iterator it = tok.begin(); it != tok.end(); it++) {
      this->tags.push_back(*it);
   }
}

/// ----------------------------------------------------------------------------
bool CObject::hasTag(std::string tag)
{
   // SPEEDUP TODO use hashmap
   bool ret = false;
   for (TagList::iterator it = tags.begin(); it != tags.end(); it++) {
      if (*it == tag) {
         ret = true;
         break;
      }
   }
   return ret;
}

/// ----------------------------------------------------------------------------
/// Class CIDTree
/// ----------------------------------------------------------------------------
void CIDTree::add(CObject *object) { objectMap[object->getID()] = object; }

/// ----------------------------------------------------------------------------
void CIDTree::remove(const TID &id)
{
   ObjectMap::iterator it = objectMap.find(id);
   if (it != objectMap.end()) {
      objectMap.erase(it);
   }
}

/// ----------------------------------------------------------------------------
CObject *CIDTree::first(void)
{
   return (objectMap.size() > 0) ? objectMap.begin()->second : 0;
}

/// ----------------------------------------------------------------------------
void CIDTree::print(std::string prefix)
{
   std::cout << "CIDTree size:" << size() << std::endl;
   for (ObjectMap::iterator it = objectMap.begin(); it != objectMap.end();
        it++) {
      std::cout << prefix << "id:" << it->first << " object: " << it->second
                << "  ";
      std::cout << std::endl;
   }
}

/// ----------------------------------------------------------------------------
/// Class CDepthTree
/// ----------------------------------------------------------------------------
void CDepthTree::add(CObject *object)
{
   TDepth depth = object->getDepth();
   DepthMap::iterator t = depthMap.find(depth);
   if (t == depthMap.end()) {
      CIDTree *tIDTree = new CIDTree();
      tIDTree->add(object);
      depthMap[depth] = tIDTree;
   } else {
      t->second->add(object);
   }
}

/// ----------------------------------------------------------------------------
void CDepthTree::remove(const TDepth &depth, const TID &id)
{
   DepthMap::iterator t = depthMap.find(depth);
   if (t != depthMap.end()) {
      // now find object
      CIDTree *idtree(t->second);
      idtree->remove(id);
      if (idtree->size() == 0) { // remove this depth
         delete t->second;
         depthMap.erase(t);
      }
   }
}

/// ----------------------------------------------------------------------------
void CDepthTree::print(void)
{
   std::cout << "DepthTree size:" << size() << std::endl;
   for (DepthMap::iterator it = depthMap.begin(); it != depthMap.end(); it++) {
      std::cout << "DepthTree depth:" << it->first << std::endl;
      it->second->print("\t");
   }
}

/// ----------------------------------------------------------------------------
void CDepthTree::execute(CObjectExecutor *executor)
{
   for (DepthMap::iterator it = depthMap.begin(); it != depthMap.end(); it++) {
      CIDTree *tIDTree(it->second);
      for (ObjectMap::iterator obj = tIDTree->objects_begin();
           obj != tIDTree->objects_end(); obj++) {
         executor->execute(obj->second);
      }
   }
}

/// ----------------------------------------------------------------------------
/// Class CManager
/// ----------------------------------------------------------------------------
CManager::CManager() {}

/// ----------------------------------------------------------------------------
CManager::~CManager() { removeAll(); }

/// ----------------------------------------------------------------------------
void CManager::add(CObject *object)
{
   depthTree.add(object);
   for (TagList::iterator it = object->tags_begin(); it != object->tags_end();
        it++) {
      Tree::iterator t = tree.find(*it);
      if (t == tree.end()) { // tag is not in tree
         CIDTree *tIDTree = new CIDTree();
         tIDTree->add(object);
         tree[*it] = tIDTree;
      } else { // tag already in tree
         t->second->add(object);
      }
   }
}

/// ----------------------------------------------------------------------------
void CManager::remove(std::string tags)
{
   Tokenizer tok(tags, separator);
   for (Tokenizer::iterator it = tok.begin(); it != tok.end(); it++) {
      removeByTag(*it);
   }
}

/// ----------------------------------------------------------------------------
void CManager::remove(Object::TID id)
{
   // go through all tags and try find id
   Tree::iterator it = tree.begin();
   CObject *obj = 0;
   while (it != tree.end()) {
      if (it->second->find(id) !=
          it->second->objects_end()) { // id is in the tree
         obj = it->second->find(id)->second;
         it->second->erase(id);         // delete object from tag tree
         if (it->second->size() == 0) { // tag is empty
            delete it->second;          // delete id tree for particular tree
            tree.erase(it);             // delete id tree
            it = tree.begin();
	 } else {
            it++;
         }
      } else {
         it++;
      }
   } // end while
   if (obj) {
      depthTree.remove(obj->getDepth(), obj->getID());
      delete obj;
   }
}

/// ----------------------------------------------------------------------------
void CManager::print(void)
{
   std::cout << "Tag Tree:" << std::endl;
   for (Tree::iterator it = tree.begin(); it != tree.end(); it++) {
      std::cout << it->first << ":" << std::endl;
      it->second->print("\t");
   }
   depthTree.print();
}

/// ----------------------------------------------------------------------------
void CManager::execute(CObjectExecutor *executor)
{
   depthTree.execute(executor);
}

/// ----------------------------------------------------------------------------
void CManager::find(std::string tag, ObjectList &list)
{
   Tokenizer tok(tag, separator);
   for (Tokenizer::iterator tag = tok.begin(); tag != tok.end(); tag++) {
      Tree::iterator objects = tree.find(*tag);
      if (objects != tree.end()) {
         for (ObjectMap::iterator it = objects->second->objects_begin();
              it != objects->second->objects_end(); it++) {
            list.push_back(it->second);
         }
      }
   }
}

/// ----------------------------------------------------------------------------
/// Private methods
/// ----------------------------------------------------------------------------
void CManager::removeByTag(const Tag &tag)
{
   if (tag == "all") {
      removeAll();
   } else {
      Tree::iterator it = tree.find(tag);
      if (it != tree.end()) { // tag already presented in tag tree
         // all objects (tids) must be removed in all tidtress
         CIDTree *tidree(it->second);
         tree.erase(it); // clear tag from tag tree
         CObject *obj;
         while ((obj = tidree->first()) != 0) {
            for (TagList::iterator t = obj->tags_begin(); t != obj->tags_end();
                 t++) {
               if (*t != tag) {
                  removeByTag(*t, obj->getID());
               }
            }
            tidree->remove(obj->getID()); // remove from TID;
            depthTree.remove(obj->getDepth(), obj->getID());
            delete obj; // delete obj
         }
         delete tidree;
      }
   }
}

/// ----------------------------------------------------------------------------
void CManager::removeByTag(const Tag &tag, const TID &id)
{
   Tree::iterator it = tree.find(tag);
   if (it != tree.end()) {
      it->second->remove(id);
      if (it->second->size() == 0) {
         delete it->second;
         tree.erase(it); // delete tag from tag tree
      }
   }
}

/// ----------------------------------------------------------------------------
void CManager::removeAll(void)
{
   TagList tags;
   for (Tree::iterator it = tree.begin(); it != tree.end(); it++) {
      tags.push_back(it->first);
   }
   for (TagList::iterator it = tags.begin(); it != tags.end(); it++) {
      remove(*it);
   }
}

/* end of manager.cc */
