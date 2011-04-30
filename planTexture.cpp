#include "planTexture.h"
#include "material.h"

#include <GL/glut.h>
#include <cmath>
#include <float.h>

// Pour ne pas avoir a mettre qglviewer:: devant les classes
// definies dans le namespace qglviewer.
using namespace qglviewer;
using namespace std;

PlanTexture::PlanTexture() {
    setRadius(1.0f);
}

void PlanTexture::init(float r, int ntheta, int nphi) {
    // Set the radius for openGL
    setRadius(r);
}


bool PlanTexture::intersect(const Ray& ray, Hit& hit, Frame global_frame) const {
    // Update the local -> global transformation matrix
    global_frame = MultiplyFrame(global_frame, frame_);

    // Center of the sphere
    Vec C = frame_.position();

    float b = 2 * ray.direction() * (ray.start() - C);
    float c = C * C + ray.start() * ray.start() - 2 * C * ray.start() - radius_*radius_;
    float delta = b * b - 4 * c;

    if (delta >= 0.0f) {
        float t = 0.5 * (-b - sqrt(delta));
        Vec P = ray.start() + t * ray.direction();

        hit.setIntersection(global_frame.inverseCoordinatesOf(P));
        hit.setNormal(global_frame.inverseTransformOf((P - C) / (P - C).norm()));
        hit.setMaterial(this->material_);
        hit.setTime(t);
        return true;
    }
    return false;
}


int PlanTexture::ImageLoad(char *filename,Image *image) const{
    FILE *file;
    unsigned long size;                 // size of the image in bytes.
    unsigned long i;                    // standard counter.
    unsigned short int planes;          // number of planes in image (must be 1) 
    unsigned short int bpp;             // number of bits per pixel (must be 24)
    char temp;                          // temporary color storage for bgr-rgb conversion.

    // make sure the file is there.
    if ((file = fopen(filename, "rb"))==NULL){
		printf("File Not Found : %s\n",filename);
		return 0;
    }
    
    // seek through the bmp header, up to the width/height:
    fseek(file, 18, SEEK_CUR);

    // read the width
    if ((i = fread(&image->sizeX, 4, 1, file)) != 1) {
		printf("Error reading width from %s.\n", filename);
		return 0;
    }
    
    // read the height 
    if ((i = fread(&image->sizeY, 4, 1, file)) != 1) {
		printf("Error reading height from %s.\n", filename);
		return 0;
    }
    
    // calculate the size (assuming 24 bits or 3 bytes per pixel).
    size = image->sizeX * image->sizeY * 3;

    // read the planes
    if ((fread(&planes, 2, 1, file)) != 1) {
		printf("Error reading planes from %s.\n", filename);
		return 0;
    }
    if (planes != 1) {
		printf("Planes from %s is not 1: %u\n", filename, planes);
		return 0;
    }

    // read the bpp
    if ((i = fread(&bpp, 2, 1, file)) != 1) {
		printf("Error reading bpp from %s.\n", filename);
		return 0;
    }
    if (bpp != 24) {
		printf("Bpp (bits per pixel) from %s should be 24. (It is %u now)\n", filename, bpp);
		return 0;
    }
	
    // seek past the rest of the bitmap header.
    fseek(file, 24, SEEK_CUR);

    // read the data. 
    image->data = (char *) malloc(size);
    if (image->data == NULL) {
		printf("Error allocating memory for color-corrected image data");
		return 0;	
    }

    if ((i = fread(image->data, size, 1, file)) != 1) {
		printf("Error reading image data from %s.\n", filename);
		return 0;
    }

    for (i=0;i<size;i+=3) { // reverse all of the colors. (bgr -> rgb)
		temp = image->data[i];
		image->data[i] = image->data[i+2];
		image->data[i+2] = temp;
    }

    return 1;
}

