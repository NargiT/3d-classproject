#ifndef RAY_TRACER_H
#define RAY_TRACER_H

#include <qimage.h>
#include <vector>
#include "color.h"
#include "ray.h"

class Scene;

class RayTracer
{
public :
  RayTracer() : scene_(NULL), antialiasing_(4) {};

  const Scene* scene() const { return scene_; }
  const QImage& image() const { return images_[0]; }

  void setScene(Scene* scene) { scene_ = scene; }
  void setAntialiasingSamplesNumber(int n) { antialiasing_ = n; }

  void renderImage() const;
  void renderAnimation(int first_frame, int last_frame) const;
  void cleanImageList() const { images_.clear(); } ;

  void saveImage(const QString& name, bool overwrite=false) const;
  void saveAnimation(const QString& name, bool overwrite=false) const;

  Color rayColor(const Ray& ray) const;

private:
  Scene* scene_;
  mutable std::vector<QImage> images_;

  int antialiasing_;
};

#endif // RAY_TRACER_H
