#include "rayTracer.h"
#include "scene.h"
#include "camera.h"
#include "hit.h"
#include "ray.h"

#include <qfileinfo.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <iostream>
#include <time.h>
#include <cstdlib>

using namespace std;

void RayTracer::renderAnimation(int first_frame, int last_frame) const {
    // For every time step do the animation
    // TODO add the motion blur
    for (int t = first_frame; t <= last_frame; ++t) {
        scene_->animate((float) t);
        renderImage();
    }
}

void RayTracer::renderImage() const {

    // Need to initialize a random machine
    srand (time(NULL));

    // Create a new image with the corresponding size
#if QT_VERSION < 0x040000
    QImage image_ = QImage(scene_->camera().xResolution(), scene_->camera().yResolution(), 32);
#else
    QImage image_ = QImage(scene_->camera().xResolution(), scene_->camera().yResolution(), QImage::Format_RGB32);
#endif

    // Populate every pixel
    for (int i = 0; i < scene_->camera().xResolution(); ++i) {
        for (int j = 0; j < scene_->camera().xResolution(); ++j) {
            Color current_color(0, 0, 0);
            for (int k = 0; k < antialiasing_; ++k) {
                // Random sub-pixel jittering (allow to do antialiasing)
                float di = (rand() - 0.5f) / (float) RAND_MAX + 0.5f;
                float dj = (rand() - 0.5f) / (float) RAND_MAX + 0.5f;

                // Construct ray from camera
                Ray ray = scene_->camera().getPixelRay(i + di, j + dj);
                current_color += rayColor(ray);
            }
            // Set the color
            image_.setPixel(i, j, current_color / (float) antialiasing_);
        }
    }
    // Put the current image in the list of images
    images_.push_back(image_);
}

Color RayTracer::rayColor(const Ray& ray) const {
    Color current_color(0, 0, 0); // TODO :: Replace by background color

    Hit hit;
    if (scene_->intersect(ray, hit)) {
        // For all light in the scene, integrate the radiance from the
        // current light to the intersection point.
        for (int k = 0; k < scene_->nbLights(); ++k) {
            current_color += scene_->getLight(k)->illuminatedColor(ray.start(), hit);
        }
    }

    return current_color;
}

void RayTracer::saveImage(const QString& name, bool overwrite) const {
    QString fileName = name;
    QFileInfo info(fileName);

    // Append jpg extension if needed
#if QT_VERSION < 0x040000
    if (info.extension(false) != "jpg")
#else
    if (info.suffix() != "jpg")
#endif
    {
        fileName += ".jpg";
        info.setFile(fileName);
    }

    // Prevent overwriting an existing file
    if (info.exists() && !overwrite)
        if (QMessageBox::warning(NULL, "Overwrite file ?",
                "File " + info.fileName() + " already exists.\nOverwrite ?",
                QMessageBox::Yes,
                QMessageBox::Cancel) == QMessageBox::Cancel)
            return;

    if (!image().save(fileName, "JPEG", 95))
        QMessageBox::information(NULL, "Error", "Error while saving " + fileName);
    else
        QMessageBox::information(NULL, "Saving done", "Image successfully saved in " + fileName);
}

void RayTracer::saveAnimation(const QString& name, bool overwrite) const {
    QString fileName = name;
    QFileInfo info(fileName);

    for (int i = 0; i < (int) images_.size(); ++i) {
        QString temp = fileName + QString::number(i) + QString(".jpg");
        if (!images_[i].save(temp, "JPEG", 95))
            QMessageBox::information(NULL, "Error", "Error while saving " + temp);
    }
}