void PlanTexture::draw() const {
    // Apply the transform
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    {
    	glMultMatrixd(frame_.worldMatrix());

    	// Draw the planTexture
    	drawPlan();

    }
    // Go back to the father's frame
    glPopMatrix();
}

void PlanTexture::drawPlan() const{
	glColor3f(1.0,1.0,1.0);
	
	glEnable(GL_TEXTURE_2D);					// Enable Texture Mapping	

	glBindTexture(GL_TEXTURE_2D, texture);	// Select Our Texture. 
	
    glBegin(GL_QUADS);
		int i=0;
		glTexCoord2f(0.0f, 0.0f); 
		glVertex3f(positions[i].x, positions[i].y,  positions[i].z);	// Bottom Left Of The Texture and Quad
		glNormal3f(0.0, -1.0,  0.0);	
		i++;
		glTexCoord2f(1.0f, 0.0f); 
		glVertex3f(positions[i].x, positions[i].y,  positions[i].z);	// Bottom Right Of The Texture and Quad
		glNormal3f(0.0, -1.0,  0.0);	
		i++;
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(positions[i].x, positions[i].y,  positions[i].z);	// Top Right Of The Texture and Quad
		glNormal3f(0.0, -1.0,  0.0);	
		i++;
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(positions[i].x, positions[i].y,  positions[i].z);	// Top Left Of The Texture and Quad
		glNormal3f(0.0, -1.0,  0.0);	
	glEnd();			
}

// Load Bitmaps And Convert To Textures
void PlanTexture::LoadGLTextures(char *filename) const{	
    Image *image1;
    
    // allocate space for texture
    image1 = (Image *) malloc(sizeof(Image));
    if (image1 == NULL) {
		printf("Error allocating space for image");
		exit(0);
    }           
  
    // Create Textures	
 	GLuint text=texture;
  
    glGenTextures(1, &text);

	//printf("\nfilename: %s\n",filename);
    if (!ImageLoad(filename,image1)) 
		exit(1);         

    // nearest filtered texture
    glBindTexture(GL_TEXTURE_2D, texture);   // 2d texture (x and y size)
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST); // scale cheaply when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST); // scale cheaply when image smalled than texture
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image1->sizeX, image1->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image1->data);
};


/* Parse the XML for elements of the PlanTexture
 */
void PlanTexture::initFromDOMElement(const QDomElement& e) {
    Object::initFromDOMElement(e);

	texture= e.attribute("index", "1").toInt();
	const char *filename_const;
	filename_const = e.attribute("filepathname", "");
	LoadGLTextures((char*) filename_const);		// Jump To Texture Loading Routine 	
	float bottomleft_x  = e.attribute("bottomleft_x", "0.0").toFloat();
	float bottomleft_y  = e.attribute("bottomleft_y", "0.0").toFloat();
	float bottomleft_z  = e.attribute("bottomleft_z", "0.0").toFloat();
	float bottomright_x = e.attribute("bottomright_x", "0.0").toFloat();
	float bottomright_y = e.attribute("bottomright_y", "0.0").toFloat();
	float bottomright_z = e.attribute("bottomright_z", "0.0").toFloat();
	float topright_x    = e.attribute("topright_x", "0.0").toFloat();
	float topright_y    = e.attribute("topright_y", "0.0").toFloat();
	float topright_z    = e.attribute("topright_z", "0.0").toFloat();
	float topleft_x     = e.attribute("topleft_x", "0.0").toFloat();
	float topleft_y     = e.attribute("topleft_y", "0.0").toFloat();
	float topleft_z     = e.attribute("topleft_z", "0.0").toFloat();

	positions.push_back(Vec(bottomleft_x,bottomleft_y,bottomleft_z));
	positions.push_back(Vec(bottomright_x,bottomright_y,bottomright_z));
	positions.push_back(Vec(topright_x,topright_y,topright_z));
	positions.push_back(Vec(topleft_x,topleft_y,topleft_z));

}
