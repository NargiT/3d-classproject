#include "cylinder.h"
#include "material.h"
#include <qdom.h> 
//#include <QMessageBox>

#include <cmath>
#ifndef M_PI
	#define M_PI 3.14159265358979323846f
#endif
#ifndef M_PI_2
	# define M_PI_2 1.57079632679489661923
#endif

using namespace qglviewer;
using namespace std;

Cylinder::Cylinder()
{
	setBottomRadius(1.0);
	setTopRadius(1.0);
	setHeight(1.0);
}

void Cylinder::draw() const
{
	glMatrixMode(GL_MODELVIEW);
	// Save the current model view matrix
    glPushMatrix();
	{
		// Multiply matrix to get in the frame coordinate system.
		glMultMatrixd(frame().matrix());
		// Set color :
		glColor3fv(material().diffuseColor());
		// Draw trunk :
		gluCylinder(gluNewQuadric(), bottomRadius_, topRadius_, height_, 60, 60);
		// Draw discs :
		gluDisk(gluNewQuadric(), 0, bottomRadius_, 60, 60);
		glTranslated(0.0,0.0,height_);
		gluDisk(gluNewQuadric(), 0, topRadius_, 60, 60);
		glTranslated(0.0,0.0,-height_);
	}
	glPopMatrix();
}

void Cylinder::initFromDOMElement(const QDomElement& e)
{
	Object::initFromDOMElement(e);
	if (e.hasAttribute("radius")) {
		setRadius(e.attribute("radius").toFloat());
	}
	if (e.hasAttribute("bottomRadius")) {
		setBottomRadius(e.attribute("bottomRadius").toFloat());
	}
	if (e.hasAttribute("topRadius")) {
		setTopRadius(e.attribute("topRadius").toFloat());
	}
	if (e.hasAttribute("height")) {
		setHeight(e.attribute("height").toFloat());  
	}
}

bool Cylinder::intersectDisc(const Ray& ray, Hit& hit, float radius, float z, qglviewer::Vec normal) const {
	// Intersection with plane then test if within disc (x*x+y*y <= r*r)
	// Ray in object frame :
	Ray ray_fr;
	ray_fr.setStart(frame().coordinatesOf(ray.start()));
	ray_fr.setDirection(frame().transformOf(ray.direction()));
	// Intersection with plane : intersectionPoint.z = height : 
	// <=> start.z + sol*dir.z = height
	// <=> sol = (height - start.z)/dir.z
	float sol = (z-ray_fr.start().z)/ray.direction().z;
	Vec interPt = ray_fr.start() + sol*ray_fr.direction();
	// Belongs to disc?
	if (interPt.x*interPt.x + interPt.y*interPt.y <= radius*radius) {
		if (sol < hit.time() && sol>0.0001){
			// Closest intersection point so far => display it
			// Get back to world frame :
			hit.setTime(sol);
			hit.setIntersection(frame().inverseCoordinatesOf(interPt));
			hit.setMaterial(material_);
			hit.setNormal(frame().inverseTransformOf(normal));
			return true;
		}
	}
	return false;
}

qglviewer::Vec Cylinder::computeNormal(qglviewer::Vec pt) const {
	Vec normal;
	if (bottomRadius_ == topRadius_) { // cylinder
		// The normal is perpendicular to cylinder axis
		Vec pointOnAxis(0.0,0.0,pt.z); // projection of intersection point on axis
		normal = pt - pointOnAxis; 
		normal.normalize();
	} else { // truncated cone
		float x0 = pt.x, y0 = pt.y, z0 = pt.z;
		// Normals from partial derivatives of cartesian equation :
		if (bottomRadius_ > topRadius_) {
			normal = Vec(height_*height_*x0, height_*height_*y0,-(topRadius_-bottomRadius_)*(bottomRadius_*height_+(topRadius_-bottomRadius_)*z0));
		} else {
			normal = Vec(2*height_*height_*x0, 2*height_*height_*y0,2*(bottomRadius_-topRadius_)*(topRadius_*height_-(bottomRadius_-topRadius_)*(z0-height_)));
		}
		normal.normalize();
	}
	return normal;
}



