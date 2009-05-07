#ifndef CURVES_H
#define CURVES_H
#ifndef DOXYGEN_NOTFOR_DEVEL
#include <tulip/Coord.h>
#include <tulip/Color.h>
#include "tulip/GlTools.h"
#include <vector>




namespace tlp {
  template<typename T, unsigned int N>
    class Matrix;
  TLP_GL_SCOPE void curveVisibility(const Coord &startPoint,const std::vector<Coord> &bends, const Coord &endPoint,
		       const Size &size, bool &polygon, bool &line, const Matrix<float,4> &projectionMatrix, const Matrix<float,4> &modelviewMatrix, const tlp::Vector<int, 4>&viewportArray);
  //====================================================================
  //return vertices, update startN and endN to prevent bad edge drawing
  TLP_GL_SCOPE std::vector<Coord> computeCleanVertices(const std::vector<Coord> &bends,
					  const Coord &startPoint, const Coord &endPoint,
					  Coord &startN, Coord &endN);
  TLP_GL_SCOPE void polyLine(const std::vector<Coord> &,/* polyline vertices */
		const Color &,             /* start color */
		const Color &);            /* end color */
  TLP_GL_SCOPE void polyLine(const std::vector<Coord> &, /* polyline vertices */
		const std::vector<Color> &);/* colors at polyline verts */
  TLP_GL_SCOPE void polyQuad(const std::vector<Coord> &, /* polyline vertces */
		const Color &,            /* start color */
		const Color &,            /* end color */
		const float,              /* start size */
		const float,              /* end size */
		const Coord &,            /* normal to the begin of the curve */
		const Coord &,            /* normal to the end curve */
		const std::string &textureName="");			/* textureName */
  TLP_GL_SCOPE void polyQuad(const std::vector<Coord> &, /* polyline vertces */
		const std::vector<Color> &, /* colors at polyline verts */
		const std::vector<float> &, /* size at polyline verts */
		const Coord &,            /* nomal to the begin of the curve */
		const Coord &,            /* nomal to the end curve */
		const std::string &textureName="");			/* textureName */
  TLP_GL_SCOPE void polyCylinder(const std::vector<Coord> &,    /* polyline vertces */
		    const std::vector<Color> &,    /* colors at polyline verts */
		    const std::vector<float> &,    /* size at polyline verts */
		    const Coord &,            /* nomal to the begin of the curve */
		    const Coord &);           /* nomal to the end curve */
  TLP_GL_SCOPE void polyCylinder(const std::vector<Coord> &,    /* polyline vertces */
		    const Color &,            /* start color */
		    const Color &,            /* end color */
		    const float,              /* start size */
		    const float,              /* end size */
		    const Coord &,            /* nomal to the begin of the curve */
		    const Coord &);           /* nomal to the end curve */
  //====================================================================
  TLP_GL_SCOPE void bezierCylinder(const std::vector<Coord> &,  /* polyline vertces */
		      const Color &,          /* start color */
		      const Color &,          /* end color */
		      const float,            /* start size */
		      const float,            /* end size */
		      const Coord &,          /* nomal to the begin of the curve */
		      const Coord &);         /* nomal to the end curve */
  TLP_GL_SCOPE void bezierQuad(const std::vector<Coord> &,  /* polyline vertces */
		  const Color &,          /* start color */
		  const Color &,          /* end color */
		  const float,            /* start size */
		  const float,            /* end size */
		  const Coord &,          /* nomal to the begin of the curve */
		  const Coord &);         /* nomal to the end curve */
  TLP_GL_SCOPE void bezierLine(const std::vector<Coord> &,  /* polyline vertces */
		  const Color &,          /* start color */
		  const Color &);          /* end color */
  //====================================================================
  TLP_GL_SCOPE void splineCylinder(const std::vector<Coord> &,  /* polyline vertces */
		      const Color &,          /* start color */
		      const Color &,          /* end color */
		      const float,            /* start size */
		      const float,            /* end size */
		      const Coord &,          /* nomal to the begin of the curve */
		      const Coord &);         /* nomal to the end curve */
  TLP_GL_SCOPE void splineQuad(const std::vector<Coord> &,  /* polyline vertces */
		  const Color &,          /* start color */
		  const Color &,          /* end color */
		  const float,            /* start size */
		  const float,            /* end size */
		  const Coord &,          /* nomal to the begin of the curve */
		  const Coord &);         /* nomal to the end curve */
  TLP_GL_SCOPE void splineLine(const std::vector<Coord> &,  /* polyline vertces */
		  const Color &,          /* start color */
		  const Color &);          /* end color */
  //====================================================================
  TLP_GL_SCOPE std::vector<Color> getColors(const std::vector<Coord> &line, const Color &c1, const Color &c2);
  TLP_GL_SCOPE std::vector<float> getSizes(const std::vector<Coord> &line, float s1, float s2);
  TLP_GL_SCOPE GLfloat* buildCurvePoints (const std::vector<Coord> &vertices,
      const std::vector<float> &sizes,
      const Coord &startN,
      const Coord &endN,
      unsigned int &resultSize,
      std::vector<unsigned int> *dec=NULL);
}
#endif //DOXYGEN_NOTFOR_DEVEL
#endif
