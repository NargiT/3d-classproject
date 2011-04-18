#include <iostream>
#include <cmath> 

#include "dynamics.h"
#include "sphere.h"
#include "viewer.h"
#include "triangle.h"

using namespace std;
using namespace qglviewer;

// some constants that can be used for springs
float stiffness = 30.0f;
float initLength = 0.2f;
float damping = 0.2f;
float viscosity = 0.1f;


// Affichage openGL de l'objet
//
void Dynamics::draw() const
{
	Node::draw();

	// Apply the transform
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMultMatrixd(frame_.matrix());

	// draw spring elements
	int nbSprings = getNbSprings();
	glLineWidth(15);
	glBegin(GL_LINES);
	glColor3f(1.0, 0.0, 0.0);
	for (int i=0 ; i<nbSprings ; i++){
		int obj1 = getSpring(i).obj1;
		int obj2 = getSpring(i).obj2;
		Vec pos1 = getBallPos(obj1);
		Vec pos2 = getBallPos(obj2);
		glVertex3f(pos1.x, pos1.y, pos1.z);
		glVertex3f(pos2.x, pos2.y, pos2.z);
	}
	glEnd();
	glMultMatrixd(_manipulated_frame.matrix());
	QGLViewer::drawAxis();

	// Go back to the father's frame
	glPopMatrix();
}

// Parse the XML File
//
void Dynamics::initFromDOMElement(const QDomElement& e)
{
	// init the object part
	Object::initFromDOMElement(e);

	// initialize integration time step
	dt = e.attribute("dt", "0.1").toFloat();

	// Parse for Ground position
	groundPosition = Vec(e.attribute("pos_x", "0.0").toFloat(), e.attribute("pos_y", "0.0").toFloat(), e.attribute("pos_z", "-4.0").toFloat());

	// default material
	Material mat;
	mat.setDiffuseColor(Color(0.9,0.1,0.1));
	setMaterial(mat);

	Vec initPos (0.0, 0.0, 0.0);
	float mass = 1.0f;
	float radius = 0.25f;
	
	// Create the manipulated ball: a special ball controllable with the mouse
	unsigned int ball1 = addBall(initPos, Vec(), 0.0, radius);

	// modify the added ball's drawing sphere to be manipulated
	// with the mouse
	Frame f=drawing_sphere[0]->frame();
	f.setReferenceFrame(&_manipulated_frame);
	Viewer::getViewer()->setManipulatedFrame(&_manipulated_frame);
	drawing_sphere[0]->setFrame(f);
	
	// add a second ball
	Vec position  = initPos + Vec(0.0f, -2.0f*radius, 0.0f);
	Vec position2  = position + Vec(0.0f, -2.0f*radius, 0.0f);
	unsigned int ball2 = addBall(position, Vec(0.0f, 1.0f, 0.0f), mass, radius);
	//unsigned int ball3 = addBall(position2, Vec(0.0f, 0.5f, 1.0f), mass, radius);

	// add 2 triangles to represent the ground plane (for collisions)
	Vec a = groundPosition + Vec(-10.0f, 10.0f, 0.0f);
	Vec b = groundPosition + Vec( 10.0f, 10.0f, 0.0f);
	Vec c = groundPosition + Vec( 10.0f, -10.0f, 0.0f);
	Vec d = groundPosition + Vec(-10.0f, -10.0f, 0.0f);

	Triangle *t1=new Triangle(a,b,d);
	t1->setMaterial(mat);
	Triangle *t2=new Triangle(b,c,d);
	t2->setMaterial(mat);

	addObject(t1); addObject(t2);
  
  	addSpring(0, 1, stiffness, initLength, damping);
}

/////////////////////////
// accessors
/////////////////////////

unsigned int Dynamics::addBall(const Vec& ballPos, const Vec& velocity, float mass, float radius)
{
        // add a sphere in Node List as drawing primitive
        Sphere* sphere = new Sphere();
	sphere->setRadius(radius);
	Frame f;
	f.setPosition(ballPos);
	sphere->setFrame(f);
	sphere->setMaterial(material());
        addObject(sphere);
	drawing_sphere.push_back(sphere);

	positions.push_back(ballPos);
	velocities.push_back(velocity);
	masses.push_back(mass);
	if (mass>0) {
		invMasses.push_back( 1/mass);
	} else {
		invMasses.push_back(0.0);
	}
	radiuses.push_back(radius);

	return (unsigned int) positions.size()-1;
}

unsigned int Dynamics::addSpring(unsigned int ball1, unsigned  int ball2, float stiffness, float initLength, float dampingFactor)
{
	springs.push_back(Spring(ball1, ball2, stiffness, initLength, dampingFactor));
	return (unsigned int) springs.size();
}

void Dynamics::setGravity(const Vec& grav)
{
	gravity = grav;
}


/////////////////////////
// methods
/////////////////////////

