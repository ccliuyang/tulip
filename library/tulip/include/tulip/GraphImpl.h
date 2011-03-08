/**
 *
 * This file is part of Tulip (www.tulip-software.org)
 *
 * Authors: David Auber and the Tulip development Team
 * from LaBRI, University of Bordeaux 1 and Inria Bordeaux - Sud Ouest
 *
 * Tulip is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation, either version 3
 * of the License, or (at your option) any later version.
 *
 * Tulip is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 */
#ifndef Tulip_SUPERGRAPHIMPL_H
#define Tulip_SUPERGRAPHIMPL_H

#ifndef DOXYGEN_NOTFOR_USER

#include <set>

#include <vector>
#include <list>
#include "tulip/GraphAbstract.h"
#include "tulip/Iterator.h"
#include "tulip/GraphStorage.h"
#include "tulip/IdManager.h"

namespace tlp {

class GraphView;
class GraphUpdatesRecorder;

///Implementation of the graph support of the Graph.
class TLP_SCOPE GraphImpl:public GraphAbstract, public Observer {

  friend class GraphUpdatesRecorder;
  friend class TLPExport;  

public:
  GraphImpl();
  ~GraphImpl();
  void clear();
  //=========================================================================
  bool isElement(const node ) const;
  bool isElement(const edge ) const;
  node addNode();
  void addNode(const node);
  edge addEdge(const node ,const node);
  void addEdge(const edge);
  void delNode(const node);
  void delEdge(const edge );
  void delAllNode(const node );
  void delAllEdge(const edge );
  void setEdgeOrder(const node,const std::vector<edge> & );
  void swapEdgeOrder(const node,const edge , const edge );
  //=========================================================================
  Iterator<node>* getNodes() const;
  Iterator<node>* getInNodes(const node )const;
  Iterator<node>* getOutNodes(const node )const;
  Iterator<node>* getInOutNodes(const node )const;
  Iterator<edge>* getEdges()const;
  Iterator<edge>* getInEdges(const node )const;
  Iterator<edge>* getOutEdges(const node )const;
  Iterator<edge>* getInOutEdges(const node )const;
  //================================================================================
  unsigned int deg(const node ) const;
  unsigned int indeg(const node) const;
  unsigned int outdeg(const node) const;
  //================================================================================
  virtual node source(const edge) const;
  virtual node target(const edge) const;
  virtual node opposite(const edge, const node n) const;
  virtual const std::pair<node, node>& ends(const edge e) const;
  virtual void setEnds(const edge, const node, const node);
  virtual void reverse(const edge);
  //================================================================================
  unsigned int numberOfEdges()const;
  unsigned int numberOfNodes()const;
  //================================================================================
  // updates management
  virtual void push(bool unpopAllowed = true);
  virtual void pop(bool unpopAllowed = true);
  virtual bool nextPopKeepPropertyUpdates(PropertyInterface* prop);
  virtual void unpop();
  virtual bool canPop();
  virtual bool canUnpop();
  virtual bool canPopThenUnpop();

  // observer interface
  void update(std::set<Observable *>::iterator begin ,std::set<Observable *>::iterator end);
  void observableDestroyed(Observable*);

  // for subgraph id management
  unsigned int getSubGraphId(unsigned int id);
  void freeSubGraphId(unsigned int id);

  // to improve memory allocation
  // attempt to reserve enough space to store nodes/edges
  virtual void reserveNodes(unsigned int nbNodes);
  virtual void reserveEdges(unsigned int nbEdges);

protected:
  // designed to reassign an id to a previously deleted elt
  // used by GraphUpdatesRecorder
  virtual node restoreNode(node);
  virtual edge restoreEdge(edge, node source, node target);
  // designed to only update own structures
  // used by GraphUpdatesRecorder
  virtual void removeNode(const node);
  virtual void removeEdge(const edge);
  // used by PropertyManager
  virtual bool canDeleteProperty(Graph* g, PropertyInterface *prop);

private :
  GraphStorage storage;
  IdManager graphIds;
  std::list<GraphUpdatesRecorder*> previousRecorders;
  std::list<Graph *> observedGraphs;
  std::list<PropertyInterface*> observedProps;
  std::list<GraphUpdatesRecorder*> recorders;

  void restoreAdj(node, std::vector<edge>&);
  void observeUpdates(Graph*);
  void unobserveUpdates();
  void delPreviousRecorders();
};

}
#endif

#endif
