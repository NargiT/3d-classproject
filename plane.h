#pragma once

#include "object.h"

#include <vector>

class Plane : public Object {
public:
    // Set default parameters. Radius is 1.0, position is 0,0,0.
    Plane();

    virtual ~Plane() {}

    // Accessors
    //
    // size of small part
    float smSize() const { return smSize_; }
    void setSmSize(float smSize) { smSize_ = smSize; }

    // size of plane
    float size() const { return size_; }
    void setSize(float size) { size_ = size; }



    // Draw using OpenGL
    //
    virtual void draw() const;


    // Does the ray r intersect the sphere
    //
    virtual bool intersect(const Ray&, Hit& hit, qglviewer::Frame frame) const;

    // Get the intersection radius information
    //
    virtual float boundingRadius() const {
      return size();
    }

    // Parse XML
    //
    virtual void initFromDOMElement(const QDomElement& e);

private:
    // Init elements after DOM callback
    //
    void init(float size, float smallSize);


    float smSize_;  // size of triangle, from which is plane created
    float size_;    //size of plain -> size_ X size_
};

