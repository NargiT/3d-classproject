/* 
 * File:   FishFlock.cpp
 * 
 */
#include "QGLViewer/qglviewer.h"
#include "fishflock.h"
#include "fish.h"
#include "sphere.h"
#include "stdlib.h"

#include <float.h>
#include <qmessagebox.h>

#ifndef fmax
#define fmax max
#endif

using namespace std;
using namespace qglviewer;

FishFlock::FishFlock() {
    population = 5;
    xmax = 32.0;
    xmin = 32.0;
    ymin = 32.0;
    ymax = 32.0;
    zmin = 32.0;
    zmax = 32.0;
}

FishFlock::~FishFlock() {
    std::list<Object*>::const_iterator iter = _fish.begin();
    for (; iter != _fish.end();) {
        delete (*iter++);
    }
}

void FishFlock::init(unsigned int population,
        float xmin, float xmax,
        float ymin, float ymax,
        float zmin, float zmax) {
    srand(time(NULL));

    this->population = population;
    this->xmin = xmin;
    this->xmax = xmax;
    this->ymin = ymin;
    this->ymax = ymax;
    this->zmin = zmin;
    this->zmax = zmax;
}

/* Add a node in the current tree
 */
void FishFlock::addFish(Fish* fish) {
    _fish.push_back(fish);
}

void FishFlock::draw() const {
    // Apply the transform
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glMultMatrixd(frame_.matrix());

    //    drawBoundingBox();
    // Display the leaves of this node
    std::list<Object*>::const_iterator iter = _fish.begin();
    for (; iter != _fish.end(); iter++) {
        Fish *tmp = static_cast<Fish*> (*iter);
        //(*iter)->draw();
        tmp->draw();
        //        drawBoidsCenter(*tmp);
    }
    // Go back to the father's frame
    glPopMatrix();

}

void FishFlock::animate(float time) {
    Object::animate(time);

    std::list<Object*>::const_iterator iter = _fish.begin();
    boids_center_ = Vec(frame_.position());

    // choose a random direction to go
    Vec rule5 = randomMove();
    Vec rule6 = -20 * randomMove();

    for (; iter != _fish.end(); iter++) {
        Fish *currentFish = static_cast<Fish*> (*iter);

        //        if ((int) time % 666 == 0)
        //            currentFish->startChaos(100);

        Vec rule1 = cohesion(*currentFish);
        Vec rule2 = separation(*currentFish);
        Vec rule3 = alignment(*currentFish);
        Vec rule4 = chaos(*currentFish);

        currentFish->cohesion(rule1);
        currentFish->separation(rule2);
        currentFish->alignment(rule3);
        //        currentFish->chaos(rule4);
        //        currentFish->go(rule5);
        //        currentFish->avoid(rule6);

        currentFish->applyForces();
        currentFish->computeNextPosition();
        // rule 0
        boundPoisition(*currentFish);

        currentFish->animate(time);

        boids_center_ += currentFish->frame().position();
    }
    boids_center_ /= _fish.size();
}

float FishFlock::boundingRadius() const {
    static float radius = 0.0f;

    std::list<Object*>::const_iterator iter = _fish.begin();
    for (; iter != _fish.end(); iter++) {
        ;

        radius = fmax(radius, (*iter)->boundingRadius() + (*iter)->frame().translation().norm());
    }

    return radius;
}

bool FishFlock::intersect(const Ray& ray, Hit& min_dist_hit, Frame frame) const {
    float dist = FLT_MAX;
    bool is_intersecting = false;
    frame = MultiplyFrame(frame, frame_);

    Ray new_ray(frame_.coordinatesOf(ray.start()), frame_.transformOf(ray.direction()));

    std::list<Object*>::const_iterator iter = _fish.begin();
    for (; iter != _fish.end(); iter++) {
        Hit hit;
        is_intersecting = (*iter)->intersect(new_ray, hit, frame) || is_intersecting;

        float current_dist = hit.time();
        if (current_dist < dist) {

            dist = current_dist;
            min_dist_hit = hit;
        }
    }

    return is_intersecting;
}

/* Load the FishFlock from a XML
 */
void FishFlock::initFromDOMElement(const QDomElement& e) {
    Object::initFromDOMElement(e);

    unsigned int population = e.attribute("population", "5.0").toUInt();
    float xmin = e.attribute("xmin", "-32.0").toFloat();
    float xmax = e.attribute("xmax", "32.0").toFloat();
    float ymin = e.attribute("ymin", "-32.0").toFloat();
    float ymax = e.attribute("ymax", "32.0").toFloat();
    float zmin = e.attribute("zmin", "-32.0").toFloat();
    float zmax = e.attribute("zmax", "32.0").toFloat();

    init(population, xmin, xmax, ymin, ymax, zmin, zmax);

    QDomNode n = e.firstChild();
    while (!n.isNull()) {
        QDomElement e = n.toElement();
        if (!e.isNull()) {
            if (e.tagName() == "Fish") {
                Fish* f = new Fish();
                f->initFromDOMElement(e);

                for (unsigned int i = 0; i < population; i++) {
                    // create a new fish and assignmnt it a identifier
                    Fish *f1 = new Fish(f->mass(), i);
                    // set the current frame
                    f1->setFrame(f->frame());
                    f1->setMaterial(f->material());
                    // position the fish in the space (random)
                    Vec v = Vec(randomPosition(xmin, xmax), randomPosition(ymin, ymax), randomPosition(zmin, zmax));
                    f1->initPosition(v);
                    // add the fish to the boids
                    addFish(f1);
                    // calculate the center
                    boids_center_ += v;
                }
                boids_center_ /= _fish.size();
                // delete the generic fish
                delete f;
            }
        } else
            QMessageBox::warning(NULL, "Object XML error", "FishFlock needs the fish");
        n = n.nextSibling();
    }
}

