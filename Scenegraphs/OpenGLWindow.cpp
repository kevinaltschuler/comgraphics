#include "OpenGLWindow.h"
#include <QScreen>
#include <OpenGLFunctions.h>
#include <QMessageBox> //requires QT += widgets in .pro file
#include <QPainter>
#include <QDebug>
#include <QStaticText>
#include <fstream>
#include <iostream>




OpenGLWindow::OpenGLWindow(QWindow *parent, int argc, char* argv[])
    :QOpenGLWindow(UpdateBehavior::NoPartialUpdate,parent)
{
    //changes the title shown on the window
    this->setTitle("Scenegraphs!");
    //resize the window to (400,400). This will result in a call to resizeGL
    this->resize(500,500);

    //make sure we have OpenGL 3.3 (major.minor), with 16-bit buffers
    QSurfaceFormat format;
    format.setDepthBufferSize(16);
    format.setStencilBufferSize(8);
    format.setMajorVersion(3);
    format.setMinorVersion(3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    this->setFormat(format);

    isDragged = false;

    frames = 0;
    setAnimating(true);

    if (argv[1]) {
        std::ifstream input(argv[1]);
        int i = 0;
        std::vector<std::string> lines;
        for( std::string line; getline(input, line); i++)
        {
            lines.push_back(line);
        }

         xmlfilename = lines[0];

         // sending eye pos
         stringstream eye(lines[1]);
         float e1; eye >> e1;
         float e2; eye >> e2;
         float e3; eye >> e3;
         // sending cen pos
         stringstream cen(lines[2]);
         float c1; cen >> c1;
         float c2; cen >> c2;
         float c3; cen >> c3;
         // sending up dir
         stringstream up(lines[3]);
         float u1; up >> u1;
         float u2; up >> u2;
         float u3; up >> u3;
         view.setCamera(glm::vec3(e1, e2, e3), glm::vec3(c1, c2, c3), glm::vec3(u1, u2, u3));
    } else {
        xmlfilename = "scenegraphmodels/draft.xml";
    }

    // format for the config file is
    // 0: path to xml file
    // 1: eye pos, ex: 0.0 50.0 80.0
    // 2: center pos, ex: 0.0 50.0 0.0
    // 3: up dir, ex: 0.0 1.0 0.0

}


OpenGLWindow::~OpenGLWindow()
{
    //When this window is called, we must release all opengl resources
    view.dispose(*gl);
    if (gl!=NULL)
      delete gl;
}

void OpenGLWindow::initializeGL()
{
    //create the opengl function wrapper class
    gl = new util::OpenGLFunctions();
    try
    {
        view.init(*gl);
    }
    catch (exception& e)
    {
        //if something goes wrong, show an error message in a popup.
        //look at the comment in the #include above for this to work correctly
        QMessageBox msgBox;
        msgBox.setText(e.what());
        msgBox.exec();
        //assuming we cannot recover from this error, shut down the application
        exit(1);
    }
    view.initScenegraph(*gl, xmlfilename);
}

void OpenGLWindow::paintGL()
{
    //simply delegate to the view's draw
    view.draw(*gl);

    //measure frame rate

    if (this->frames==0)
    {
        this->timer.start();
    }
    this->frames++;
    if (this->frames>100)
    {
        framerate = this->frames/((float)this->timer.restart()/1000.0f);
        this->frames = 0;
    }

    //display frame rate as text

    QPainter painter(this);
   // painter.fillRect(0, 0, width(), height(), Qt::white);
    painter.setPen(QColor(255, 0, 0));
    painter.setFont(QFont("Sans", 12));
    QStaticText text(QString("Frame rate: %1 fps").arg(framerate));
    painter.drawStaticText(5, 20, text);

}



void OpenGLWindow::resizeGL(int w,int h)
{
    //simply delegate to the view's reshape
    view.reshape(*gl,w,h);
}

void OpenGLWindow::mousePressEvent(QMouseEvent *e)
{
    isDragged = true;
    view.mousePressed(e->x(),e->y());
}


void OpenGLWindow::mouseMoveEvent(QMouseEvent *e)
{
    //for now, simply re-render if it is a drag
    if (isDragged)
    {
        view.mouseDragged(e->x(),e->y());
        this->update();
    }

}

void OpenGLWindow::mouseReleaseEvent(QMouseEvent *e)
{
    isDragged = false;
    view.mouseReleased(e->x(),e->y());
}


 void OpenGLWindow::keyPressEvent(QKeyEvent *e)
 {
     if(e->key() == Qt::Key_T){
         printf("you have pressed T \n");
         view.keySwitch(1);
         this->update();
     }

     if(e->key() == Qt::Key_G){
         printf("you have pressed G \n");
         view.keySwitch(2);
         this->update();
     }

 }



/*
 * This function helps us to automatically start animating
 * When we call this function with "true", it sets up the window so
 * that it calls update() again and again automatically
 */

void OpenGLWindow::setAnimating(bool enabled)
{
    if (enabled) {
        // Animate continuously, throttled by the blocking swapBuffers() call the
        // QOpenGLWindow internally executes after each paint. Once that is done
        // (frameSwapped signal is emitted), we schedule a new update. This
        // obviously assumes that the swap interval (see
        // QSurfaceFormat::setSwapInterval()) is non-zero.
        connect(this, SIGNAL(frameSwapped()), this, SLOT(update()));
        update();
    } else {
        disconnect(this, SIGNAL(frameSwapped()), this, SLOT(update()));
    }
}




