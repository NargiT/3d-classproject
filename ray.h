#ifndef RAY_H
#define RAY_H

#include <QGLViewer/frame.h>

class Ray
{
public :
  Ray();
  Ray(const qglviewer::Vec start, const qglviewer::Vec dir);
  
  qglviewer::Vec start() const { return start_; }
  qglviewer::Vec direction() const { return direction_; }
  
  void setStart(const qglviewer::Vec& start) { start_ = start; }
  void setDirection(const qglviewer::Vec& direction);

  void draw() const;

private:
  qglviewer::Vec start_, direction_;
  qglviewer::Frame current_frame_;
};

#endif // RAY_H
