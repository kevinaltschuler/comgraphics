#ifndef VIEW_H
#define VIEW_H

#include "OpenGLFunctions.h"
#include <exception>
#include <glm/glm.hpp>
#include "ShaderProgram.h"
#include "sgraph/Scenegraph.h"
#include "ShaderLocationsVault.h"
#include "ObjectInstance.h"
#include "VertexAttrib.h"
#include "sgraph/GLScenegraphRenderer.h"
#include <stack>
#include <QKeyEvent>

using namespace std;

/*
 * This class encapsulates all our program-specific details. This makes our
 * design better if we wish to port it to another C++-based windowing
 * library
 */

class View
{
    class LightLocation
    {
    public:
      int ambient,diffuse,specular,position;
      LightLocation()
      {
        ambient = diffuse = specular = position = -1;
      }

    };
public:
    View();
    ~View();
    /*
     * This is called when the application is being initialized. We should
     * do all our initializations here. This is also the first function where
     * OpenGL commands will work (i.e. don't do any OpenGL related stuff in the
     * constructor!)
     */
    void init(util::OpenGLFunctions& e) throw(runtime_error);

    void initScenegraph(util::OpenGLFunctions& e,const string& in) throw(runtime_error);

    void initCameraObjScenegraph(util::OpenGLFunctions &gl, const string& filename) throw(runtime_error);

    /*
     * This function is called whenever the window is to be redrawn
     */
    void draw(util::OpenGLFunctions& e);

    /*
     * This function is called whenever the window is reshaped
     */
    void reshape(util::OpenGLFunctions& gl,int width,int height);

    /*
     * This function is called whenever the window is being destroyed
     */
    void dispose(util::OpenGLFunctions& gl);

    void mousePressed(int x,int y);
    void mouseReleased(int x,int y);
    void mouseDragged(int x,int y);
    void keySwitch(int c);

    void switchCamera();

    void onKeyPressed(int key);

    //takes a string in my config format and sets the cameras initial position
    void setCamera(glm::vec3 e, glm::vec3 c, glm::vec3 u);

    void addToCamera(glm::vec3 e, glm::vec3 c, glm::vec3 u);

    void raytrace(int w, int h, stack<glm::mat4> stack);

private:
    int time;
    //record the current window width and height
    int WINDOW_WIDTH,WINDOW_HEIGHT;
    //the projection matrix
    glm::mat4 proj;
    //the trackball transform
    glm::mat4 trackballTransform;
    //the radius of the virtual trackball
    float trackballRadius;
    //the mouse position
    glm::vec2 mousePos;
    //the modelview matrix
    stack<glm::mat4> modelview;
    //the camera objs modelview matrix
    stack<glm::mat4> cameramodelview;
    //the scene graph
    sgraph::Scenegraph *scenegraph;
    //the scene graph
    sgraph::Scenegraph *cameraScenegraph;
    //the list of shader variables and their locations within the shader program
    util::ShaderLocationsVault shaderLocations;
    //the GLSL shader
    util::ShaderProgram program;
    sgraph::GLScenegraphRenderer renderer;

    bool fixedCamera = false;

    // 1 for trackball camera
    // 2 for fixed;
    int camera = 1;

    glm::vec3 eye;
    glm::vec3 center;
    glm::vec3 up;

    int zoom = 0;

    bool renderCamera= false;

    bool rayTrace = false;
};

#endif // VIEW_H
