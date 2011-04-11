#ifndef CYLINDER_H
#define CYLINDER_H
#include "object.h"
using namespace std;
class Cylinder : public Object
{
public :
	// Set default parameters. Radii (both top and bottom) are 1.0, 
	//height is 1.0, position is 0,0,0. 
	Cylinder();
	virtual ~Cylinder() {};

	// Accessors 
	float bottomRadius() const { return bottomRadius_; }
	void setBottomRadius(float radius) { bottomRadius_ = radius; }
  
	float topRadius() const { return topRadius_; }
	void setTopRadius(float radius) { topRadius_ = radius; }
  
	// Set both top and bottom radii :
	void setRadius(float radius) {bottomRadius_ = radius; topRadius_ = radius;}
  
	float height() const { return height_; }
	void setHeight(float height) { height_ = height; }

	// Draw using OpenGL
	//
	virtual void draw() const;
  
	// Get the intersection radius information
	//
	virtual float boundingRadius() const { 
		float max = ( bottomRadius_> topRadius_) ? bottomRadius_ : topRadius_;
		max = (max < height_) ? height_ : max;
		return max;
	}

	// Does the ray r intersect the sphere
	//
	virtual bool intersect(const Ray&, Hit& hit, qglviewer::Frame frame) const;
	// Intersection with bottom/top discs :
	bool intersectDisc(const Ray& ray, Hit& hit, float radius, float z, qglviewer::Vec normal) const;
	// Computes normal at point pt :
	qglviewer::Vec computeNormal(qglviewer::Vec pt) const;

	// Parse XML
	//
	virtual void initFromDOMElement(const QDomElement& e);
  
private:
	float bottomRadius_; // radius of bottom disc
	float topRadius_; // radius of top disc
	float height_; // height of the cylinder
	// The position() of the frame is located at the center of bottom disc
};

#endif // CYLINDER_H
