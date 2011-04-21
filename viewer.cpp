#include "viewer.h"
#include "scene.h"
#include "material.h"
#include "color.h"

#include <qfiledialog.h>
#if QT_VERSION >= 0x040000
#include <QKeyEvent>
#endif
#include <math.h>

using namespace std;
using namespace qglviewer;

Viewer* Viewer::getViewer() {
    static Viewer* _instance = NULL;

    if (_instance == NULL)
        _instance = new Viewer();

    return _instance;
}

Viewer::~Viewer() {
    delete scene_;
}

void Viewer::draw() {
    scene_->draw();
    /*scene_->camera().draw(1);
    scene_->camera().drawAllRays();*/

    if (_selection) {
        _ray.draw();

        glDisable(GL_LIGHTING);
        glPointSize(4.0f);
        glColor3f(1, 0, 0);
        glBegin(GL_POINTS);
        glVertex3f(_hit_pos[0], _hit_pos[1], _hit_pos[2]);
        glEnd();
        glEnable(GL_LIGHTING);
    }
}

void Viewer::init() {
    // Key description for help window (press 'H')
    setKeyDescription(Qt::Key_L, "Loads a new scene");
    setKeyDescription(Qt::Key_S, "Shoot rays in the scene and saves the result");
    setKeyDescription(Qt::SHIFT + Qt::Key_S, "Shoot rays from <i>current</i> view point");

    scene_ = new Scene();
    setScene(scene_);
    rayTracer_.setScene(scene());

    // Ray viewing disabled at first
    _selection = false;
    time = 0.0;

    // So that transparent materials are correctly displayed.
    // Disable if rendering is too slow.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Restore previous viewer state (camera, key frames...)
    restoreStateFromFile();

    // Loads scene (prevents from pressing 'L' at each start).
    //loadScene("troisSpheres.scn");
    //loadScene("cylindres.scn");
    //loadScene("arm.scn");
    //loadScene("dynamics.scn");
    //loadScene("frame.scn");
    // Set Camera to scene Camera. Set scene center and radius.
    initFromScene();

    time = 0;
}

QString Viewer::helpString() const {
    QString text("<h2>L a n c e r   d e   r a y o n s</h2>");
    text += "Ajoutez ici toute aide utile � l'utilisateur. ";
    text += "N'oubliez pas de commenter vos raccourcis clavier avec setKeyDescription().<br>";
    text += "Appuyez sur <b>Escape</b> pour quitter.";
    return text;
}

void Viewer::keyPressEvent(QKeyEvent *e) {
    switch (e->key()) {
        case Qt::Key_L:
#if QT_VERSION < 0x040000
            loadScene(QFileDialog::getOpenFileName("", "Scenes (*.scn);;All files (*)", this));
#else
            loadScene(QFileDialog::getOpenFileName(this, "Select a scene", ".", "Scenes (*.scn);;All files (*)"));
#endif
            break;
        case Qt::Key_S:
#if QT_VERSION < 0x040000
            if ((e->state() == Qt::NoButton) || (e->state() == Qt::ShiftButton))
#else
            if ((e->modifiers() == Qt::NoModifier) || (e->modifiers() == Qt::ShiftModifier))
#endif	
            {
#if QT_VERSION < 0x040000
                if (e->state() == Qt::ShiftButton)
#else
                if (e->modifiers() == Qt::ShiftModifier)
#endif
                {
                    // Shift+S renders image from current view point
                    scene()->camera().frame().setPosition(camera()->position());
                    scene()->camera().frame().setOrientation(camera()->orientation());
                }

                rayTracer().cleanImageList();
                rayTracer().renderImage();
                // Remplacer cette ligne par : const QString name = "result.jpg";
                // pour ne pas avoir � choisir un nom � chaque fois.
                const QString name = QFileDialog::getSaveFileName();
                rayTracer().saveImage(name);

#if QT_VERSION < 0x040000
                if ((e->state() == Qt::ShiftButton) && (camera()->keyFrameInterpolator(1)))
#else
                if ((e->modifiers() == Qt::ShiftModifier) && (camera()->keyFrameInterpolator(1)))
#endif
                {
                    // Restore initial scene camera
                    scene()->camera().frame().setPosition(camera()->keyFrameInterpolator(1)->keyFrame(0).position());
                    scene()->camera().frame().setOrientation(camera()->keyFrameInterpolator(1)->keyFrame(0).orientation());
                }
            }
            break;
        default:
            QGLViewer::keyPressEvent(e);
    }
}

void Viewer::loadScene(const QString& name) {
    if (name.isEmpty())
        return;

    if (time > 0.0)
        time = 0.0;

    scene()->loadFromFile(name);

    // Change QGLViewer settings according to scene
    setSceneCenter(scene()->center());
    setSceneRadius(scene()->radius(sceneCenter()));

    // Set first camera key frame (F1) to camera position.
    initFromScene();
}

// Make the first keyFrameInterpolator (binded to F1) contain one position, that cooresponds to the
// scene's camera position. Attention, camera() is moved to camera position.

void Viewer::initFromScene() {
    camera()->setPosition(scene()->camera().frame().position());
    camera()->setOrientation(scene()->camera().frame().orientation());
    camera()->setFieldOfView(scene()->camera().fieldOfView());
    
    // Remove previous keyFrames in path 1 (if any)
    if (camera()->keyFrameInterpolator(1))
        camera()->keyFrameInterpolator(1)->deletePath();

    // Add current (i.e. scene camera) position to F1.
    camera()->addKeyFrameToPath(1);
    
}

void Viewer::select(const QPoint& point) {
    // Compute the ray
    Vec start, direction;
    camera()->convertClickToLine(point, start, direction);
    _ray.setStart(start);
    _ray.setDirection(direction);

    // Tray the given ray
    Hit hit;
    if (scene_->intersect(_ray, hit)) {
        _hit_pos = hit.intersection();
    } else {
        _hit_pos = Vec(1000, 1000, 1000);
    }

    _selection = true;
}

void Viewer::animate() {
    // Time management
    time++;
    scene()->animate(time);
}
