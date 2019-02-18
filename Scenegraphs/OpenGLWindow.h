#ifndef OPENGLWINDOW_H
#define OPENGLWINDOW_H


#include <QtGui/QOpenGLWindow>
#include <QCoreApplication>
#include <QMouseEvent>
#include <OpenGLFunctions.h>
#include <QOpenGLShaderProgram>
#include <QTime>
#include "View.h"

/*
 * This is the main OpenGL-based window in our application
 * The QOpenGLWindow class exposes some convenient functionality for any OpenGL program to override
 *
*/

class OpenGLWindow : public QOpenGLWindow
{
  Q_OBJECT
public:
  explicit OpenGLWindow(QWindow *parent=0);
  virtual ~OpenGLWindow();
  void setAnimating(bool enabled);

protected:
  /*
         * this function will be called when the window is being initialized
         * (it will be called only once)
         */
  void initializeGL();

  /*
         * this function is called every time the window is drawing itself.
         * To refresh the window, call update()
         */
  void paintGL();

  //     void paintOverGL();

  /*
         * this function is called whenever the window resizes itself
         * (manually or due to calling resize)
         */
  void resizeGL(int w,int h);

  /*
                 * this function is called whenever the mouse is moved (even hovered).
                 */
  void mouseMoveEvent(QMouseEvent *);

  /*
                 * this function is called whenever a mouse button is pressed.
                 */
  void mousePressEvent(QMouseEvent *);

  /*
                 * this function is called whenever a mouse button is released.
                 */
  void mouseReleaseEvent(QMouseEvent *);


  /*
         * this class encapsulates all our program-specific rendering.
         * This provides a clean divide from Qt-specific window setup and
         * events and our OpenGL stuff. Thus we would need to mostly change
         * the View class from one application to another. That is, to create a
         * new application, retain this class as-is and change the View class
         */
  View view;

  /*
         * normally OpenGL is for the C language (i.e. no objects). Thus all
         * Opengl functions can be called directly provided we include
         * the header files.

         * With newer modern versions of opengl, this gets messy. This is
         * because (a) we must have the correct version of the header file that
         * has all the functions in a particular OpenGL version. (b) this correct
         * file must be included at every place where we are using OpenGL
         * functions. For this simple application, that ends up being here,
         * View, ObjectInstance classes. To change the OpenGL version for any
         * reasons, all these files must be edited.
         *
         * Therefore we wrap all the OpenGL functionality in an object, which
         * we pass around. Qt has several wrapper classes, one for each version
         * of OpenGL (and even OpenGL ES). In order to make our code even
         * more independent of Qt, we create a new class OpenGLFunctions. For
         * now we make it extend the QOpenGLFunctions_3_3_Core, which
         * encapsulates all the OpenGL 3.3 core functionality.
         *
         * To change the version or even to use with a different library,
         * simply change the OpenGLFunctions class to have the correct parent
         */

  util::OpenGLFunctions *gl;

private:
  bool isDragged;
  int frames;
  QTime timer;
  float framerate;
};



#endif // OPENGLWINDOW_H
