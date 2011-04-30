#include "material.h"
#include <stdlib.h>
#include <qmessagebox.h>

Material::Material()
{
  setSpecularCoefficient(0.0);
  setTextureScale(1.0);
}

void Material::initFromDOMElement(const QDomElement& e)
{
  if (e.tagName() != "Material")
    {
      QMessageBox::critical(NULL, "Material init error", "Material::initFromDOMElement, bad DOM tagName.\nExpecting 'Material', got "+e.tagName());
      return;
    }

  QDomNode n = e.firstChild();
  while (!n.isNull())
    {
      QDomElement e = n.toElement();
      if (!e.isNull())
	{
	  if (e.tagName() == "DiffuseColor")
	    diffuseColor_.initFromDOMElement(e);
	  else
	    if (e.tagName() == "SpecularColor")
	      {
		specularColor_.initFromDOMElement(e);
		if (!e.hasAttribute("exponent"))
		  QMessageBox::warning(NULL, "Material error", "No Material specular coefficient provided. Using 0.0 instead");
		setSpecularCoefficient(e.attribute("exponent", "0.0").toFloat());
	      }
	    else
	      if (e.tagName() == "ReflectiveColor")
		reflectiveColor_.initFromDOMElement(e);
	      else
		if (e.tagName() == "Texture")
		  {
		    setTextureScale(e.attribute("scale", "1.0").toFloat());
		    if (e.hasAttribute("scaleU"))
		      setTextureScaleU(e.attribute("scaleU", "1.0").toFloat());
		    if (e.hasAttribute("scaleV"))
		      setTextureScaleV(e.attribute("scaleV", "1.0").toFloat());
		    const char *filename =e.attribute("file"); 
		    loadTextureFromFile((char *)filename);
		    if (e.attribute("mode") == "MODULATE") setTextureMode(MODULATE);
		    if (e.attribute("mode") == "BLEND")    setTextureMode(BLEND);
		    if (e.attribute("mode") == "REPLACE")  setTextureMode(REPLACE);
		  }
		else
		  QMessageBox::warning(NULL, "Material child error", "Unsupported Material child : "+e.tagName());
	}
      else
	QMessageBox::warning(NULL, "Material XML error", "Error while parsing Material XML document");

      n = n.nextSibling();
    }
}

//texture
int Material::ImageLoad(char *filename, Image *image) {
    FILE *file;
    unsigned long size;                 // size of the image in bytes.
    unsigned long i;                    // standard counter.
    unsigned short int planes;          // number of planes in image (must be 1) 
    unsigned short int bpp;             // number of bits per pixel (must be 24)
    char temp;                          // temporary color storage for bgr-rgb conversion.

    // make sure the file is there.
    if ((file = fopen(filename, "rb"))==NULL)
    {
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
    printf("Width of %s: %lu\n", filename, image->sizeX);
    
    // read the height 
    if ((i = fread(&image->sizeY, 4, 1, file)) != 1) {
	printf("Error reading height from %s.\n", filename);
	return 0;
    }
    printf("Height of %s: %lu\n", filename, image->sizeY);
    
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
	printf("Bpp from %s is not 24: %u\n", filename, bpp);
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
    
    // we're done.
    return 1;
}

// Load Bitmaps And Convert To Textures
void Material::LoadGLTextures(char *filename) 
{	
    // Load Texture
    Image *image1;
    
    // allocate space for texture
    image1 = (Image *) malloc(sizeof(Image));
    if (image1 == NULL) {
	printf("Error allocating space for image");
	exit(0);
    }

    if (!ImageLoad(filename/*"Data/lesson10/Seabed-Texture-651366.bmp"*/, image1)) {
	exit(1);
    }        
  
    // Create Textures	
    glGenTextures(1, &texture_vet[0]);

    // nearest filtered texture
    glBindTexture(GL_TEXTURE_2D, texture_vet[0]);   // 2d texture (x and y size)
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST); // scale cheaply when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST); // scale cheaply when image smalled than texture
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image1->sizeX, image1->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image1->data);
};

void Material::loadTextureFromFile(char *filename)
{
  /*if (filename.isNull())
    QMessageBox::warning(NULL, "Material texture error", "Material error : no texture file name provided");
  else{
    //if (!texture_.load(fileName))
      //QMessageBox::warning(NULL, "Material texture error", "Unable to load Material texture from "+filename);
*/
	//texture
    //LoadGLTextures(filename);							// Jump To Texture Loading Routine ( NEW )
	//glEnable(GL_TEXTURE_2D);						// Enable Texture Mapping ( NEW )     
  //}
}

Color Material::diffuseColor(float u, float v) const
{
  if (texture().isNull())
    return diffuseColor();

  // Take texture scaling into account
  u /= textureScaleU();
  v /= textureScaleV();

  // Compute pixel coordinates
  const int i = int(fabs(u - floor(u)) * texture().width());
  const int j = int(fabs(v - floor(v)) * texture().height());

  // Compute color
  switch(textureMode())
    {
    case MODULATE : return Color(texture().pixel(i,j)) * diffuseColor();
    case BLEND    :
      {
	const float alpha = qAlpha(texture().pixel(i,j)) / 255.0;
	return alpha*Color(texture().pixel(i,j)) + (1.0f-alpha)*diffuseColor();
      }
    case REPLACE  : return Color(texture().pixel(i,j));
    }
  return Color();
}
