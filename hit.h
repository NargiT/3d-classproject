#ifndef HIT_H
#define HIT_H

#include <QGLViewer/frame.h>
#include "material.h"

class Hit
{
public :
  Hit() { setTime(1.0E5); normal_ = qglviewer::Vec(0, 0, 1); }

  qglviewer::Vec intersection() const { return intersection_; }
  float time() const { return time_; }
  const Material& material() const { return material_; }
  const qglviewer::Vec normal() const { return normal_; }

  void setIntersection(const qglviewer::Vec& intersection) { intersection_ = intersection; }
  void setMaterial(const Material& material) { material_ = material; }
  void setNormal(const qglviewer::Vec& normal) { normal_ = normal; }
  void setTime(float time) { time_ = time; }
  
private:
  qglviewer::Vec intersection_, normal_;
  Material material_;
  float time_;
};

#endif // HIT_H
