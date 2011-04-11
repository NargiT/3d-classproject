#pragma once

#include "object.h"

#include <QGLViewer/vec.h>

//
//
class Triangle : public Object
{
	public:
		// Contructors
		//
		Triangle(const qglviewer::Vec& a, const qglviewer::Vec& b, const qglviewer::Vec& c) ;
		Triangle(const qglviewer::Vec& a , const qglviewer::Vec& b , const qglviewer::Vec& c,
					const qglviewer::Vec& ta, const qglviewer::Vec& tb, const qglviewer::Vec& tc);

		// Affichage openGL de l'objet
		//
		virtual void draw() const;

		// Rayon d'une sphčre englobante (meme grossiere).
		//
		virtual float boundingRadius() const;

		// Methode principale d'intersection entre l'objet et un rayon
		//
		virtual bool intersect(const Ray&, Hit& hit, qglviewer::Frame frame) const;

		//
		//
		virtual void initFromDOMElement(const QDomElement& e);

	private:
		// Vertices of the triangle, normals and texture
		//
		qglviewer::Vec v[3], n[3], uv[3];
};
