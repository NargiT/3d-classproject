#include "object.h"
#include <qmessagebox.h>
#include <iostream>
#include <assert.h>

using namespace qglviewer;
using namespace std;

void Object::initFromDOMElement(const QDomElement& e) {
    QDomNode n = e.firstChild();
#if QT_VERSION < 0x040000
    name_ = e.attribute("name", "NONE").ascii();
#else
    name_ = e.attribute("name", "NONE").toStdString();
#endif
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
            } else if (e.tagName() == "KeyFrame") {
                if (anglekeyframe_.size() == 0 && translationkeyframe_.size() == 0) {
                    qglviewer::Frame tmpFrame = frame_;
                    anglekeyframe_.push_back(KeyFrame(0.0, tmpFrame));
                    translationkeyframe_.push_back(KeyFrame(0.0, tmpFrame));
                }

                qglviewer::Frame frame = frame_;
                float t = e.attribute("time", "0.0").toFloat();
                QDomNode contentNode = e.firstChild();
                bool rotation = false;
                bool translation = false;
                while (!contentNode.isNull()) {
                    QDomElement nodeElement = contentNode.toElement();
                    if (nodeElement.tagName() == "RotationKeyFrame" && !rotation) {
                        rotation = true;
                        qglviewer::Vec axis;
                        axis.initFromDOMElement(nodeElement);
                        float angle = nodeElement.attribute("angle", "0.0").toFloat();
                        qglviewer::Quaternion animQuat;
                        animQuat.setAxisAngle(axis, angle);
                        frame.rotate(animQuat);
                        anglekeyframe_.push_back(KeyFrame(t, frame));
                    } else if (nodeElement.tagName() == "TranslationKeyFrame" && !translation) {
                        translation = true;
                        qglviewer::Vec axis;
                        axis.initFromDOMElement(nodeElement);
                        frame.translate(axis);
                        translationkeyframe_.push_back(KeyFrame(t, frame));
                    } 
	
 
			else
                        QMessageBox::warning(NULL, "Object XML error", "Error while parsing KeyFrames XML document");
                    contentNode = contentNode.nextSibling();
                }
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

/**
 * Animate rotation and / or translation
 **/
void Object::animate(float time) {

    Quaternion *q = new Quaternion;
    Vec *v = new Vec;
    bool qfind = animateAngle(time, q);
    bool vfind = animateTranslation(time, v);

    if (!qfind && !vfind) {
        return;
    }

    if (qfind && vfind) {
        frame_.setTranslationAndRotationWithConstraint(*v, *q);
    } else if (qfind) {
        frame_.setRotationWithConstraint(*q);
    } else { //if (vfind)
        frame_.setTranslationWithConstraint(*v);
    }

}

/**
 * Animate rotation
 **/
bool Object::animateAngle(float time, Quaternion *q) {
    list<KeyFrame>::const_iterator iter = anglekeyframe_.begin();
    list<KeyFrame>::const_iterator iter_next = iter;

    Quaternion q1, q2;
    float t1, t2;
    bool found = false;
    for (iter_next++; !found && iter_next != anglekeyframe_.end(); iter++, iter_next++) {
        if ((*iter).first <= time && time < (*iter_next).first) {
            found = true;

            t1 = (*iter).first;
            q1 = (*iter).second.rotation();

            t2 = (*iter_next).first;
            q2 = (*iter_next).second.rotation();

        }
    }
    if (found) {
        float t = (time - t1) / (t2 - t1);
        (*q) = Quaternion::slerp(q1, q2, t, true);
    }
    return found;
}

/**
 * Aimate translation
 **/
bool Object::animateTranslation(float time, Vec * v) {
    list<KeyFrame>::const_iterator iter = translationkeyframe_.begin();
    list<KeyFrame>::const_iterator iter_next = iter;

    Vec v1, v2;
    float t1, t2;
    bool find = false;
    for (iter_next++; !find && iter_next != translationkeyframe_.end(); iter++, iter_next++) {
        if ((*iter).first <= time && time < (*iter_next).first) {
            find = true;

            t1 = (*iter).first;
            v1 = (*iter).second.translation();

            t2 = (*iter_next).first;
            v2 = (*iter_next).second.translation();

            (*v).setValue((*iter).second.translation().x,
                    (*iter).second.translation().y,
                    (*iter).second.translation().z);
        }
    }

    if (find) {
        float t = (time - t1) / (t2 - t1);
        if (t != 0) {
            Vec nextTranslation = Vec((v2.x - v1.x) * t, (v2.y - v1.y) * t, (v2.z - v1.z) * t);
            (*v) += nextTranslation;
        }
    }
    return find;
}

