/* 
 * File:   Fish.cpp
 * Author: Tigran Tchougourian
 * 
 * Created on 26 April 2011, 00:38
 */

#include <GL/freeglut_std.h>

#include "fish.h"
#include "light.h"

using namespace qglviewer;
using namespace std;

Fish::Fish() {
    //    this->steering_direction = Vec(0.0f, 0.0f, 0.0f);
    //    this->orientation = frame_.orientation();
    //    this->max_force = 10.0;

    this->id_ = 0;
    this->mass_ = 5.0;
    this->max_speed = 1.1;
    this->position_ = frame_.position();
    this->cohesion_ = qglviewer::Vec(0.0, 0.0, 0.0);
    this->separation_ = qglviewer::Vec(0.0, 0.0, 0.0);
    this->alignment_ = qglviewer::Vec(0.0, 0.0, 0.0);
    this->velocity_ = qglviewer::Vec(0.0, 0.0, 0.0);
    this->chaos_ = qglviewer::Vec(0.0, 0.0, 0.0);
    this->go_ = qglviewer::Vec(0.0, 0.0, 0.0);
    this->avoid_ = qglviewer::Vec(0.0, 0.0, 0.0);
}

Fish::Fish(float mass, unsigned int id) {
    //    this->steering_direction = Vec(0.0f, 0.0f, 0.0f);
    //    this->orientation = frame_.orientation();
    //    this->max_force = 5.0;

    this->id_ = id;
    this->mass_ = abs(mass);
    this->max_speed = 1.1;
    this->position_ = frame_.position();
    this->cohesion_ = qglviewer::Vec(0.0, 0.0, 0.0);
    this->separation_ = qglviewer::Vec(0.0, 0.0, 0.0);
    this->alignment_ = qglviewer::Vec(0.0, 0.0, 0.0);
    this->velocity_ = qglviewer::Vec(0.0, 0.0, 0.0);
    this->chaos_ = qglviewer::Vec(0.0, 0.0, 0.0);
    this->go_ = qglviewer::Vec(0.0, 0.0, 0.0);
    this->avoid_ = qglviewer::Vec(0.0, 0.0, 0.0);
}

Fish::~Fish() {
}

void Fish::init(float mass) {
    this->mass_ = abs(mass);
}

void Fish::draw() const {
    glMatrixMode(GL_MODELVIEW);

    // Save the current model view matrix
    glPushMatrix();
    {
        // Multiply matrix to get in the frame coordinate system.
        glMultMatrixd(frame_.matrix());
        // Set color :
        glColor3fv(material().diffuseColor());
        glBegin(GL_TRIANGLES);
        // Draw trunk :

        Vec A = mass_ * Vec(0.0f, 0.25f, 0.0f); //P0
        Vec B = mass_ * Vec(0.2f, -0.25f, 0.0f); //P1
        Vec C = mass_ * Vec(-0.2f, -0.25f, 0.0f); //P2
        Vec D = mass_ * Vec(0.0f, -0.2f, 0.25f); // P3

        Vec AB = B - A;
        Vec AC = C - A;
        Vec AD = D - A;
        Vec DC = C - D;
        Vec DB = B - D;

        Vec crossProduct = Vec(AC.z * AB.y - AC.y * AB.z, AC.z * AB.x - AC.x * AB.z, AC.x * AB.y - AC.y * AB.x);
        Vec normal = crossProduct / crossProduct.norm();
        glNormal3fv(normal);
        // A
        glVertex3f(A.x, A.y, A.z);
        // B
        glVertex3f(B.x, B.y, B.z);
        // C
        glVertex3f(C.x, C.y, C.z);

        crossProduct = Vec(AD.z * AC.y - AD.y * AC.z, AD.z * AC.x - AD.x * AC.z, AD.x * AC.y - AD.y * AC.x);
        normal = crossProduct / crossProduct.norm();
        glNormal3fv(normal);
        // A
        glVertex3f(A.x, A.y, A.z);
        // C
        glVertex3f(C.x, C.y, C.z);
        // D
        glVertex3f(D.x, D.y, D.z);

        crossProduct = Vec(AD.z * AB.y - AD.y * AB.z, AD.z * AB.x - AD.x * AB.z, AD.x * AB.y - AD.y * AB.x);
        normal = crossProduct / crossProduct.norm();
        glNormal3fv(normal);
        // A
        glVertex3f(A.x, A.y, A.z);
        // D
        glVertex3f(D.x, D.y, D.z);
        // B
        glVertex3f(B.x, B.y, B.z);

        crossProduct = Vec(DC.z * DB.y - DC.y * DB.z, DC.z * DB.x - DC.x * DB.z, DC.x * DB.y - DC.y * DB.x);
        normal = crossProduct / crossProduct.norm();
        glNormal3fv(normal);
        // C
        glVertex3f(C.x, C.y, C.z);
        // B
        glVertex3f(B.x, B.y, B.z);
        // D
        glVertex3f(D.x, D.y, D.z);

        glEnd();
    }
    glPopMatrix();
}

