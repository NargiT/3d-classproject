/* 
 * File:   Fish.h
 * Author: Tigran Tchougourian
 *
 * Created on 26 April 2011, 00:38
 */

#ifndef _Fish_H
#define	_Fish_H

#include "object.h"
#include "QGLViewer/quaternion.h"
#include "QGLViewer/vec.h"

class Fish : public Object {
public:
    Fish();
    Fish(float mass, unsigned int id);

    virtual ~Fish();
    // Draw using OpenGL
    //
    virtual void draw() const;

    // Animate the Fish
    //
    void animate(float current_time);

    // Rayon d'une sphčre englobante (meme grossiere).
    //
    virtual float boundingRadius() const;

    // Methode principale d'intersection entre l'objet et un rayon
    //
    virtual bool intersect(const Ray& ray, Hit& hit, qglviewer::Frame frame) const;

    // Parse XML
    //
    virtual void initFromDOMElement(const QDomElement& e);

    void initPosition(qglviewer::Vec& vec);

    float mass() const {
        return mass_;
    };

    unsigned int id() const {
        return id_;
    };

    void cohesion(qglviewer::Vec& v) {
        cohesion_ = v;
    };

    void separation(qglviewer::Vec& v) {
        separation_ = v;
    };

    void alignment(qglviewer::Vec& v) {
        alignment_ = v;
    };

    void chaos(qglviewer::Vec& v) {
        chaos_ = v;
    };

    void decreaseChaosTimer() {
        if (chaos_timer > 0) {
            chaos_timer--;
        } else {
            isChaos = false;
        }
    }

    void startChaos(int chaos_timer) {
        isChaos = true;
        this->chaos_timer = chaos_timer;
    }

    bool chaos() const {
        return isChaos;
    };

    void go(qglviewer::Vec& v) {
        go_ = v;
    }

    void avoid(qglviewer::Vec& v) {
        avoid_ = v;
    }

    qglviewer::Vec velocity() {
        return velocity_;
    }

    void boundPoisition(float Xmax, float Xmin, float Ymax, float Ymin, float Zmax, float Zmin);

    bool applyForces();

    void computeNextPosition();

private:
    // Init elements after DOM callback
    //
    void init(float mass);
    void limitVelocity();

//    qglviewer::Vec steering_direction;
//    qglviewer::Quaternion orientation; // orientation of the Fish
//    float max_force; //


    qglviewer::Vec position_;
    qglviewer::Vec velocity_; // bounded by max_speed

    qglviewer::Vec cohesion_;
    qglviewer::Vec separation_;
    qglviewer::Vec alignment_;
    qglviewer::Vec chaos_;
    qglviewer::Vec go_;
    qglviewer::Vec avoid_;


    int chaos_timer;
    bool isChaos;
    unsigned int id_;
    float max_speed; //
    float mass_; // weight

};

#endif	/* _Fish_H */

