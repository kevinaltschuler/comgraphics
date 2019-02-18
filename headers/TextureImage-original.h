#ifndef _TEXTUREIMAGE_H_
#define _TEXTUREIMAGE_H_

#include <QtGui/QImage>
#include <QOpenGLTexture>

namespace util
{	
  /**
         A class that represents an image. Provides a function for bilinear interpolation
         */
  class TextureImage
  {
  protected:
    QImage *image;
    QOpenGLTexture *texture;
    string name;
  protected:
  void deleteImage()
  {

    if (image)
      delete image;
  }

  public:
    TextureImage(string filepath,string name) throw(runtime_error)
    {
      image = new QImage(QString(filepath.c_str()));

      if (image->isNull())
        throw runtime_error("Image cannot be loaded!");

      texture = new QOpenGLTexture(image->mirrored());

    }

    QOpenGLTexture *getTexture()
    {
      return texture;
    }

    string getName()
    {
      return name;
    }

    glm::vec4 getColor(float x,float y)
    {
      int x1,y1,x2,y2;

      x = x - (int)x; //GL_REPEAT
      y = y - (int)y; //GL_REPEAT

      x1 = (int)(x*image->width());
      y1 = (int)(y*image->height());

      x1 = (x1 + image->width())%image->width();
      y1 = (y1 + image->height())%image->height();

      x2 = x1+1;
      y2 = y1+1;

      if (x2>=image->width())
        x2 = image->width()-1;

      if (y2>=image->height())
        y2 = image->height()-1;

      glm::vec4 one = ColorToVector4f(image->pixel(x1,y1));
      glm::vec4 two = ColorToVector4f(image->pixel(x2,y1));
      glm::vec4 three = ColorToVector4f(image->pixel(x1,y2));
      glm::vec4 four = ColorToVector4f(image->pixel(x2,y2));

      glm::vec4 inter1 = glm::mix(one,three,y-(int)y);
      glm::vec4 inter2 = glm::mix(two,four,y-(int)y);
      glm::vec4 inter3 = glm::mix(inter1,inter2,x-(int)x);

      return inter3;
    }

  private:
    glm::vec4 ColorToVector4f(QRgb c)
    {
    return glm::vec4((float)qRed(c)/255,(float)qGreen(c)/255,(float)qBlue(c)/255,(float)qAlpha(c)/255);
  }




  };
}

#endif
