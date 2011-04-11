#ifndef _SPRING_H_
#define _SPRING_H_

#include <QGLViewer/vec.h>
#include <vector>

class Spring 
{
public:
	unsigned int obj1;  ///< index of the first object
	unsigned int obj2;  ///< index of the second object

	float stiffness;  // spring stiffness
	float initLength;  // equilibrium length
	float damping;  // damping factor	
	
	// Constructor with the object indices, striffness, equilibrium length and damping factor :
	Spring(unsigned int o1, unsigned int o2, float s, float l, float d);
	
	// Apply force on involved particles depending on the current positions and velocities :
        void addForce( std::vector<qglviewer::Vec>& forces,
		 const std::vector<qglviewer::Vec>& positions,
		 const std::vector<qglviewer::Vec>& velocities );	
};

#endif // _SPRING_H_
