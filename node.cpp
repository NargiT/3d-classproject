#include "QGLViewer/qglviewer.h"
#include "sphere.h"
#include "cylinder.h"
#include "dynamics.h"
#include "planTexture.h"
#include "plane.h"
#include "rope.h"
#include "fishflock.h"
#include "airbubble.h"

#include <float.h>
#include <qmessagebox.h>

#ifndef fmax
#define fmax max
#endif

using namespace std;
using namespace qglviewer;

/* Constructor
 */
Node::Node() : is_constant(false) {
}

/* Destructor
 */
Node::~Node() {
    std::list<Object*>::const_iterator iter = _leaves.begin();
    for (; iter != _leaves.end();) {
        delete (*iter++);
    }
}

/* Add a node in the current tree
 */
void Node::addObject(Object* node) {
    _leaves.push_back(node);
    is_constant = false;
}

/* OpenGL display of the Node
 * We display all of the leaves of the current Node 
 */
void Node::draw() const {

    // Apply the transform
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glMultMatrixd(frame_.matrix());

    // Display the leaves of this node
    std::list<Object*>::const_iterator iter = _leaves.begin();
    for (; iter != _leaves.end(); iter++) {
        (*iter)->draw();
    }

    // Go back to the father's frame
    glPopMatrix();
}

void Node::animate(float time) {
    Object::animate(time);

    std::list<Object*>::const_iterator iter = _leaves.begin();
    for (; iter != _leaves.end(); iter++) {
        (*iter)->animate(time);
    }
}

/* Radius of the scene node
 * We use radius caching to speed up things
 */
float Node::boundingRadius() const {
    static float radius = 0.0f;

    if (!is_constant) {
        std::list<Object*>::const_iterator iter = _leaves.begin();
        for (; iter != _leaves.end(); iter++) {
            radius = fmax(radius, (*iter)->boundingRadius() + (*iter)->frame().translation().norm());
        }
        is_constant = true;
    }

    return radius;
}

/* Intersection between the ray and the list of all object in this
 * node 
 */
bool Node::intersect(const Ray& ray, Hit& min_dist_hit, Frame frame) const {
    float dist = FLT_MAX;
    bool is_intersecting = false;
    frame = MultiplyFrame(frame, frame_);

    Ray new_ray(frame_.coordinatesOf(ray.start()), frame_.transformOf(ray.direction()));

    std::list<Object*>::const_iterator iter = _leaves.begin();
    for (; iter != _leaves.end(); iter++) {
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

/* Load the Node from a XML
 */
void Node::initFromDOMElement(const QDomElement& e) {
    Object::initFromDOMElement(e);

    QDomNode n = e.firstChild();
    while (!n.isNull()) {
        QDomElement e = n.toElement();
        if (!e.isNull()) {
            if (e.tagName() == "Node") {
                Node* n = new Node();
                n->initFromDOMElement(e);
                addObject(n);
            } else if (e.tagName() == "Sphere") {
                Sphere* s = new Sphere();
                s->initFromDOMElement(e);
                addObject(s);
            } else if (e.tagName() == "Plane") {
              Plane* p = new Plane();
              p->initFromDOMElement(e);
              addObject(p);
            } else if (e.tagName() == "Dynamics") {
                Dynamics* d = new Dynamics();
                d->initFromDOMElement(e);
                addObject(d);
            } else if (e.tagName() == "Cylinder") {
                Cylinder* c = new Cylinder();
                c->initFromDOMElement(e);
                addObject(c);
            } else if (e.tagName() == "PlanTexture") {
                PlanTexture* p = new PlanTexture();
                p->initFromDOMElement(e);
                addObject(p);
            } else if (e.tagName() == "FishFlock") {
                FishFlock* f = new FishFlock();
                f->initFromDOMElement(e);
                addObject(f);
            } else if (e.tagName() == "Rope") {
                Rope* d = new Rope();
                d->frame().setReferenceFrame(&frame_);
                d->initFromDOMElement(e);
                addObject(d);
            } else if (e.tagName() == "Airbubble") {
                Airbubble* a = new Airbubble();
                a->frame().setReferenceFrame(&frame_);
                a->initFromDOMElement(e);
                addObject(a);
            }


            // TODO others nodes
        } else
            QMessageBox::warning(NULL, "Object XML error", "Error while parsing Object XML document");
        n = n.nextSibling();
    }
}
