#include "View.h"
#include "PolygonMesh.h"
#include "sgraph/ScenegraphInfo.h"
#include "sgraph/SceneXMLReader.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <map>
#include <string>
#include <math.h>       /* tan */
#define PI 3.14159265359
using namespace std;

View::View()
{   
  WINDOW_WIDTH = WINDOW_HEIGHT = 0;
  trackballRadius = 300;
  trackballTransform = glm::mat4(1.0);
  proj = glm::mat4(1.0);
  scenegraph = NULL;
  cameraScenegraph = NULL;
  time = 0;
  eye = glm::vec3(0.0, 50.0, 80.0);
  center = glm::vec3(0.0, 50.0, 0.0);
  up = glm::vec3(0.0, 1.0, 0.0);
  zoom = 0;
  renderCamera = false;
  rayTrace = false;
}

View::~View()
{
  if (scenegraph!=NULL)
    delete scenegraph;
}

void View::initScenegraph(util::OpenGLFunctions &gl, const string& filename) throw(runtime_error)
{
  if (scenegraph!=NULL)
    delete scenegraph;

  program.enable(gl);
  sgraph::ScenegraphInfo<VertexAttrib> sinfo;
  sinfo = sgraph::SceneXMLReader::importScenegraph<VertexAttrib>(filename);
  scenegraph = sinfo.scenegraph;

  renderer.setContext(&gl);
  map<string,string> shaderVarsToVertexAttribs;
  shaderVarsToVertexAttribs["vPosition"] = "position";
  shaderVarsToVertexAttribs["vNormal"] = "normal";
  shaderVarsToVertexAttribs["vTexCoord"] = "texcoord";
  renderer.initShaderProgram(program,shaderVarsToVertexAttribs);
  scenegraph->setRenderer<VertexAttrib>(&renderer,sinfo.meshes);
  program.disable(gl);

}


void View::initCameraObjScenegraph(util::OpenGLFunctions &gl, const string& filename) throw(runtime_error)
{
  if (cameraScenegraph!=NULL)
    delete cameraScenegraph;

  if (renderCamera) {
      program.enable(gl);
      sgraph::ScenegraphInfo<VertexAttrib> sinfo;
      sinfo = sgraph::SceneXMLReader::importScenegraph<VertexAttrib>(filename);
      cameraScenegraph = sinfo.scenegraph;
      cameraScenegraph->setRenderer<VertexAttrib>(&renderer,sinfo.meshes);
      program.disable(gl);
  }

}

void View::init(util::OpenGLFunctions& gl) throw(runtime_error)
{
  //do this if your initialization throws an error (e.g. shader not found,
  //some model not found, etc.
  //  throw runtime_error("Some error happened!");

  //create the shader program
  program.createProgram(gl,
                        string("shaders/phong-multiple.vert"),
                        string("shaders/phong-multiple.frag"));

  //assuming it got created, get all the shader variables that it uses
  //so we can initialize them at some point
  shaderLocations = program.getAllShaderVariables(gl);
}

void View::draw(util::OpenGLFunctions& gl) {
  gl.glClearColor(0,0,0,1);
  gl.glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  gl.glEnable(GL_DEPTH_TEST);
  gl.glEnable(GL_TEXTURE_2D);

  if (scenegraph==NULL)
    return;

  program.enable(gl);

  while (!modelview.empty())
    modelview.pop();
  while (!cameramodelview.empty() && renderCamera)
    cameramodelview.pop();

  time += 1;
  if (time > 3600) {
      time = 0;
  }

  /*
         *In order to change the shape of this triangle, we can either move the vertex positions above, or "transform" them
         * We use a modelview matrix to store the transformations to be applied to our triangle.
         * Right now this matrix is identity, which means "no transformations"
         */
  modelview.push(glm::mat4(1.0));


  modelview.top() = modelview.top() *
          glm::lookAt(glm::vec3(0.0, 0.0, 0.0),
                glm::vec3(0.0, 0.0, -2.0),
                glm::vec3(0.0, 1.0, 0.0));// * trackballTransform;

  if (rayTrace) {
      printf("raytrace\n");
      raytrace(WINDOW_WIDTH, WINDOW_HEIGHT, modelview);
      rayTrace = false;
  } else {

      /*
        *Supply the shader with all the matrices it expects.
        */
      gl.glUniformMatrix4fv(shaderLocations.getLocation("projection"),
                            1,
                            false,
                            glm::value_ptr(proj));

      scenegraph->draw(modelview);

      gl.glFlush();

      program.disable(gl);
  }
}

