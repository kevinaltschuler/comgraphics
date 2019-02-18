#ifndef _OBJECTINSTANCE_H_
#define _OBJECTINSTANCE_H_

#include "PolygonMesh.h"
#include <string>
using namespace std;
#include "OpenGLFunctions.h"
#include "ShaderProgram.h"
#include "ShaderLocationsVault.h"
#include <glm/glm.hpp>

namespace util 
{	

  /*
 * This class represents an "object" to be drawn. The object's geometry is
 * represented in an underlying PolygonMesh object.
 *
 * This class encapsulates within it all the details of drawing the mesh using
 * OpenGL 3.x. This includes the vertex array object, vertex buffer objects, etc.
 *
 *
 */

  class ObjectInstance
  {
    /*
     * Create a blank ObjectInstance with a name
     * \param name a name of the object
     */

  public:
    ObjectInstance(const string& name)
    {
      //set the name
      setName(name);


    }
    ~ObjectInstance(){}

    template <class K>
    void initPolygonMesh(OpenGLFunctions& gl,
                         ShaderProgram& program,
                         const ShaderLocationsVault& shaderLocations,
                         const map<string,string>& shaderVarsToAttributeNames,
                         const PolygonMesh<K>& mesh) ;
    template <class K>
    void initPolygonMesh(OpenGLFunctions& gl,
                         const ShaderLocationsVault& shaderLocations,
                         const map<string,string>& shaderVarsToAttributeNames,
                         const PolygonMesh<K>& mesh) ;
    inline void draw(OpenGLFunctions& gl) const;
    inline void setName(string name);
    inline string getName() const;
    inline glm::vec4 getMinimumBounds() const;
    inline glm::vec4 getMaximumBounds() const;
    inline void cleanup(OpenGLFunctions& gl);
  private:
    inline void initVertexObjects(OpenGLFunctions& gl);

  protected:
    GLuint vao; //our VAO
    GLuint vbo[2];//one VBO for vertex data, one VBO for index data
    string name; //a unique "name" for this object
    unsigned int primitiveType;
    unsigned int primitiveCount;
  };




  void ObjectInstance::initVertexObjects(OpenGLFunctions &gl)
  {
    //create the VAO ID for this object
    gl.glGenVertexArrays(1,&vao);
    //bind the VAO
    gl.glBindVertexArray(vao);
    //create as many VBO IDs as you need
    gl.glGenBuffers(2, vbo);
  }




  /*
 * A helper method that sets this object up for rendering
 * \param program the shader program to be used to render this object
 * \param shaderLocations the locations of various shader variables relevant
 *        to this object
 * \param shaderVarsToAttributeNames a mapping of
 *        shader variable -> vertex attributes in the underlying mesh
 * \param mesh the underlying polygon mesh
 */
  template<class K>
  void ObjectInstance::initPolygonMesh(OpenGLFunctions& gl,
                                       ShaderProgram& program,
                                       const ShaderLocationsVault& shaderLocations,
                                       const map<string,string>& shaderVarsToAttributeNames,
                                       const PolygonMesh<K>& mesh)
  {
    unsigned int i,j;

    initVertexObjects(gl);


    primitiveType = mesh.getPrimitiveType();
    primitiveCount = mesh.getPrimitiveCount();
    //get a list of all the vertex attributes from the mesh
    vector<K> vertexDataList = mesh.getVertexAttributes();
    vector<unsigned int> primitives = mesh.getPrimitives();


    //No need to create buffers in C++!

    int sizeOfOneVertex=0;
    map<string,int> offsets;

    for (map<string,string>::const_iterator it=shaderVarsToAttributeNames.cbegin();it!=shaderVarsToAttributeNames.cend();it++)
      {
        offsets[it->second] = sizeOfOneVertex;
        sizeOfOneVertex += vertexDataList[0].getData(it->second).size();
      }

    int stride;

    if (shaderVarsToAttributeNames.size()>1)
      stride = sizeOfOneVertex;
    else
      stride = 0;


    vector<float> vertexDataAsFloats;
    vector<float> data;

    for (i=0;i<vertexDataList.size();i++)
      {
        for (map<string,string>::const_iterator e = shaderVarsToAttributeNames.cbegin();e!=shaderVarsToAttributeNames.cend();e++)
          {
            data.clear();
            data = vertexDataList[i].getData(e->second);
            for (j=0;j<data.size();j++)
              {
                vertexDataAsFloats.push_back(data[j]);
              }
          }
      }





    /*
     * Bind the VAO as the current VAO, so that all subsequent commands affect it
     */
    gl.glBindVertexArray(vao);

    //enable the program
    program.enable(gl);

    //copy all the data to the vbo[0]
    gl.glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    gl.glBufferData(GL_ARRAY_BUFFER,
                    sizeof(float) * vertexDataAsFloats.size(),
                    &vertexDataAsFloats[0],
        GL_STATIC_DRAW);



    /*
     * go through all variables and enable that
     * attribute.
     */


    for (map<string,string>::const_iterator it=shaderVarsToAttributeNames.cbegin();
         it!=shaderVarsToAttributeNames.cend();
         it++)
      {
        /*
         * e.key: the name of the shader variable
         * e.value: the name of the corresponding vertex attribute in polygon mesh
         */


        int shaderLocation = shaderLocations.getLocation(it->first);

        if (shaderLocation>=0)
          {
            //tell opengl how to interpret the above data
            gl.glVertexAttribPointer(shaderLocation,
                                     vertexDataList[0].getData(it->second).size(),
                GL_FLOAT,
                GL_FALSE,
                sizeof(float) * stride,
                (void *)(sizeof(float) * offsets[it->second]));
            //enable this attribute so that when rendered, this is sent to the vertex shader
            gl.glEnableVertexAttribArray(shaderLocation);
          }
      }





    /*
     * Allocate the VBO for triangle indices and send it to GPU
     */
    gl.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
    gl.glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                    primitives.size()*sizeof(GLuint),
                    &primitives[0],
        GL_STATIC_DRAW);

