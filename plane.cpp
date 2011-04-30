
#include "plane.h"
#include "material.h"

#include <GL/glut.h>
#include <cmath>
#include <float.h>

//#include <QDebug>

// Pour ne pas avoir a mettre qglviewer:: devant les classes
// definies dans le namespace qglviewer.
using namespace qglviewer;
using namespace std;

Plane::Plane()
{  
  init(20.0, 4.0);
}

void Plane::init(float size, float smSize)
{
  // Set the radius for openGL
  setSize(size);
  setSmSize(smSize);
}

void Plane::draw() const
{  
  // Apply the transform
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glMultMatrixd(frame_.matrix());

  // Color of the sphere
  glColor3fv(material().diffuseColor());


  // Draw the plane
  float step = smSize();
  float width = size();
  float xPos1, xPos2;

  // creating several stripes of triangles
  for(float stripPos=0.0; stripPos+step <= width; stripPos += step)  {
    glBegin(GL_TRIANGLE_STRIP);
      xPos1 = stripPos;
      xPos2 = stripPos + step;

      for(float yPos = 0.0; yPos <= width; yPos+=step )  {
        glVertex3f(xPos1, yPos, 0.0);
        glVertex3f(xPos2, yPos, 0.0);
      }
    glEnd();
  }

  // Go back to the father's frame
  glPopMatrix();
}

/* Does the ray intersect any of the triangles composing
 * the current sphere ?
 */
bool Plane::intersect(const Ray& ray, Hit& hit, Frame global_frame) const
{
  return false;
}

/* Parse the XML for elements of the Sphere
 */
void Plane::initFromDOMElement(const QDomElement& e)
{
  // init the object part
  Object::initFromDOMElement(e);

  // Parse for ntheta, nphi, radius
  float smSize = e.attribute("smallSize", "4.0").toFloat();
  float size = e.attribute("size", "20.0").toFloat();
  init(size, smSize);
}

