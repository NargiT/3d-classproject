#pragma once

#include "color.h"
#include "hit.h"
#include "ray.h"

class Scene;

class Light
{
	public :
		Color color() const { return color_; }
		void setColor(const Color& color) { color_ = color; }

		// Load a light from XML, this method should be overloaded
		// by the sons of this class
		//
		virtual void initFromDOMElement(const QDomElement& e);

		// Compute the integral of the radiance from the light
		// to the hit point that goes to the camera
		//
		virtual Color illuminatedColor(const qglviewer::Vec& eye, const Hit& hit) const = 0;

	private:
		Color color_;
};


class AmbientLight : public Light
{
	public :
		// Constructor and Destructor
		//
		AmbientLight() {} ;
		virtual ~AmbientLight() {} ;

		// Compute the integral of the radiance from the light
		// to the hit point that goes to the camera
		//
		virtual Color illuminatedColor(const qglviewer::Vec& eye, const Hit& hit) const ;
};


class DirectionalLight : public Light
{
	public :
		DirectionalLight();

		qglviewer::Vec direction() const { return direction_; }
		void setDirection(const qglviewer::Vec& direction) { direction_ = direction; direction_.normalize(); }

		virtual void initFromDOMElement(const QDomElement& e);

		// Compute the integral of the radiance from the light
		// to the hit point that goes to the camera
		//
		virtual Color illuminatedColor(const qglviewer::Vec& eye, const Hit& hit) const;

	private:
		qglviewer::Vec direction_;
};


class PointLight : public Light
{
	public :
		qglviewer::Vec position() const { return position_; }
		void setPosition(const qglviewer::Vec& position) { position_ = position; }

		virtual void initFromDOMElement(const QDomElement& e);

		// Compute the integral of the radiance from the light
		// to the hit point that goes to the camera
		//
		virtual Color illuminatedColor(const qglviewer::Vec& eye, const Hit& hit) const;

	private:
		qglviewer::Vec position_;
};

