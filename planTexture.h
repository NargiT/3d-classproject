#pragma once
#include "object.h"
#include <vector>

class PlanTexture : public Object {
public:

	//The image height and width MUST be a power of 2
	//The width and height must be at least 64 pixels and shouldn't be more than 256 pixels
	struct Image {
	    unsigned long sizeX;
	    unsigned long sizeY;
	    char *data;
	};
	typedef struct Image Image;


    // Set default parameters. Radius is 1.0, position is 0,0,0.
    PlanTexture();

    virtual ~PlanTexture() {
    };

    // Accessors

    float radius() const {
        return radius_;
    }

    void setRadius(float radius) {
        radius_ = radius;
    }

    // Draw using OpenGL
    virtual void draw() const;

    void drawPlan() const;

    // Get the intersection radius information
    virtual float boundingRadius() const {
        return radius();
    }

    // Parse XML
    virtual void initFromDOMElement(const QDomElement& e);

	virtual bool intersect(const Ray&, Hit& hit, qglviewer::Frame frame) const;

    void LoadGLTextures(char *filename) const;
    int ImageLoad(char *filename,Image *image) const;

private:
    // Init elements after DOM callback
    void init(float radius, int ntheta, int nphi);

    std::vector<qglviewer::Vec> positions;

    // Radius of the sphere
    float radius_;

    GLuint texture;
};
