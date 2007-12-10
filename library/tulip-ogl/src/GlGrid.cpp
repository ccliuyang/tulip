#include "tulip/GlGrid.h"

#include <tulip/GlTools.h>

using namespace std;

namespace tlp {
  //============================================================
  GlGrid::GlGrid(const Coord& frontTopLeft, const Coord& backBottomRight, 
		 const Size& cell, const Color& color, bool displayDim[3]) :
    frontTopLeft(frontTopLeft), 
    backBottomRight(backBottomRight), 
    color(color), 
    cell(cell){
    for(unsigned int i=0; i < 3; i++)
      this->displayDim[i] = displayDim[i];
    boundingBox.check(frontTopLeft);
    boundingBox.check(backBottomRight);
  }
  //============================================================
  void GlGrid::draw(float lod) {
    Coord delta = backBottomRight - frontTopLeft;
    Coord gridSize = backBottomRight - frontTopLeft;
    delta /= gridSize / cell;
  
    glDisable(GL_COLOR_MATERIAL);
    setMaterial(color);
    //glColor4ubv(((const GLubyte *)&color));

    glLineWidth(1);
    glBegin(GL_LINES);
    //xy-plane
    if (displayDim[0]) {
      Coord A(frontTopLeft);
      while (A[0] <= backBottomRight[0] + 1E-3) {
	glVertex3f(A[0], A[1], A[2]);
	glVertex3f(A[0], A[1] + gridSize[1] , A[2]);
	A[0] += delta[0];
      }
      A = frontTopLeft;
      while (A[1] <= backBottomRight[1] + 1E-3) {
	glVertex3f(A[0], A[1], A[2]);
	glVertex3f(A[0] + gridSize[0], A[1]  , A[2]);
	A[1] += delta[1];
      }
    }
    //yz-plane
    if (displayDim[1]) {
      Coord A(frontTopLeft);
      while (A[2] <= backBottomRight[2] + 1E-3) {
	glVertex3f(A[0], A[1], A[2]);
	glVertex3f(A[0], A[1] + gridSize[1] , A[2]);
	A[2] += delta[2];
      }
      A = frontTopLeft;
      while (A[1] <= backBottomRight[1] + 1E-3) {
	glVertex3f(A[0], A[1], A[2]);
	glVertex3f(A[0], A[1] , A[2] + gridSize[2]);
	A[1] += delta[1];
      }
    }
    //xz-plane
    if (displayDim[2]) {
      Coord A(frontTopLeft);
      while (A[2] <= backBottomRight[2] + 1E-3) {
	glVertex3f(A[0], A[1], A[2]);
	glVertex3f(A[0]+ gridSize[0], A[1]  , A[2]);
	A[2] += delta[2];
      }
      A = frontTopLeft;
      while (A[0] <= backBottomRight[0] + 1E-3) {
	glVertex3f(A[0], A[1], A[2]);
	glVertex3f(A[0], A[1], A[2] + gridSize[2]);
	A[0] += delta[0];
      }
    }
    glEnd();
  }
  //============================================================
  void GlGrid::getDisplayDim(bool displayDim[3]) const {
    for(int i=0; i < 3; i++)
      displayDim[i] = this->displayDim[i];
  }
  //============================================================
  void GlGrid::setDisplayDim(bool displayDim[3]) {
    for(int i=0; i < 3; i++)
      this->displayDim[i] = displayDim[i];
  }
  //============================================================
}
