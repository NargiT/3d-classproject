#include "triangle.h"

#include <GL/gl.h>

using namespace qglviewer;
using namespace std;

/* Standard constructor, without normal information
 */
Triangle::Triangle(const Vec& a, const Vec& b, const Vec& c)
{
	v[0] = a;
	v[1] = b;
	v[2] = c;

	Vec ab = (b-a); ab.normalize();
	Vec ac = (c-a); ac.normalize();
	n[0] = ab ^ ac;
	n[1] = n[0];
	n[2] = n[0];
}

/* Extended contructor using normals but not textures coords 
 */
Triangle::Triangle(	const Vec& a , const Vec& b , const Vec& c ,
							const Vec& ta, const Vec& tb, const Vec& tc
						)
{
	v[0] = a;
	v[1] = b;
	v[2] = c;

	n[0] = ta;
	n[1] = tb;
	n[2] = tc;
}

/* Affichage openGL de l'objet
 */
void Triangle::draw() const
{
	// Apply the transform
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMultMatrixd(frame_.matrix());

	glBegin(GL_TRIANGLES);
		glNormal3f(n[0][0], n[0][1], n[0][2]);
		glVertex3f(v[0][0], v[0][1], v[0][2]);

		glNormal3f(n[1][0], n[1][1], n[1][2]);
		glVertex3f(v[1][0], v[1][1], v[1][2]);

		glNormal3f(n[2][0], n[2][1], n[2][2]);
		glVertex3f(v[2][0], v[2][1], v[2][2]);
	glEnd();

	// Go back to the correct frame
	glPopMatrix();
}

/* Rayon d'une sphčre englobante (meme grossiere).
 */
float Triangle::boundingRadius() const
{
	static float radius = 0.0f;
	static bool cached = false;

	if(!cached)
	{
		radius = fmax(radius, v[0].norm());
		radius = fmax(radius, v[1].norm());
		radius = fmax(radius, v[2].norm());
		cached = true;
	}

	return radius;
}

/* Methode principale d'intersection entre l'objet et un rayon
 */
bool Triangle::intersect(const Ray& ray, Hit& hit, Frame global_frame) const
{
	global_frame = MultiplyFrame(global_frame, frame_);

	static Vec n;
	static bool cached_normal = false;

	Vec ab = (v[1]-v[0]); ab.normalize();
	Vec ac = (v[2]-v[0]); ac.normalize();

	// Compute the real normal of the triangle
	if(!cached_normal)
	{
		n = ab ^ ac;
		cached_normal = true;
	}

	// Compute the parameter t giving the intersection point
	float denom = n * ray.direction();
	if(denom == 0.0f)
	{
		return false;
	}
	float num = n * (v[0] - ray.start());

	float t = num / denom;
	Vec p = ray.start() + t*ray.direction();

	float pb = ((p-v[0]) * ab) / pow(ab.norm(), 2);	
	float pc = ((p-v[0]) * ac) / pow(ac.norm(), 2);
	if(pb >= 0.0f && pc >= 0.0f && pb + pc <= 1.0f)
	{
		hit.setTime(t);
		hit.setIntersection(global_frame.inverseCoordinatesOf(p));
		hit.setMaterial(material_);
		hit.setNormal(global_frame.inverseTransformOf(n));
		return true;
	}
	else
	{
		return false;
	}
}

/*
 */
void Triangle::initFromDOMElement(const QDomElement& e)
{
}
