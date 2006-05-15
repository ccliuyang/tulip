#include <tulip/MutableContainer.h>
#include <tulip/ForEach.h>
#include <tulip/Rectangle.h>
#include "SpringElectrical.h"

LAYOUTPLUGIN(SpringElectrical,"Spring_Electrical","David Auber","14/01/2002","Alpha","0","1");

using namespace std;
using namespace tlp;

inline double sqr(double x) {
  return (x*x);
}
//================================================================================
SpringElectrical::SpringElectrical(const PropertyContext &context):LayoutAlgorithm(context){
}
//================================================================================
Coord SpringElectrical::repulsiveForces(node u) {
  Coord uCoord = layoutResult->getNodeValue(u);
  Coord result(0,0,0);
  node v;
  forEach(v, graph->getNodes()) {
    if (u == v) continue;
    Coord uv = layoutResult->getNodeValue(v) - uCoord;
    double dist = uv.norm();
    if (dist > 1.0E-3) {
      uv /= dist;
      dist -= (1.0 + sizeNorm.get(u.id) + sizeNorm.get(v.id));
      //      if (dist > 5.0) continue;
      if (dist < 1.E-3)
	uv *= dist;//-(1.0 + sizeNorm.get(u.id) + sizeNorm.get(v.id));
      else
	uv *=  -sizeNorm.get(u.id) /sqr(dist);
    } else {
      uv[rand()%2] += 0.01 - (0.02 * (double(rand()%2)));
    }
    result += uv;
  }
  return result;
}
//================================================================================
Coord SpringElectrical::attractiveForces(node n) {
  node nu = n;
  Coord u = layoutResult->getNodeValue(nu);
  Coord result(0,0,0);
  node nv;
  forEach(nv, graph->getInOutNodes(n)) {
    Coord v = layoutResult->getNodeValue(nv);
    Coord uv = v-u;
    double length = 1. +  (sizeNorm.get(nu.id) + sizeNorm.get(nv.id));
    double dist = uv.norm();
    if (dist < length) {
      uv *= dist - length;
    }
    else {
      if (dist > 1e-3) {
	uv /= dist;
	uv *= pow((dist - length) , 2.0); //Hooke's law
	//	uv *= pow((dist - length) , 1.0); //Hooke's law
	//uv *= length/k * log( dist / length); //Eades's law
      }
      else
	uv[rand()%2] += 0.01 - (0.02 * (double(rand()%2)));
    }
    result += uv;
  }
  return result;
}
//================================================================================
Coord maxForce(Coord force, double max) {
  Coord result(force);
  float norm = result.norm();
  if (norm > max) {
    result /= norm;
    result *= max;
  }
  return result;
}

