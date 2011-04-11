#include "object.h"
#include <qmessagebox.h>
#include <iostream>
#include <assert.h>

using namespace qglviewer;
using namespace std;

void Object::initFromDOMElement(const QDomElement& e) {
    QDomNode n = e.firstChild();
    while (!n.isNull()) {
        QDomElement e = n.toElement();
        if (!e.isNull()) {
            if (e.tagName() == "Material") {
                material_.initFromDOMElement(e);
            } else if (e.tagName() == "Frame") {
                frame_.initFromDOMElement(e);

                // Patch : Make sure the orientation is normalized.
                // Absolutely needed to correctly rotate rays.
                qglviewer::Quaternion o = frame_.orientation();
                o.normalize();
                frame_.setOrientation(o);
            } else if (e.tagName() == "AxisAngleKeyFrame") {
                qglviewer::Frame frame = frame_;
                qglviewer::Vec axis;
                axis.initFromDOMElement(e);
                float angle = e.attribute("angle", "0.0").toFloat();
                qglviewer::Quaternion animQuat;
                animQuat.setAxisAngle(axis, angle);
                frame.rotate(animQuat);
                keyframe_.push_back(KeyFrame(e.attribute("time", "0.0").toFloat(), frame));
            }
        } else
            QMessageBox::warning(NULL, "Object XML error", "Error while parsing Object XML document");
        n = n.nextSibling();
    }
}

/* Concatenate two Frame transformation f1 and f2
 */
Frame Object::MultiplyFrame(const Frame& f1, const Frame& f2) const {
    GLdouble matrix[4][4];
    const GLdouble* m1 = f1.matrix();
    const GLdouble* m2 = f2.matrix();

    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j) {
            matrix[i][j] = 0;
            for (int k = 0; k < 4; ++k) {
                matrix[i][j] += m1[k * 4 + i] * m2[j * 4 + k];
            }
        }

    // Check that the matrix is orthonormal
    assert(matrix[3][3] == 1);

    Frame f;
    f.setFromMatrix(matrix);
    return f;
}

void Object::animate(float time) {

    list<KeyFrame>::const_iterator iter = keyframe_.begin();
    list<KeyFrame>::const_iterator iter_next = iter;

    Quaternion q1, q2;
    float t1, t2;
    bool find = false;
    for (iter_next++; !find && iter_next != keyframe_.end(); iter++, iter_next++) {
        if (time >= (*iter).first && time < (*iter_next).first) {
            t1 = (*iter).first;
            q1 = (*iter).second.orientation();
            t2 = (*iter_next).first;
            q2 = (*iter_next).second.orientation();
            find = true;
        }
    }

    if (find) {
        float t = (time - t1) / (t2 - t1);
        Quaternion q = Quaternion::slerp(q1, q2, t, true);
        frame_.setRotation(q);
    }
}
