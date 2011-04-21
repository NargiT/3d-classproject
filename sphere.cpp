#include "sphere.h"
#include "material.h"

#include <GL/glut.h>
#include <cmath>
#include <float.h>

// Pour ne pas avoir a mettre qglviewer:: devant les classes
// definies dans le namespace qglviewer.
using namespace qglviewer;
using namespace std;

Sphere::Sphere() {
    setRadius(1.0f);
}

void Sphere::init(float r, int ntheta, int nphi) {
    // Set the radius for openGL
    setRadius(r);
}

void Sphere::draw() const {
    // Apply the transform
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    {
    glMultMatrixd(frame_.worldMatrix());

    // Color of the sphere
    glColor3fv(material().diffuseColor());

    // Draw the sphere
    //glutSolidSphere(radius_, 20, 20);
    gluSphere(gluNewQuadric(), radius_, 20, 20);
    }
    // Go back to the father's frame
    glPopMatrix();
}

/* Does the ray intersect any of the triangles composing
 * the current sphere ?
 */
bool Sphere::intersect(const Ray& ray, Hit& hit, Frame global_frame) const {
    // Update the local -> global transformation matrix
    global_frame = MultiplyFrame(global_frame, frame_);

    // Center of the sphere
    Vec C = frame_.position();

    float b = 2 * ray.direction() * (ray.start() - C);
    float c = C * C + ray.start() * ray.start() - 2 * C * ray.start() - radius_*radius_;
    float delta = b * b - 4 * c;

    if (delta >= 0.0f) {
        float t = 0.5 * (-b - sqrt(delta));
        Vec P = ray.start() + t * ray.direction();

        hit.setIntersection(global_frame.inverseCoordinatesOf(P));
        hit.setNormal(global_frame.inverseTransformOf((P - C) / (P - C).norm()));
        hit.setMaterial(this->material_);
        hit.setTime(t);
        return true;
    }
    return false;
}

/* Parse the XML for elements of the Sphere
 */
void Sphere::initFromDOMElement(const QDomElement& e) {
    // init the object part
    Object::initFromDOMElement(e);
    // Parse for ntheta, nphi, radius
    float radius = e.attribute("radius", "1.0").toFloat();
    int ntheta = e.attribute("ntheta", "10").toInt();
    int nphi = e.attribute("nphi", "10").toInt();
    init(radius, ntheta, nphi);
}

