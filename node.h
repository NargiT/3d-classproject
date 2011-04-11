#pragma once

#include "object.h"

#include <list>

class Node : public Object
{
	public:
		// Constructor & Destructor
		//
		Node() ;
		~Node() ;

		// Add a leave to the current node
		// We force the node to be non modifiable
		//
		void addObject(Object* node) ;

		// Affichage openGL de l'objet
		//
		virtual void draw() const ;
		virtual void animate(float time);

		// Rayon d'une sphčre englobante (meme grossiere).
		//
		virtual float boundingRadius() const ;

		// Methode principale d'intersection entre l'objet et un rayon
		//
		virtual bool intersect(const Ray& ray, Hit& hit, qglviewer::Frame frame) const ;

		// Load from the XML
		//
		virtual void initFromDOMElement(const QDomElement& e) ;

	protected:
		// Data
		//
		std::list<Object*> _leaves;

		// Has the node been modified ?
		// Needed for the bounding radius caching
		//
		mutable bool is_constant;
} ;
