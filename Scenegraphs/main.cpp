#include <QApplication>
#include "openglwindow.h"

int main(int argc, char *argv[])
{
    //create a main application within which this window will be
   QApplication app(argc, argv);

   //add our window to this application
   OpenGLWindow window;

   //show this window
   window.show();

   //give control to the application
   return app.exec();
}
