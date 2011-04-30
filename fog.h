#ifndef FOG_H
#define FOG_H

#include "object.h"

#include <qdom.h>

using namespace std;

class Fog //: public Object
{
public :

  /**
  * Constructor and destructor
  */
  Fog();
  ~Fog() {}

  /**
  * Init fog
  */
  void init(const QString &type, Color fogColor, float val1, float val2=0.0f);

  /**
  * Enable/Disable fog
  */
  void switchFog();

  /**
  * Parse XML
  */
  virtual void initFromDOMElement(const QDomElement& e);

private:
  Color color;    // color of fog
};


#endif // FOG_H
