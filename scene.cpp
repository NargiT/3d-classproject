#include "QGLViewer/qglviewer.h"
#include "scene.h"
#include "sphere.h"
#include "cylinder.h"

#include <iostream>
#include <qfile.h>


using namespace qglviewer;
using namespace std;

/* Need to create the scene Node
 */
Scene::Scene()
{
	_top_node = new Node();
}

/* Clean up memory by deleting the top node
 */
Scene::~Scene()
{
	delete _top_node;
	for(int i=0; i<_lights.size(); ++i)
		delete _lights[i];
}

/* Clean up the view matrix using the camera transform
 * and do the drawing
 */
void Scene::draw() const
{
	_top_node->draw();
}

void Scene::animate(float a) 
{
  _top_node->animate(a);
}

void Scene::addObject(Object* o)
{
	_top_node->addObject(o);
}

/* Add a Light to the Light list
 */
void Scene::addLight(const Light* light)
{
	_lights.push_back(light);
}

void Scene::loadFromFile(const QString& filename)
{
	delete _top_node;
	_top_node = new Node();

	QDomDocument doc;
	QFile file(filename);
	//if (!file.open(QIODevice::ReadOnly))
	//	return;
	if (!doc.setContent(&file)) {
		file.close();
		return;
	}
	file.close();

	// Parse all first elements
	QDomElement docElem = doc.documentElement();
	QDomNode n = docElem.firstChild();
	while(!n.isNull()) {
		QDomElement e = n.toElement();

		if(!e.isNull()) {
			if (e.tagName() == "Sphere")
			{
				Sphere* s = new Sphere();
				s->initFromDOMElement(e);
				addObject(s);
			}
			else if(e.tagName() == "Node")
			{
				Node* n = new Node();
				n->initFromDOMElement(e);
				addObject(n);
			}
			else if(e.tagName() == "Cylinder")
			{
				Cylinder* c = new Cylinder();
				c->initFromDOMElement(e);
				addObject(c);
			}
			else if(e.tagName() == "Camera")
			{
				camera_.initFromDOMElement(e);
			}
			else if (e.tagName()=="AmbientLight") 
			{
				AmbientLight *li = new AmbientLight;
				li->initFromDOMElement(e);
				addLight(li);
			}
			else if (e.tagName()=="DirectionalLight") 
			{
				DirectionalLight *li = new DirectionalLight;
				li->initFromDOMElement(e);
				addLight(li);
			}
			else if (e.tagName()=="PointLight") 
			{
				PointLight *li = new PointLight;
				li->initFromDOMElement(e);
				addLight(li);
			}
			else
			{
			  cout << "TODO element : \n";// << qPrintable(e.tagName()) << endl;
			}
		}
		else
		{
			cerr << "Unable to parse this element" << endl;
		}
		n = n.nextSibling();
	}

}

Vec Scene::center() const
{
  // A FAIRE

  return Vec();
}

float Scene::radius(const Vec& center) const
{
	Q_UNUSED(center);
	return _top_node->boundingRadius();
}

/* Test intersection with all the elements of the scene
 */
bool Scene::intersect(const Ray& ray, Hit& hit) const
{
	Frame frame;
	return _top_node->intersect(ray, hit, frame);
}
