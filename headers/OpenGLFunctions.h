#ifndef _OPENGLFUNCTIONS_H_
#define _OPENGLFUNCTIONS_H_

#include <QOpenGLFunctions_3_3_Core>

namespace util
{

/*
 * Wrapper class for Qt opengl Functions. This is so that it is easier to
 * change version of opengl to be supported.
 * Just use the appropriate Qt profile above and below
 */
class OpenGLFunctions:public QOpenGLFunctions_3_3_Core
{
public:
    OpenGLFunctions()
    {
        initializeOpenGLFunctions();
    }
};
}

#endif