float FishFlock::randomPosition(float min, float max) {
    float toReturn = 0.0;
    if (min < max)
        toReturn = min + (max - min) * rand() / ((float) RAND_MAX + 1.0);
    else
        toReturn = max + (min - max) * rand() / ((float) RAND_MAX + 1.0);
    return toReturn;
}

qglviewer::Vec FishFlock::cohesion(Fish& fish) {
    Vec perceived_center;
    std::list<Object*>::const_iterator iter = _fish.begin();
    for (; iter != _fish.end(); iter++) {
        Fish *tmp = static_cast<Fish*> (*iter);
        if (fish.id() != tmp->id()) {
            perceived_center += tmp->frame().position();
        }
    }
    perceived_center /= _fish.size() - 1;

    return (perceived_center - fish.frame().position()) / 100.0;
}

qglviewer::Vec FishFlock::separation(Fish& fish) {
    Vec c;
    std::list<Object*>::const_iterator iter = _fish.begin();
    for (; iter != _fish.end(); iter++) {
        Fish *tmp = static_cast<Fish*> (*iter);
        if (fish.id() != tmp->id()) {
            Vec res = fish.frame().position() - tmp->frame().position();
            // to be close enough
            if (abs(res.norm()) < fish.mass() / 2.0) {
                c += (fish.frame().position() - tmp->frame().position());
            }
        }
    }
    // to avoid that 2 boids go to far from each other
    // if distance = 4 then distance between birds at time t+1 will be 8 !!!
    return c / 2.0;
}

qglviewer::Vec FishFlock::alignment(Fish& fish) {
    Vec perceived_velocity;
    std::list<Object*>::const_iterator iter = _fish.begin();
    for (; iter != _fish.end(); iter++) {
        Fish *tmp = static_cast<Fish*> (*iter);
        if (fish.id() != tmp->id()) {
            perceived_velocity += tmp->velocity();
        }
    }
    perceived_velocity /= _fish.size() - 1;
    return (perceived_velocity - fish.velocity()) / 8.0;
}

qglviewer::Vec FishFlock::chaos(Fish& fish) {
    if (fish.chaos()) {
        fish.decreaseChaosTimer();
        return -45 * cohesion(fish);
    }
    return Vec();
}

qglviewer::Vec FishFlock::randomMove() {
    Vec steering_direction = Vec(
            ((rand() % 2) ? -1 : 1) * (1 + rand() % 5),
            ((rand() % 2) ? -1 : 1) * (1 + rand() % 5),
            ((rand() % 2) ? -1 : 1) * (1 + rand() % 5)
            );
    return steering_direction / 100;
}

void FishFlock::boundPoisition(Fish& fish) {
    fish.boundPoisition(xmax, xmin, ymax, ymin, zmax, zmin);
}

void FishFlock::drawBoundingBox() const {
    glBegin(GL_LINE);
    {
        // A
        glVertex3f(xmin, ymin, zmin);
        // B
        glVertex3f(xmin, ymin, zmax);

        // B
        glVertex3f(xmin, ymin, zmax);
        // C
        glVertex3f(xmin, ymax, zmax);

        // C
        glVertex3f(xmin, ymax, zmax);
        // D
        glVertex3f(xmin, ymax, zmin);

        // A
        glVertex3f(xmin, ymin, zmin);
        // D
        glVertex3f(xmin, ymax, zmin);

        // G
        glVertex3f(xmax, ymax, zmax);
        // H
        glVertex3f(xmax, ymax, zmin);

        // H
        glVertex3f(xmax, ymax, zmin);
        // E
        glVertex3f(xmax, ymin, zmin);

        // E
        glVertex3f(xmax, ymin, zmin);
        // F
        glVertex3f(xmax, ymin, zmax);

        // G
        glVertex3f(xmax, ymax, zmax);
        // F
        glVertex3f(xmax, ymin, zmax);

        // C
        glVertex3f(xmin, ymax, zmax);
        // G
        glVertex3f(xmax, ymax, zmax);

        // E
        glVertex3f(xmax, ymin, zmin);
        // A
        glVertex3f(xmin, ymin, zmin);

        // B
        glVertex3f(xmin, ymin, zmax);
        // F
        glVertex3f(xmax, ymin, zmax);

        // H
        glVertex3f(xmax, ymax, zmin);
        // D
        glVertex3f(xmin, ymax, zmin);

    }
    glEnd();
}

void FishFlock::drawBoidsCenter(Fish& fish) const {
    glBegin(GL_LINE_STRIP);
    {
        glVertex3fv(fish.frame().position());
        glVertex3fv(boids_center_);
    }
    glEnd();
}
