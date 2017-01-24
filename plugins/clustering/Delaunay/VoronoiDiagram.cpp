/**
 *
 * This file is part of Tulip (www.tulip-software.org)
 *
 * Authors: David Auber and the Tulip development Team
 * from LaBRI, University of Bordeaux
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

#include <tulip/Algorithm.h>
#include <tulip/TulipPluginHeaders.h>
#include <tulip/Delaunay.h>
#include <tulip/LayoutProperty.h>

using namespace std;

static bool voronoiDiagram(tlp::Graph *graph, bool voronoiCellsSubGraphs,
                           bool connectNodeToCellBorder, bool originalClone) {
  vector<tlp::node> nodes;
  vector<tlp::Coord> sites;
  tlp::node n;
  tlp::VoronoiDiagram voronoiDiag;

  tlp::LayoutProperty *layout = graph->getProperty<tlp::LayoutProperty>("viewLayout");

  nodes.reserve(graph->numberOfNodes());
  sites.reserve(graph->numberOfNodes());
  forEach(n, graph->getNodes()) {
    nodes.push_back(n);
    sites.push_back(layout->getNodeValue(n));
  }

  bool ret = tlp::voronoiDiagram(sites, voronoiDiag);

  if (ret) {
    tlp::Graph *voronoiSg = graph->addSubGraph("Voronoi");

    if (originalClone)
      graph->addCloneSubGraph("Original graph");

    TLP_HASH_MAP<unsigned int, tlp::node> voronoiVertexToNode;

    for (size_t i = 0 ; i < voronoiDiag.nbVertices() ; ++i) {
      tlp::node n = voronoiSg->addNode();
      layout->setNodeValue(n, voronoiDiag.vertex(i));
      voronoiVertexToNode[i] = n;
    }

    for (size_t i = 0 ; i < voronoiDiag.nbEdges() ; ++i) {
      voronoiSg->addEdge(voronoiVertexToNode[voronoiDiag.edge(i).first], voronoiVertexToNode[voronoiDiag.edge(i).second]);
    }

    if (voronoiCellsSubGraphs) {
      ostringstream oss;
      unsigned int cellCpt = 0;

      for (unsigned int i = 0 ; i < voronoiDiag.nbSites() ; ++i) {
        oss.str("");
        oss << "voronoi cell " << cellCpt++;
        set<tlp::node> nodesSet;

        for (set<unsigned int>::iterator it2 = voronoiDiag.voronoiCellForSite(i).begin() ; it2 != voronoiDiag.voronoiCellForSite(i).end() ; ++it2) {
          nodesSet.insert(voronoiVertexToNode[*it2]);
        }

        tlp::Graph *cellSg = voronoiSg->inducedSubGraph(nodesSet);
        cellSg->setName(oss.str());
      }
    }

    if (connectNodeToCellBorder) {
      for (unsigned int i = 0 ; i < voronoiDiag.nbSites() ; ++i) {
        voronoiSg->addNode(nodes[i]);

        for (set<unsigned int>::iterator it2 = voronoiDiag.voronoiCellForSite(i).begin() ; it2 != voronoiDiag.voronoiCellForSite(i).end() ; ++it2) {
          voronoiSg->addEdge(nodes[i], voronoiVertexToNode[*it2]);
        }
      }
    }
  }

  return ret;
}

static const char *paramHelp[] = {

  // voronoi cells
  "If true, a subgraph will be added for each computed voronoi cell.",

  // connect
  "If true, existing graph nodes will be connected to the vertices of their voronoi cell.",

  // original clone
  "If true, a clone subgraph named 'Original graph' will be first added."
};

class VoronoiDiagram : public tlp::Algorithm {

public :

  VoronoiDiagram(tlp::PluginContext *context) : tlp::Algorithm(context) {
    addInParameter<bool>("voronoi cells", paramHelp[0], "false");
    addInParameter<bool>("connect", paramHelp[1], "false");
    addInParameter<bool>("original clone", paramHelp[2], "true");
  }

  PLUGININFORMATION("Voronoi diagram", "Antoine Lambert", "",
                    "Performs a Voronoi decomposition, in considering the positions of the graph nodes as a set of points. These points define the seeds (or sites) of the voronoi cells. New nodes and edges are added to build the convex polygons defining the contours of these cells.","1.1","Triangulation")

  bool run() {
    tlp::Observable::holdObservers();

    bool voronoiCellSg = false;
    bool connectNodesToVoronoiCell = false;
    bool originalClone = true;

    if (dataSet) {
      dataSet->get("voronoi cells", voronoiCellSg);
      dataSet->get("connect", connectNodesToVoronoiCell);
      dataSet->get("original clone", originalClone);
    }

    bool ret = voronoiDiagram(graph, voronoiCellSg, connectNodesToVoronoiCell,
                              originalClone);

    tlp::Observable::unholdObservers();

    return ret;
  }
};

PLUGIN(VoronoiDiagram)