bool Cylinder::intersect(const Ray& ray, Hit& hit, Frame current_frame) const
{
	// Update the father's frame to be the local -> global frame
	current_frame = MultiplyFrame(current_frame, frame());

     bool doesInter = false; // there is an intersection
	 // Ray in object frame :
     Ray ray_fr;
	 ray_fr.setStart(frame().coordinatesOf(ray.start()));
	 ray_fr.setDirection(frame().transformOf(ray.direction()));   
     // Short for start and direction of the ray :
     float ox=ray_fr.start()[0], oy=ray_fr.start()[1], oz=ray_fr.start()[2];
	 float dx=ray_fr.direction()[0], dy=ray_fr.direction()[1], dz=ray_fr.direction()[2];
     float a,b,c; // parameters used to compute discriminant/solution of polynomial equation
	 
     // Two cases : bottomRadius = topRadius (cylinder) et bottomRadius!=topRadius (truncated cone)
     
	 // Get the parameters used to solve quadratic polynomial
     if (bottomRadius_ == topRadius_) {
		 // Cylinder : bottomRadius = topRadius :
		 // equation is : x^2 + y^2 = r^2 <=> (ox+sol*dx)^2+(oy+sol*dy)^2=radius^2
         a = dx*dx + dy*dy;
         b = 2*(dx*ox + dy*oy);
         c = ox*ox + oy*oy - bottomRadius_*bottomRadius_;
     } else {
		 // Truncated cone :
		 // equation is : x^2 + y^2 = (z')^2*tan^2(alpha)
		 // <=> (ox+sol*dx)^2+(oy+sol*dy)^2 = (ozb+sol*dz)^2*tan^2(alpha) :
		 float maxRadius = (bottomRadius_ < topRadius_) ? topRadius_ : bottomRadius_;
		 float h = maxRadius*height_/fabs(bottomRadius_-topRadius_);
		 float tan_alpha = maxRadius/h; 
		 float ozb = (bottomRadius_ < topRadius_) ? oz  + h - height_ : oz - h;
		 float tan_alpha2 = tan_alpha*tan_alpha;
		 a = dx*dx + dy*dy - dz*dz*tan_alpha2;
		 b = 2*(dx*ox + dy*oy - dz*ozb*tan_alpha2);
		 c = ox*ox + oy*oy - ozb*ozb*tan_alpha2;
     }

	 // Solve 2nd order polynomial equation and keep solutions with z between 0 and height
	 float delta=b*b-4*a*c; // discriminant
	 if (delta>=0){ // at least 1 possible solution
		 float sol0=(-b-sqrt(delta))/(2*a); //solution of equation 
		 // 2nd solution : sol1 = (-b+sqrt(delta))/(2a) larger than sol0
		 Vec solu0(ox+sol0*dx,oy+sol0*dy,oz+sol0*dz); // corresponding intersection point
		 if (solu0.z <= height_ && solu0.z>=0) { // point between 0 and height
			 if (hit.time()>sol0 && sol0>0.0001){ // closest intersection point so far
				 Vec normal = computeNormal(solu0);
				 // We have our intersection point and its properties :
				 // Transform them into world frame :
				 hit.setTime(sol0);
				 hit.setMaterial(material_);
				 hit.setIntersection(current_frame.inverseCoordinatesOf(solu0));
				 hit.setNormal(current_frame.inverseTransformOf(normal));
				 doesInter = true;
			 }
		 }
	 }
	 
	 // There may be an intersection with the bottom or top discs closer :
	 doesInter = intersectDisc(ray,hit, bottomRadius_, 0, Vec(0,0,-1)) || doesInter;
	 doesInter = intersectDisc(ray,hit, topRadius_, height_, Vec(0,0,1)) || doesInter;
	 return doesInter;
}
