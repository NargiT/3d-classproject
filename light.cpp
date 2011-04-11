#include "light.h"
#include "material.h"
#include "scene.h"
#include <qmessagebox.h>

using namespace qglviewer;


/* Code for the Light class
 */

void Light::initFromDOMElement(const QDomElement& e)
{
  QDomNode n = e.firstChild();
  while (!n.isNull())
    {
      QDomElement e = n.toElement();
      if (!e.isNull())
	{
	  if (e.tagName() == "Color")
	    color_.initFromDOMElement(e);
	}
      else
	QMessageBox::warning(NULL, "Light XML error", "Error while parsing Light XML document");
      n = n.nextSibling();
    }
}

/* Code for the AmbientLight class
 */

Color AmbientLight::illuminatedColor(const qglviewer::Vec& eye, const Hit& hit) const {
	Q_UNUSED(eye);
	Color cambient = color(); // light color
	Color cobjectdiffuse = hit.material().diffuseColor(); //object color
	return cambient*cobjectdiffuse;
};

/* Code for the DirectionnalLight class
 */

DirectionalLight::DirectionalLight()
{
  setDirection(Vec(1.0, 0.0, 0.0));
}

void DirectionalLight::initFromDOMElement(const QDomElement& e)
{
  Light::initFromDOMElement(e);
  
  QDomNode n = e.firstChild();
  while (!n.isNull())
    {
      QDomElement e = n.toElement();
      if (!e.isNull())
	{
	  if (e.tagName() == "Direction")
	    {
	      Vec dir;
	      dir.initFromDOMElement(e);
	      setDirection(dir);
	    }
	}
      else
	QMessageBox::warning(NULL, "DirectionalLight XML error", "Error while parsing DirectionalLight XML document");
      n = n.nextSibling();
    }
}

Color DirectionalLight::illuminatedColor(const qglviewer::Vec& eye, const Hit& hit) const {
	Q_UNUSED(eye);
	Color cpixel;
	return cpixel;
};

/* Code for the PointLight class
 */

void PointLight::initFromDOMElement(const QDomElement& e)
{
  Light::initFromDOMElement(e);
  
  QDomNode n = e.firstChild();
  while (!n.isNull())
    {
      QDomElement e = n.toElement();
      if (!e.isNull())
	{
	  if (e.tagName() == "Position")
	    position_.initFromDOMElement(e);
	}
      else
	QMessageBox::warning(NULL, "PointLight XML error", "Error while parsing PointLight XML document");
      n = n.nextSibling();
    }
}

Color PointLight::illuminatedColor(const qglviewer::Vec& eye, const Hit& hit) const {
	Q_UNUSED(eye);
	Color csourcei = color(); // light color
	Color cobjectdiffuse = hit.material().diffuseColor(); //object color
	Vec N = hit.normal(); // normal at the surface intersection point
	N.normalize();
	Vec Li = (position_ - hit.intersection()); // light direction
	Li.normalize();
	float scalarProduct = Li*N;
	if (scalarProduct < 0) scalarProduct = 0.0;
	return scalarProduct*csourcei*cobjectdiffuse;
}
