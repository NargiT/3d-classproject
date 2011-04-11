#pragma once

#include "object.h"

#include <vector>

class Sphere : public Object
{
public :
  // Set default parameters. Radius is 1.0, position is 0,0,0. 
  Sphere();
  virtual ~Sphere() {};
  
  // Accessors
  //
  float radius() const { return radius_; }
  void setRadius(float radius) { radius_ = radius; }

  // Draw using OpenGL
  //
  virtual void draw() const;

  // Get the intersection radius information
  //
  virtual float boundingRadius() const { return radius(); }

  // Does the ray r intersect the sphere
  //
  virtual bool intersect(const Ray&, Hit& hit, qglviewer::Frame frame) const;

  // Parse XML
  //
  virtual void initFromDOMElement(const QDomElement& e);
  
private:
  // Init elements after DOM callback
  //
  void init(float radius, int ntheta, int nphi) ;

  // Radius of the sphere
  //
  float radius_;
};