void Dynamics::animate(float t)
{

        // for this class, use the dt in the configuration file as
        // integration step (fixed)
        Q_UNUSED(t);

	////////////////////
	// Force computations
	
	unsigned int nbBalls = (unsigned int) positions.size();
	vector<Vec> forces(nbBalls);
	for(unsigned int i=0; i<nbBalls; ++i ) {
			// poids
			forces[i] = forces[i] + masses[i] * gravity;
	}
	
	for(unsigned int i=0; i<springs.size(); ++i ) {
			// ressorts amortis
			springs[i].addForce(forces, positions, velocities);
	}
	
	///////////////////////////
	// Acceleration computations :
	
	for(unsigned int i=0; i<nbBalls; ++i ) {
			// accelerations are stored in the forces
			forces[i] = invMasses[i] * forces[i];
	}

	///////////////////////////
	// TODO: question 1: compute velocities

	for(unsigned int i=0; i<nbBalls; ++i ) {
			// accelerations are stored in the forces
			velocities[i] = velocities[i] + dt * forces[i];
	}

	///////////////////////
	// Integration scheme :

	// first ball is updated from manipulated frame
	positions[0]=_leaves.front()->frame().position();

	// other balls follow the Newton law:
	for(unsigned int i=1; i<nbBalls; ++i ) {
			positions[i] = positions[i] + dt * velocities[i];

			// update corresponding drawing sphere positions
			Frame f;
			f.setPosition(positions[i]);
			drawing_sphere[i]->setFrame(f);
	}

	////////////////////////////
	// Collisions :
	//
	for(unsigned int i=0; i<nbBalls; ++i ){
//		// avec le sol
		collisionBallPlane(positions[i], velocities[i], radiuses[i], invMasses[i], groundPosition, groundVelocity, groundNormal, 0.0f, 0.5f);
		//collisionBallBall(Vec& x1, Vec& v1, float r1, float invm1,Vec& x2, Vec& v2, float r2, float invm2,float rebound )
		//collisionBallBall(x1,v1,r1,invm1,x2,v2,r2,invm2,rebound )
		for(unsigned int j=i+1; j<nbBalls; ++j ){
			//collisionBallBall(positions[i], velocities[i], radiuses[i], invMasses[i], groundPosition, groundVelocity, 0.1f, 0.0f, 0.5f);
			collisionBallBall(positions[i], velocities[i], radiuses[i], invMasses[i], positions[j], velocities[j], radiuses[j], invMasses[j], 0.5f);
		}
	}

}



void Dynamics::collisionBallPlane(Vec& x1, Vec& v1, float r1, float invm1,
                                  Vec& x2, Vec& v2, Vec& normal, float invm2, float rebound )
{
	// don't process fixed objects :
	if( invm1==0 && invm2==0 ) 
		return;

	// Plane-ball distance :
	Vec gap = x1 - x2;
	float penetration = (gap * normal) - r1;
	if( penetration > 0 ) 
		return;
	
	// Penetration velocity :
	float vpen = (v1-v2)*normal;
	
	// Correction distribution (the heaviest object moves less) :
	float corr1 = invm1 / (invm1+invm2);
	float corr2 = invm2 / (invm1+invm2);
	
	// rebound :
	corr1 *= (1+rebound);
	corr2 *= (1+rebound);
	
	// position and velocity updates :
	x1 = x1 - (corr1 * penetration) * normal;
	v1 = v1 - (corr1 * vpen) * normal;
	x2 = x2 + (corr2 * penetration) * normal;
	v2 = v2 + (corr2 * vpen) * normal;		
}


void Dynamics::collisionBallBall(Vec& x1, Vec& v1, float r1, float invm1,
                                 Vec& x2, Vec& v2, float r2, float invm2,
                                 float rebound )
{

	// don't process fixed objects :
	if( invm1==0 && invm2==0 ) 
		return;

	// Plane-ball distance :
	//Vec gap = x1 - x2;
	Vec gap = x2 -x1;
	float distance=gap.x*gap.x+gap.y*gap.y+gap.z*gap.z;
	distance=sqrt(distance);
	if( distance > (r1+r2) ){ 
		return;
	}
	//printf("colided!!\n");	

	Vec normal=gap/distance;
	float penetration=r1+r2-distance;//distance;
	//float penetration=distance;
	// Penetration velocity :
	float vpen = (v1-v2)*normal;
	
	// Correction distribution (the heaviest object moves less) :
	float corr1 = invm1 / (invm1+invm2);
	float corr2 = invm2 / (invm1+invm2);
	
	// rebound :
	corr1 *= (1+rebound);
	corr2 *= (1+rebound);
	
	// position and velocity updates :
	x1 = x1 - (corr1 * penetration) * normal;
	v1 = v1 - (corr1 * vpen) * normal;
	x2 = x2 + (corr2 * penetration) * normal;
	v2 = v2 + (corr2 * vpen) * normal;		


 // Q_UNUSED(x1); Q_UNUSED(v1); Q_UNUSED(r1); Q_UNUSED(invm1);
 // Q_UNUSED(x2); Q_UNUSED(v2); Q_UNUSED(r2); Q_UNUSED(invm2);
 // Q_UNUSED(rebound);
}
