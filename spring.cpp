#include <cmath>

#include "spring.h"

using namespace std;
using namespace qglviewer;

Spring::Spring(unsigned int o1, unsigned int o2, float s, float l, float d) 
	: obj1(o1), obj2(o2), stiffness(s), initLength(l), damping(d) 
	{}

void Spring::addForce( vector<Vec>& forces, const vector<Vec>& pos, const vector<Vec>& vel)
{
	Vec u (pos[obj2].x - pos[obj1].x, pos[obj2].y - pos[obj1].y, pos[obj2].z - pos[obj1].z);
	
	float distance = sqrt( u.x*u.x + u.y*u.y + u.z*u.z);
	if ( distance < 1.0e-6 ) 
		return;

	u.x *= 1/distance;
	u.y *= 1/distance;
	u.z *= 1/distance;

	float projVelocity1 = vel[obj1].x*u.x + vel[obj1].y*u.y + vel[obj1].z*u.z;
	float projVelocity2 = vel[obj2].x*u.x + vel[obj2].y*u.y + vel[obj2].z*u.z;
	
	// force of a damped spring = spring force + damping force
	float fx = stiffness*(distance-initLength)*u.x + stiffness*damping*(projVelocity2 - projVelocity1) * u.x;
	float fy = stiffness*(distance-initLength)*u.y + stiffness*damping*(projVelocity2 - projVelocity1) * u.y;
	float fz = stiffness*(distance-initLength)*u.z + stiffness*damping*(projVelocity2 - projVelocity1) * u.z;
	
	forces[obj1].x += fx;
	forces[obj1].y += fy;
	forces[obj1].z += fz;
	forces[obj2].x -= fx;
	forces[obj2].y -= fy;
	forces[obj2].z -= fz;
}
