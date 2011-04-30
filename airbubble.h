#pragma once

#include <QGLViewer/vec.h>
#include <QGLViewer/manipulatedFrame.h>
#include "node.h"

class Airbubble : public Node 
{
	public:
		Airbubble() {}
		
		~Airbubble() {}
		
		// update positions and velocities of dynamic objects after a time step dt
		void animate(float dt);
		
		// Affichage openGL de l'objet
		//
		void draw() const;
		
		// Parse the XML File
		//
		void initFromDOMElement(const QDomElement& e);

		unsigned int addBubble(const qglviewer::Vec& bubblePos);

		unsigned int addBubble(const qglviewer::Vec& bubblePos, float radius);

		void deleteBubble(int id);

		void deleteObject(int id);

		void mergeBubble(int id1, int id2);

		bool intersects(int id1, int id2, float min_intersection);

		int t_nextbubble;

		float limit;
		int max_size;
		int min_size;
		int max_interval;
		int min_interval;
		float min_intersection;
		float speed_factor;
		int del_cyronix;



	private:
		//Bubbles
		std::vector<qglviewer::Vec> positions;
		std::vector<Object*> spheres;
		std::vector<double> radiuses;
		std::vector<double> speeds;
		std::vector<int> inactive;


	
};