void View::raytrace(int w, int h, stack<glm::mat4> stack) {
    glm::vec3 colors[w][h];

    for (int i = 0; i < w; i++) {
      for (int j = 0; j < h; j++) {
        //_3DRay ray = calculate the ray to be cast here;
         _3DRay ray = _3DRay(glm::vec4(0,0,0,1), glm::vec4(i - w / 2, j - h / 2,
                                       (float) ((-0.5f * h) / 1.73205080757), 0));
        colors[i][j] = scenegraph->raycast(ray, stack);
//        if (colors[i][j].x != 0.0f && colors[i][j].y != 0.0f && colors[i][j].z != 0.0f)
//            printf("%f %f %f \n", colors[i][j].x, colors[i][j].y, colors[i][j].z);
      }
    }

    for (int i = 0; i < w; i++) {
      for (int j = 0; j < h; j++) {
        printf("%f %f %f \n", colors[i][j].x, colors[i][j].y, colors[i][j].z);
      }
    }
}


void View::switchCamera() {
    fixedCamera = !fixedCamera;
}


void View::mousePressed(int x,int y)
{
  mousePos = glm::vec2(x,y);
}

void View::mouseReleased(int x,int y)
{

}

void View::mouseDragged(int x,int y)
{
  glm::vec2 newM = glm::vec2((float)x,(float)y);

  glm::vec2 delta = glm::vec2((float)(newM.x-mousePos.x),(float)(newM.y-mousePos.y));
  mousePos = newM;

  trackballTransform =
      glm::rotate(glm::mat4(1.0),delta.x/trackballRadius,glm::vec3(0.0f,1.0f,0.0f)) *
      glm::rotate(glm::mat4(1.0),delta.y/trackballRadius,glm::vec3(1.0f,0.0f,0.0f)) *
      trackballTransform;
}

void View::reshape(util::OpenGLFunctions& gl,int width,int height)
{
  //record the new width and height
  WINDOW_WIDTH = width;
  WINDOW_HEIGHT = height;

  /*
     * The viewport is the portion of the screen window where the drawing
     * would be placed. We want it to take up the entire area of the window
     * so we set the viewport to be the entire window.
     * Look at documentation of glViewport
     */

  gl.glViewport(0, 0, width, height);

  /*
     * This sets up the part of our virtual world that will be visible in
     * the screen window. Since this program is drawing 2D, the virtual world
     * is 2D. Thus this window can be specified in terms of a rectangle
     * Look at the documentation of glOrtho2D, which glm::ortho implements
     */

  proj = glm::perspective(glm::radians(120.0f),(float)width/height,0.1f,10000.0f);

}

void View::setCamera(glm::vec3 e, glm::vec3 c, glm::vec3 u) {
    eye = e;
    center = c;
    up = u;
}

void View::addToCamera(glm::vec3 e, glm::vec3 c, glm::vec3 u) {
    eye = glm::vec3(eye.x + e.x, eye.y + e.y, eye.z + e.z);
    center = glm::vec3(center.x + c.x, center.y + c.y, center.z + c.z);
    up = glm::vec3(up.x + u.x, up.y + u.y, up.z + u.z);
}

void View::onKeyPressed(int key) {

    if(key == Qt::Key_W){
        addToCamera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    }

    if(key == Qt::Key_A){
        addToCamera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    }

    if(key == Qt::Key_S){
        addToCamera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    }

    if(key == Qt::Key_D){
        addToCamera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    }

    if(key == Qt::Key_Up){
        addToCamera(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    }

    if(key == Qt::Key_Down){
        addToCamera(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    }

    if(key == Qt::Key_Left){
        addToCamera(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    }

    if(key == Qt::Key_Right){
        addToCamera(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    }

    if(key == Qt::Key_F){
        addToCamera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.5f, 0.0f, 0.0f));
    }

    if(key == Qt::Key_C){
        addToCamera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-0.5f, 0.0f, 0.0f));
    }

    if(key == Qt::Key_Plus){
        zoom -= 1;
    }

    if(key == Qt::Key_Minus){
        zoom += 1;
    }

    if(key == Qt::Key_R){
        rayTrace = true;
    }

}

void View::dispose(util::OpenGLFunctions& gl)
{
  //clean up the OpenGL resources used by the object
  scenegraph->dispose();
  renderer.dispose();
  //release the shader resources
  program.releaseShaders(gl);
}