    gl.glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);


    /*
     * Unbind the VAO to prevent accidental change to all the settings
     * so at this point, this VAO has two VBOs and two enabled VertexAttribPointers.
     * It is going to remember all of that!
     */
    gl.glBindVertexArray(0);
    program.disable(gl);
  }

  /*
 * A helper method that sets this object up for rendering
 * \param program the shader program to be used to render this object
 * \param shaderLocations the locations of various shader variables relevant
 *        to this object
 * \param shaderVarsToAttributeNames a mapping of
 *        shader variable -> vertex attributes in the underlying mesh
 * \param mesh the underlying polygon mesh
 */
  template<class K>
  void ObjectInstance::initPolygonMesh(OpenGLFunctions& gl,
                                       const ShaderLocationsVault& shaderLocations,
                                       const map<string,string>& shaderVarsToAttributeNames,
                                       const PolygonMesh<K>& mesh)
  {
    unsigned int i,j;

    initVertexObjects(gl);

    primitiveType = mesh.getPrimitiveType();
    primitiveCount = mesh.getPrimitiveCount();
    //get a list of all the vertex attributes from the mesh
    vector<K> vertexDataList = mesh.getVertexAttributes();
    vector<unsigned int> primitives = mesh.getPrimitives();


    //No need to create buffers in C++!

    int sizeOfOneVertex=0;
    map<string,int> offsets;

    for (map<string,string>::const_iterator it=shaderVarsToAttributeNames.cbegin();it!=shaderVarsToAttributeNames.cend();it++)
      {
        offsets[it->second] = sizeOfOneVertex;
        sizeOfOneVertex += vertexDataList[0].getData(it->second).size();
      }

    int stride;

    if (shaderVarsToAttributeNames.size()>1)
      stride = sizeOfOneVertex;
    else
      stride = 0;


    vector<float> vertexDataAsFloats;
    vector<float> data;

    for (i=0;i<vertexDataList.size();i++)
      {
        for (map<string,string>::const_iterator e = shaderVarsToAttributeNames.cbegin();e!=shaderVarsToAttributeNames.cend();e++)
          {
            data.clear();
            data = vertexDataList[i].getData(e->second);
            for (j=0;j<data.size();j++)
              {
                vertexDataAsFloats.push_back(data[j]);
              }
          }
      }





    /*
     * Bind the VAO as the current VAO, so that all subsequent commands affect it
     */
    gl.glBindVertexArray(vao);


    //copy all the data to the vbo[0]
    gl.glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    gl.glBufferData(GL_ARRAY_BUFFER,
                    sizeof(float) * vertexDataAsFloats.size(),
                    &vertexDataAsFloats[0],
        GL_STATIC_DRAW);



    /*
     * go through all variables and enable that
     * attribute.
     */


    for (map<string,string>::const_iterator it=shaderVarsToAttributeNames.cbegin();
         it!=shaderVarsToAttributeNames.cend();
         it++)
      {
        /*
         * e.key: the name of the shader variable
         * e.value: the name of the corresponding vertex attribute in polygon mesh
         */


        int shaderLocation = shaderLocations.getLocation(it->first);

        if (shaderLocation>=0)
          {
            //tell opengl how to interpret the above data
            gl.glVertexAttribPointer(shaderLocation,
                                     vertexDataList[0].getData(it->second).size(),
                GL_FLOAT,
                GL_FALSE,
                sizeof(float) * stride,
                (void *)(sizeof(float) * offsets[it->second]));
            //enable this attribute so that when rendered, this is sent to the vertex shader
            gl.glEnableVertexAttribArray(shaderLocation);
          }
      }





    /*
     * Allocate the VBO for triangle indices and send it to GPU
     */
    gl.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
    gl.glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                    primitives.size()*sizeof(GLuint),
                    &primitives[0],
        GL_STATIC_DRAW);

    gl.glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);


    /*
     * Unbind the VAO to prevent accidental change to all the settings
     * so at this point, this VAO has two VBOs and two enabled VertexAttribPointers.
     * It is going to remember all of that!
     */
    gl.glBindVertexArray(0);
  }


  void ObjectInstance::cleanup(OpenGLFunctions& gl)
  {
    if (vao!=0)
      {
        //give back the VBO IDs to OpenGL, so that they can be reused
        gl.glDeleteBuffers(2,vbo);
        //give back the VAO ID to OpenGL, so that it can be reused
        gl.glDeleteVertexArrays(1,&vao);
      }
  }

  /*
 * Draw this ObjectInstance. This assumes that the object has been setup for rendering
 * prior to calling this method.
 */

  void ObjectInstance::draw(OpenGLFunctions& gl) const
  {

    //1. bind its VAO
    gl.glBindVertexArray(vao);

    //2. execute the "superpower" command
    //this effectively reads the index buffer, grabs the vertex data using
    //the indices and sends them to the shader
    gl.glDrawElements(primitiveType,primitiveCount, GL_UNSIGNED_INT,(GLvoid *)0);

    gl.glBindVertexArray(0);
  }



  /*
 * Set the name of this object
 */

  void ObjectInstance::setName(string name)
  {
    this->name = name;
  }

  /*
 * Gets the name of this object
 */

  string ObjectInstance::getName() const
  {
    return string(name);
  }


}
#endif
