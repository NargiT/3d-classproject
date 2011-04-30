#include <iostream>
#include <cmath> 

#include "airbubble.h"
#include "sphere.h"
#include "viewer.h"
#include "triangle.h"
#include "object.h"
#include <list>

#include "material.h"
#include <GL/glut.h>
#include <float.h>
#include <GL/gl.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifndef M_PI
// If PI is not defined
#define M_PI 3.14159265358979323846f
#endif

using namespace std;
using namespace qglviewer;

void Airbubble::initFromDOMElement(const QDomElement& e)
{
	// init the object part
	Object::initFromDOMElement(e);
	// initialize integration time step
	//printf("\nAirbubble InitFromDomElement\n");
	limit = e.attribute("limit", "7,5").toFloat();
	max_size = e.attribute("maxsize", "50").toFloat();
	min_size = e.attribute("minsize", "10").toFloat();
	min_interval = e.attribute("mininterval", "0.1").toFloat();
	max_interval = e.attribute("maxinterval", "0.1").toFloat();
	speed_factor = e.attribute("speedfactor", "1").toFloat();
	min_intersection = e.attribute("minintersection", "0.1").toFloat();
	
	srand ( time(NULL) );

	//security
	del_cyronix=0;

//	Vec position1 = Vec(0.0,0.0,0.0);
//	addBubble(position1, 0.25);
//	Vec position2 = Vec(0.0,7.0,0.0);
//	addBubble(position2, 0.4);

}

void Airbubble::draw() const
{
	//printf("\nAirbubble draw\n");
	Node::draw();

//	Sphere* s = new Sphere();
//	addObject(s);

	// Apply the transform
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	//glMultMatrixd(frame_.matrix());

	glLineWidth(15);
	glBegin(GL_LINES);
	glColor3f(0.0, 0.0, 1.0);

//	for (int i=0 ; i<nbBubbles ; i++){
//		Vec pos1 = positions[i];
//		glVertex3f(pos1.x, pos1.y, pos1.z);
//	}
	glEnd();
	//glMultMatrixd(frame_.matrix());
	//QGLViewer::drawAxis();

	// Go back to the father's frame
	glPopMatrix();
}

void Airbubble::animate(float t)
{
    Q_UNUSED(t);

	//printf("\nAirbubble Animate\n");
	unsigned int nbBubbles = (unsigned int) positions.size();
	std::vector<int> toErase;
	float speed;
	for(unsigned int i=0; i<nbBubbles; ++i ) {
		//printf("\nBubble-vorher %d, x=%f, y=%f, z=%f\n", i, positions[i].x, positions[i].y, positions[i].z);
		speed = speeds[i];
		//printf("Speed=%f", speed);
//		speed = 0.1;
		Vec up = Vec(0.0,speed, 0.0);
		positions[i]= positions[i] + up;
		//printf("\nBubble-nachher %d, x=%f, y=%f, z=%f\n", i, positions[i].x, positions[i].y, positions[i].z);
		Frame f;
		f.setPosition(positions[i]);
		spheres[i]->setFrame(f);
		if (f.position().y > frame_.position().y + limit) {
			//printf("\nBubble id=%d, z=%f overreached limit=%f\n", i, f.position().y, limit);
			toErase.push_back(i);
		}
	}

	//delete bubbles that overreached limit
	for (int i=toErase.size()-1; i>=0; i--) {
		//printf("\nDeleting Bubble, id=%d\n", toErase[i]);
		deleteBubble(toErase[i]);
	}

	//Check intersections
	nbBubbles = (unsigned int)positions.size();
	for (unsigned int i=0; i<nbBubbles; ++i) {
		for (unsigned int j=i+1; j<nbBubbles; ++j) {
			if (intersects(i,j, min_intersection)) {
				mergeBubble(i,j);
				nbBubbles--;
			}
		}
	}

	//Add new bubble
    if (t_nextbubble < t) {
    	//printf("\nCreate bubble\n");
    	Vec position = frame_.position();
    	addBubble(position);
    	t_nextbubble = t + rand() % (max_interval-min_interval) + min_interval;
    }
}

unsigned int Airbubble::addBubble(const Vec& bubblePos) {
	//printf("\nAddBubble\n");


	int radius = rand() % (max_size-min_size) + min_size;
	double rad = radius/100.f;
	return addBubble(bubblePos, rad);

}

unsigned int Airbubble::addBubble(const Vec& bubblePos, float radius) {
	Sphere* sphere = new Sphere();
    sphere->setRadius(radius);
    radiuses.push_back(radius);
    float tneraul = radius/3.f * speed_factor;
    speeds.push_back(tneraul);
	Frame f;
	f.setPosition(bubblePos);
	sphere->setFrame(f);
	sphere->setMaterial(material());
    addObject(sphere);
	spheres.push_back(sphere);
	positions.push_back(bubblePos);
	//printf("\nBubble is in position: %d\n ",positions.size()-1);

	return (unsigned int) positions.size()-1;
}

void Airbubble::deleteBubble(int id) {
	//printf("\nBegin-Delete\n");
	while (del_cyronix != 0) {
		//printf("\nSome Deletion already in process, waiting\n");
	}
	del_cyronix = 1;
	//printf("\nDelete Sphere\n");
	spheres.erase(spheres.begin() + id);
	//printf("\nDelete Radius\n");
	radiuses.erase(radiuses.begin() + id);
	//printf("\nDelete Speed\n");
	speeds.erase(speeds.begin() + id);
	//printf("\nDelete Position\n");
	positions.erase(positions.begin() + id);
	deleteObject(id);
	del_cyronix = 0;
	//printf("\nEnd-Delete\n");

}

void Airbubble::deleteObject(int id) {
	//printf("\nBegin-Delete-Object\n");
	std::list<Object*>::iterator it1;
	it1 = _leaves.begin();
	for (int i=0; i<id; i++) {
		it1++;
	}
	it1 = _leaves.erase (it1);
	//printf("\nEnd-Delete-Object\n");
}

bool Airbubble::intersects(int id1, int id2, float min_intersection) {
	Vec pos1 = positions[id1];
	Vec pos2 = positions[id2];
	float radius1 = radiuses[id1];
	float radius2 = radiuses[id2];
	float y1 = pos1.y;
	float y2 = pos2.y;

	if (y1 > y2) {
		if ((y1-radius1 - min_intersection) < (y2 + radius2)) {
			return true;
		} else {
			return false;
		}
	} else {
		if ((y2-radius2) < (y1 + radius1 - min_intersection)) {
			return true;
		} else {
			return false;
		}
	}
}

void Airbubble::mergeBubble(int id1, int id2) {
	Vec pos1 = positions[id1];
	Vec pos2 = positions[id2];
	float radius1 = radiuses[id1];
	float radius2 = radiuses[id2];

	if (id1 > id2) {
		deleteBubble(id1);
		deleteBubble(id2);
	} else {
		deleteBubble(id2);
		deleteBubble(id1);
	}

	float v1 = 4/3.f * radius1 * radius1 * radius1 * M_PI;
	float v2 = 4/3.f * radius2 * radius2 * radius2 * M_PI;
	float new_radius;
	if (radius1 > radius2) {

		float ratio = v2/v1;
		new_radius = radius1 * (1+ratio);
	} else {
		float ratio = v1/v2;
		new_radius = radius2 * (1+ratio);
	}

	Vec frame_pos = frame_.position();
	Vec new_pos = Vec (frame_pos.x, 0.5 * (pos1.y+pos2.y), frame_pos.z);

	addBubble(new_pos, new_radius);

}





