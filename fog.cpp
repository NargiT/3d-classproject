

#include <qdom.h>
#include <GL/gl.h>

#include "fog.h"
#include <qmessagebox.h>

using namespace std;




/**
* Constructor
*/
Fog::Fog()  {
  QString str("exp2");
  Color fogColor(0.3f, 0.8f, 0.98f);
  init(str, fogColor, 0.04f, 10.0f );
}


/**
* Init fog
*/
void Fog::init(const QString& type, Color fogColor, float val1, float val2)   {

  // enable fog effect
  glEnable(GL_FOG);  
  // set color of fog
  glFogfv(GL_FOG_COLOR, fogColor);  
  // clearing color buffer (set background of window)
  glClearColor(fogColor.r, fogColor.g, fogColor.b, 1.0f);
  glHint (GL_FOG_HINT, GL_NICEST);    // quality of fog (highest)

  if(type == "linear")  {
    glFogi(GL_FOG_MODE, GL_LINEAR);   // setting type of fog - linear function
    glFogf(GL_FOG_START, val1);       // bottom distance for start effect
    glFogf(GL_FOG_END, val2);         // top distance for finish effect (100% fog)
  }
  else  {
    if(type == "exp")
      glFogi(GL_FOG_MODE, GL_EXP);    // two different expo functions
    else if(type == "exp2")
      glFogi(GL_FOG_MODE, GL_EXP2);
    else
      glDisable(GL_FOG);

    // setting density of exponential functions (changeble parameter)
    glFogf(GL_FOG_DENSITY, val1);
  }

  return;
}

/**
* Init form DOM element
*/
void Fog::initFromDOMElement(const QDomElement& e) {
  float val1, val2;
  QString str;

  if (e.hasAttribute("type")) {
      str = e.attribute("type");
  }
  if (e.hasAttribute("val1")) {
      val1 = e.attribute("val1").toFloat();
  }
  if (e.hasAttribute("val2")) {
      val2 = e.attribute("val2").toFloat();
  }
  //-- parse child (Color)
  QDomNode n = e.firstChild();
  while (!n.isNull()) {
    QDomElement e = n.toElement();
    if (!e.isNull()) {

      if (e.tagName() == "Color") {
          color.initFromDOMElement(e);
      }
//      else
//        qDebug() << "Cannot parse " << e.tagName();

      n = n.nextSibling();
    }
  }

  init(str, color, val1, val2);
}

/**
* Enable/Disable fog (by push Key_T, but couldn't make it work)
*/
void Fog::switchFog()   {
  if(glIsEnabled(GL_FOG))
    glDisable(GL_FOG);
  else
    glEnable(GL_FOG);

  return;
}




