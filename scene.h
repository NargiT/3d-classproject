#pragma once

#include <vector>

#include "object.h"
#include "camera.h"
#include "node.h"
#include "ray.h"
#include "hit.h"
#include "light.h"
#include "fog.h"

#include <vector>

class Scene
{
	public :
    // Fog (scattering)
    Fog* fog;

		Scene() ;
		~Scene() ;
		void draw() const;
		void animate(float time);
		void addObject(Object* o);
		void loadFromFile(const QString& filename);

		qglviewer::Vec center() const;
		float radius(const qglviewer::Vec& center) const;

		void setCamera(const Camera& camera) { camera_ = camera; }
		const Camera& camera() const { return camera_; }
		Camera& camera() { return camera_; }

		// Add a light in the scene
		//
		void addLight(const Light* light) ;
  
  Color illuminatedColor(Hit& hit) const;

		// Access to the Lights
		//
		int nbLights() const { return _lights.size(); } ;
		const Light* getLight(int i) const { return _lights[i]; } ;

		// Does the ray intersect the scene
		//
		bool intersect(const Ray& ray, Hit& hit) const;

	private:
		// Camera object
		//
		Camera camera_;

		// First node of the Scenegraph
		//
		Node* _top_node;

		// All the Light in the scene are stored here
		//
		std::vector<const Light*> _lights;
  std::vector<Light*> lights_;
};