bool SpringElectrical::overlap(node u, Coord &move) {
  Coord pos = layoutResult->getNodeValue(u) + move;
  bool  overlap = false;
  node v;
  forEach(v, graph->getNodes()) {
    if (v==u) continue;
    Coord pos2 = layoutResult->getNodeValue(v);
    if ((pos - pos2).norm() < (sizeNorm.get(u.id) + sizeNorm.get(v.id)))
      overlap = true;
  }
  return overlap;
}
//================================================================================  
bool SpringElectrical::run() {
  
  int iterations = graph->numberOfNodes();
  prevMove.setAll(Coord(0,0,0));
  if (iterations < 500) iterations = 500;// iterations = iterations >? 500;

  inputSelection = graph->getProperty<BooleanProperty>("viewSelection");
  inputSize = graph->getProperty<SizeProperty>("viewSize");
  LayoutProperty *inputLayout = graph->getProperty<LayoutProperty>("viewLayout");
  
  node n;
  forEach(n, graph->getNodes()) {
    layoutResult->setNodeValue(n, inputLayout->getNodeValue(n));
    sizeNorm.set(n.id, inputSize->getNodeValue(n).norm()/2.0);
  }

  double maxforce   = inputSize->getMax().norm() / 2.0;
  double deltaForce = 1.0;
  
  for (int count = 0; count < iterations; ++count) {
    maxforce = ((layoutResult->getMax() - layoutResult->getMin()).norm() + inputSize->getMax().norm()) / 100;
    if (pluginProgress->progress(count,iterations)!=TLP_CONTINUE) break;
    node itn;
    forEach(itn, graph->getNodes()) {
      if (inputSelection->getNodeValue(itn)) continue;

      Coord newMove;
      if (count%3 == 0)
	newMove = maxForce(repulsiveForces(itn), maxforce); //prevent too big jumps
      else 
	newMove = maxForce(attractiveForces(itn) + repulsiveForces(itn), maxforce); //prevent too big jumps
      
      Coord move;
      if (count == 0 || count%3!=0) {
	move = newMove;
      } else {
	Coord prev = prevMove.get(itn.id);
	Coord cur  = newMove;
	cur  /= cur.norm();
	prev /= prev.norm();
	float r = prev.dotProduct(cur);
	if (r > 0) {
	  move = (prevMove.get(itn.id) + newMove) * (r / 2.0 );
	} else {
	  move  = newMove;
	  move /= move.norm();
	}
      }
      
      //test overlapping
      int i = 0;
      while(overlap(itn, move) && i<10) {
	move /= 4.0;
	++i;
      }
      if (overlap(itn, move)) {
	move = Coord(0,0,0);
	move[rand()%2] += 0.01 - (0.02 * (double(rand()%2)));
      }
      
      prevMove.set(itn.id, move);
      Coord tmp  = layoutResult->getNodeValue(itn);
      layoutResult->setNodeValue(itn, tmp + prevMove.get(itn.id));      
    }
  }
  return pluginProgress->state()!=TLP_CANCEL;
}
//================================================================================
bool intersect2D(const Coord &a, const Coord &b, const Coord &c, const Coord &d) {
  Coord AB = b - a;
  Coord AC = c - a;
  Coord AD = d - a;
  float ABAC = (AB^AC)[2];
  ABAC /= fabs(ABAC);
  float ABAD = (AB^AD)[2];
  ABAD /= fabs(ABAD);
  if (fabs(ABAC -  ABAD) < 1E-6 ) return false;
  Coord CD = d - c;
  Coord CA = a - c;
  Coord CB = b - c;  
  float CDCA = (CD^CA)[2];
  CDCA /= fabs(CDCA);
  float CDCB = (CD^CB)[2];
  CDCB /= fabs(CDCB);
  if (fabs(CDCA - CDCB) < 1E-6 ) return false;
  return true;
}
//================================================================================
bool SpringElectrical::checkEdgeIntersection(const node n, const Coord & move) {
  Coord a = layoutResult->getNodeValue(n);
  Coord b = a + move;
  Iterator<edge> *itE = graph->getEdges();
  while(itE->hasNext()) {
    edge e = itE->next();
    node src = graph->source(e);
    node tgt = graph->target(e);
    if (src == n || tgt == n) continue;
    Coord c = layoutResult->getNodeValue(src);
    Coord d = layoutResult->getNodeValue(tgt);
    if (intersect2D(a, b, c, d)) {
      delete itE;
      return true;
    }
  } delete itE;
  itE = graph->getInOutEdges(n);
  while(itE->hasNext()) {
    edge e = itE->next();
    node src = graph->source(e);
    node tgt = graph->target(e);
    Coord a = layoutResult->getNodeValue(src);
    Coord b = layoutResult->getNodeValue(tgt);
    Iterator<node> *itN = graph->getNodes();
    while(itN->hasNext()) {
      node currentNode = itN->next();
      if((currentNode == src) || (currentNode == tgt))
	continue;
      Coord c = layoutResult->getNodeValue(currentNode);
      Coord d = c - move;
      if(intersect2D(a, b, c, d)) {
	delete itE;
	delete itN;
	return true;
      }
    } delete itN;
    
  } delete itE;
  return false;
}
