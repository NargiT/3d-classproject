/* 
 * File:   FishFlock.h
 */

#ifndef _FISHFLOCK_H
#define	_FISHFLOCK_H

#define CHAOS_TIMER 100

#include "object.h"
#include "light.h"
#include "fish.h"
#include <list>

class FishFlock : public Object {
public:
    FishFlock();
    virtual ~FishFlock();
    // Add a leave to the current node
    // We force the node to be non modifiable
    //
    void addFish(Fish* fish);

    //    // later on
    //    void addObstacle(Obstacle *obstacle);

    // Draw the object
    //
    virtual void draw() const;

    // Animate the object
    //
    virtual void animate(float time);

    // Bounding radius of the current scene.
    //
    virtual float boundingRadius() const;

    // Methode principale d'intersection entre l'objet et un rayon
    //
    virtual bool intersect(const Ray& ray, Hit& hit, qglviewer::Frame frame) const;

    // Load from the XML
    //
    virtual void initFromDOMElement(const QDomElement& e);

    float randomPosition(float min, float max);

    // rule 1 : cohesion
    // The boids move together
    qglviewer::Vec cohesion(Fish& fish);

    // rule 2 : separation
    // each boids do not collide with it's neigboor
    qglviewer::Vec separation(Fish& fish);

    // rule 3 : alignment
    // the fish have to try to go at the same velocity
    qglviewer::Vec alignment(Fish& fish);

    // rule 4 : chaos
    // there is a predator !!! it's chaos time
    qglviewer::Vec chaos(Fish& fish);

    // rule 5 : move
    qglviewer::Vec randomMove();
    
    // rule 0 : bounding box
    void boundPoisition(Fish& fish);

protected:
    void drawBoundingBox() const;
    void drawBoidsCenter(Fish& fish) const;
private:
    std::list<Object*> _fish;
    unsigned int population;
    float xmin;
    float xmax;
    float ymin;
    float ymax;
    float zmin;
    float zmax;

    qglviewer::Vec boids_center_;
    void init(unsigned int population,
            float xmin, float xmax,
            float ymin, float ymax,
            float zmin, float zmax);
};

#endif	/* _FISHFLOCK_H */

