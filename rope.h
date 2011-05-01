#pragma once

#include <QGLViewer/vec.h>
#include <QGLViewer/manipulatedFrame.h>
#include "node.h"
#include "spring.h"


class Rope : public Node 
{
	public:

		// initialize dynamic animation
		Rope()
			: staticPosition(0.0f, 0.0f, 0.0f),
			groundVelocity(0.0f, 0.0f, 0.0f),
			groundNormal(0.0f, 0.0f, 1.0f),
			gravity(0.0f, 0.0f, -1.0f), viscosity(1.0f),stiffness(10.0f),initLength(0.1f),damping(1.0f),dynamicPosition(0.0f,0.0f,0.0f) {}

		~Rope(){}

		// ACCESSORS

		// add a ball to dynamic animation and send back an identifier for this ball
		unsigned int addBall(const qglviewer::Vec& ballPos,
				const qglviewer::Vec& velocity, float mass, float radius);
		// add a spring between two balls
		unsigned int addSpring(unsigned int ball1, unsigned int ball2,
				float stiffness, float initLength, float dampingFactor);

		// set gravity vector
		void setGravity(const qglviewer::Vec& grav);

		// get ball position
		const qglviewer::Vec& getBallPos(unsigned int ball) const {
			return positions[ball];
		}

		// get ball radius
		double getBallRadius(unsigned int ball) const {return radiuses[ball];}
		// get number of springs
		unsigned int getNbSprings() const {return (unsigned int) springs.size();}
		// get a spring
		const Spring& getSpring (unsigned int spring) const {return springs[spring];}

		// METHODS

		// compute collision between a ball and a plane
		void collisionBallPlane
			(qglviewer::Vec& x1, qglviewer::Vec& v1, float r1, float invm1,
			 qglviewer::Vec& x2, qglviewer::Vec& v2, qglviewer::Vec& normal, float invm2,
			 float rebound );

		// compute collision between a ball and a ball
		void collisionBallBall
			(qglviewer::Vec& x1, qglviewer::Vec& v1, float r1, float invm1,
			 qglviewer::Vec& x2, qglviewer::Vec& v2, float r2, float invm2,
			 float rebound );

		// update positions and velocities of dynamic objects after a time step dt
		void animate(float dt);

		// Affichage openGL de l'objet
		//
		void draw() const;

		// Parse the XML File
		//
		void initFromDOMElement(const QDomElement& e);


	private:
		// balls
		std::vector<qglviewer::Vec> positions; 
		std::vector<qglviewer::Vec> velocities; 
  		std::vector<Object*> drawing_sphere;
		std::vector<float> masses; 
		std::vector<float> invMasses; 
		std::vector<float> radiuses;

		// ground
		qglviewer::Vec dynamicPosition;
		qglviewer::Vec worldRef;
		qglviewer::Vec staticPosition;
		qglviewer::Vec groundVelocity;
		qglviewer::Vec groundNormal;

		// forces
		std::vector<Spring> springs;
		qglviewer::Vec gravity;
		float viscosity;

                // integration time step
                float dt;
		
                // Manipulated frame, used to move the dynamic
		// ball in the scene.
		//
		qglviewer::ManipulatedFrame _manipulated_frame;

// some constants that can be used for springs
float stiffness ;
float initLength ;
float damping ;
//float viscosity ;



};