void Fish::animate(float current_time) {
    // Euler integration

    // Simulation of 15 the direction in a 3D space where (0, 0, 0) means do not move
    //    steering_direction = Vec(
    //            ((rand() % 2) ? -1 : 1) * (1 + rand() % 5),
    //            ((rand() % 2) ? -1 : 1) * (1 + rand() % 5),
    //            ((rand() % 2) ? -1 : 1) * (1 + rand() % 5)
    //            );
    //    Vec acceleration = steering_direction / mass_;
    //    velocity = truncate (velocity + acceleration, max_speed)
    // applyForces();
    //    if ((int) current_time % 20 == 0) {
    //        Quaternion q = Quaternion(Vec(0.0f, 0.0f, 1.0f), atan2(acceleration.x, acceleration.y));
    //        frame_.rotate(q);
    //    }
    // limitVelocity();
    //    position_ = position_ + velocity_;
    frame_.setTranslation(position_);
}

// Rayon d'une sphčre englobante (meme grossiere).
//

float Fish::boundingRadius() const {
    return 50.0;
}

// Methode principale d'intersection entre l'objet et un rayon
//

bool Fish::intersect(const Ray& ray, Hit& hit, qglviewer::Frame frame) const {
    Q_UNUSED(ray);
    Q_UNUSED(hit);
    Q_UNUSED(frame);

    return false;
}

void Fish::initFromDOMElement(const QDomElement& e) {
    // init the object part
    Object::initFromDOMElement(e);

    // Parse for
    float mass = e.attribute("mass", "1.0").toFloat();
    //    int ntheta = e.attribute("ntheta", "10").toInt();
    //    int nphi = e.attribute("nphi", "10").toInt();
    init(mass);
}

void Fish::initPosition(qglviewer::Vec& vec) {
    this->position_ = vec;
    this->frame_.setTranslation(position_);
}

// Truncate the current velocity with the velocity limit
// Ex : if velocity > vlimit then velocity = vlimit

void Fish::limitVelocity() {
    if (velocity_.norm() > max_speed) {
        velocity_ = (velocity_ / velocity_.norm()) * max_speed;
    }
}

void computePosition() {

}

void Fish::boundPoisition(float Xmax, float Xmin, float Ymax, float Ymin, float Zmax, float Zmin) {

    if (position_.x > Xmax)
        position_.setValue(Xmax, position_.y, position_.z);
    else if (position_.x < Xmin)
        position_.setValue(Xmin, position_.y, position_.z);

    if (position_.y > Ymax)
        position_.setValue(position_.x, Ymax, position_.z);
    else if (position_.y < Ymin)
        position_.setValue(position_.x, Ymin, position_.z);

    if (position_.z > Zmax)
        position_.setValue(position_.x, position_.y, Zmax);
    else if (position_.z < Zmin)
        position_.setValue(position_.x, position_.y, Zmin);
}

/**
 * apply all the forces to the fish and truncate with the limit
 *
 **/
bool Fish::applyForces() {
    velocity_ = velocity_ + cohesion_ + alignment_ + separation_ + chaos_ + go_ + avoid_/*+ acceleration */;
    limitVelocity();
    return true;
}

/**
 * compute the next position of the fish
 **/
void Fish::computeNextPosition() {
    position_ = position_ + velocity_;
}

