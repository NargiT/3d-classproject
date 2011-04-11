#include "ray.h"
#include <qmessagebox.h>

using namespace qglviewer;

Ray::Ray()
{
	setStart(Vec());
	setDirection(Vec(0.0, 0.0, 1.0));
}

Ray::Ray(const Vec start, const Vec direction)
{
	setStart(start);
	setDirection(direction);
}

void Ray::setDirection(const Vec& direction)
{
	if (direction.norm() < 1e-8)
	{
		QMessageBox::critical(NULL, "Ray::setDirection error", "Null length direction in Ray::setDirection");
		return;
	}
	direction_ = direction;
	direction_.normalize();
}

/* Draw a ray using GL_LINES
 */
void Ray::draw() const
{
	glDisable(GL_LIGHTING);
	glLineWidth(2.0f);

	glColor3f(1,1,1);
	glBegin(GL_LINES);
		glVertex3f(start_[0], start_[1], start_[2]);
		glVertex3f(start_[0] + 100.0f*direction_[0], start_[1] + 100.0f*direction_[1], start_[2] + 100.0f*direction_[2]);
	glEnd();

	glEnable(GL_LIGHTING);
}
