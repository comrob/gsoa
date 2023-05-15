/*
 * File name: dijkstra_lite.h
 * Date:      2008/07/30 10:38
 * Author:    Jan Faigl
 */

#ifndef __DIJKSTRA_LITE_H__
#define __DIJKSTRA_LITE_H__

#include <algorithm>
#include <iostream>

#include <fstream>
#include <string>
#include <vector>

#include <boost/foreach.hpp>

#include <crl/dijkstra_heap.h>

namespace crl {

   template <class T> class CDijkstraLite {
    public:
      typedef T Weight;
      CDijkstraLite() {}
      ~CDijkstraLite() {}

      void addEdge(int from, int to, Weight weight)
      {
         int max;
         max = from > to ? from : to;
         if (max >= nodes.size()) {
            nodes.resize(max + 1);
         }
         nodes[from].edges.push_back(SEdge(to, weight));
      }

      void addEdge(int from, int to, Weight weight, bool check)
      {
         bool already = false;
         int max;
         max = from > to ? from : to;
         if (max >= nodes.size()) {
            nodes.resize(max + 1);
         }
         if (check) {
            Edges &edges = nodes[from].edges;
            for (int i = 0; i < edges.size(); i++) {
               if (edges[i].to == to) {
                  already = true;
                  break;
               }
            }
         }
         if (!already) {
            nodes[from].edges.push_back(SEdge(to, weight));
         }
      }

      /// ----------------------------------------------------------------------------
      /// @brief is_valid
      ///
      /// @return true if no duplicities of edges has been found
      /// ----------------------------------------------------------------------------
      bool is_valid(void)
      {
         bool ret = true;
         int v = 0;
         while (v < nodes.size() && ret) {
            Edges &edges(nodes[v].edges);
            const int s = edges.size();
            int i = 0;
            while (i < s && ret) {
               int j = i + 1;
               while (j < s && ret) {
                  ret = edges[i].to != edges[j].to;
                  if (!ret) {
                     std::cout << "Duplicate edge from " << v << "->"
                               << edges[i].to << "i:" << i << " j:" << j
                               << std::endl;
                  }
                  j++;
               }
               i++;
            } // end first to
            v++;
         }
         return ret;
      }

      void load(std::string filename)
      {
         int from;
         int to;
         Weight weight;

         std::ifstream in(filename.c_str());
         while (in >> from >> to >> weight) {
            addEdge(from, to, weight);
         }
      }
      void save(std::string filename)
      {
         std::fstream out;
         out.open(filename.c_str(), std::ios::out);

         for (int i = 0; i < nodes.size(); i++) {
            out << i << " " << nodes[i].weight << " " << nodes[i].pred
                << std::endl;
         }
      }
      void solve() { solve(0); }

      /// ----------------------------------------------------------------------------
      /// @brief solve     start and goal node are part of path
      ///
      /// @param start
      /// @param goal
      /// @param weights
      /// @param path
      /// ----------------------------------------------------------------------------
      void solve(unsigned int start, unsigned int goal,
                 std::vector<Weight> &weights, std::vector<int> &path)
      {
         std::vector<int> pred;
         solve(start, pred, weights);
         int cur = goal;
         path.clear();
         if (pred[cur] != -1) { // path has been found
            do {
               path.push_back(cur);
               cur = pred[cur];
            } while (cur != start && pred[cur] != -1);
            if (cur == start) {       // path has been found
               path.push_back(start); // push the start
               std::reverse(path.begin(), path.end());
	    } else {
               path.clear(); // path has not been found
            }
         }
      }

      void solve(unsigned int start, unsigned int goal, Weight &weight,
                 std::vector<int> &path)
      {
         std::vector<int> pred;
         std::vector<Weight> weights;
         solve(start, goal, weights, path);
         if (path.size() > 0) {
            weight = weights[goal];
         }
      }

      void solve(unsigned int start, std::vector<int> &pred,
                 std::vector<Weight> &weight)
      {
         solve(start);
         pred.clear();
         weight.clear();
         pred.reserve(nodes.size());
         weight.reserve(nodes.size());
         for (int i = 0; i < nodes.size(); i++) {
            pred.push_back(nodes[i].pred);
            weight.push_back(nodes[i].weight);
         }
      }

      void solve(unsigned int start)
      {
         int node_idx;
         CHeap<Nodes, Weight> heap(nodes.size(), nodes);
         for (int i = 0; i < nodes.size(); i++) {
            nodes[i].weight = -1;
            nodes[i].pred = -1;
         }
         nodes[start].weight = 0;
         heap.add(start, 0);
         while ((node_idx = heap.getFirst()) != -1) {
            SNode &node = nodes[node_idx];

            BOOST_FOREACH (SEdge e, node.edges) {
               SNode &to = nodes[e.to];
               if (heap.getIDX(e.to) == -1) {
                  if (to.weight == -1) { // not  in close list
                     to.weight = node.weight + e.weight;
                     to.pred = node_idx;
                     heap.add(e.to, to.weight);
                  }
	       } else {
                  Weight c = node.weight + e.weight;
                  if (to.weight > c) {
                     to.weight = c;
                     to.pred = node_idx;
                     heap.update(e.to, to.weight);
                  }
               }
            } // end for loop
            heap.getIDX(node_idx) = -1;
         }
      }

      void clear(void) { nodes.clear(); }

      CDijkstraLite &operator=(const CDijkstraLite &d)
      {
         if (this != &d) {
            nodes = d.nodes;
         }
         return *this;
      }

    private:
      struct SEdge {
         int to;
         Weight weight;
         SEdge(int to, Weight weight) : to(to), weight(weight) {}
         SEdge &operator=(const SEdge &e)
         {
            if (this != &e) {
               to = e.to;
               weight = e.weight;
            }
            return *this;
         }
      };

      typedef std::vector<SEdge> Edges;

      struct SNode {
         Weight weight;
         int pred;
         Edges edges;
         SNode() : weight(0), pred(-1) {}

         SNode &operator=(const SNode &n)
         {
            if (this != &n) {
               weight = n.weight;
               pred = n.pred;
               edges = n.edges;
            }
            return *this;
         }
      };

      typedef std::vector<SNode> Nodes;
      Nodes nodes;
   };

} // namespace crl

#endif

/* end of dijkstra_lite.h */
