#pragma once

#include "QGLViewer/qglviewer.h"
#include "material.h"
#include "hit.h"
#include "ray.h"


#ifndef fmax
#define fmax max
#endif

// Classe générique abstraite dont vont deriver les objets de la scene.
class Object
{
public :
  virtual ~Object() {};
  
  //type for key Frames, a pair of <time, Frame>
  typedef std::pair<float, qglviewer::Frame> KeyFrame;

  //key frame smart array
  std::list<KeyFrame> keyframe_;
  
  // Accesseurs
  //
  const Material& material() const { return material_; }
  const qglviewer::Frame& frame() const { return frame_; }

  // Modifieurs
  //
  void setMaterial(const Material& material) { material_ = material; }
  void setFrame(const qglviewer::Frame& frame) { frame_ = frame; }

  // Affichage openGL de l'objet
  //
  virtual void draw() const = 0;
  virtual void animate(float time);

  // Rayon d'une sphère englobante (meme grossiere).
  //
  virtual float boundingRadius() const = 0;

  // Intersection method between a Ray and the Object. The resulting
  // intersection (if any) is stored in the Hit. The associated Frame
  // allow to transform from local coordinates of this node to global
  // coordinates.
  //
  virtual bool intersect(const Ray&, Hit& hit, qglviewer::Frame frame) const = 0;

  virtual void initFromDOMElement(const QDomElement& e);

  // This operator allow to concatenate two transformations. It performs
  // the multiplication of the two transformation matrices in the order of
  // the arguments.
  //
  qglviewer::Frame MultiplyFrame(const qglviewer::Frame& f1, const qglviewer::Frame& f2) const;

protected :
  // Tous les objets ont un repere et un materiau.
  //
  qglviewer::Frame frame_;
  Material material_;
};
