#ifndef _SCENEGRAPH_H_
#define _SCENEGRAPH_H_

#include "GLScenegraphRenderer.h"
#include "INode.h"
#include "OpenGLFunctions.h"
#include "glm/glm.hpp"
#include "IVertexData.h"
#include "PolygonMesh.h"
#include "_3DRay.h"
#include "HitRecord.h"
#include <string>
#include <map>
using namespace std;

namespace sgraph
{

  /**
 * A specific implementation of this scene graph. This implementation is still independent
 * of the rendering technology (i.e. OpenGL)
 * \author Amit Shesh
 */

  class Scenegraph
  {
    /**
     * The root of the scene graph tree
     */
  protected:
    INode *root;



    /**
     * A map to store the (name,node) pairs. A map is chosen for efficient search
     */
    map<string,INode *> nodes;

    map<string,string> textures;

    /**
     * The associated renderer for this scene graph. This must be set before attempting to
     * render the scene graph
     */
    GLScenegraphRenderer *renderer;


  public:
    Scenegraph()
    {
      root = NULL;
    }

    ~Scenegraph()
    {
      dispose();
    }

    void dispose()
    {

      if (root!=NULL)
        {
          delete root;
          root = NULL;
        }
    }

    /**
     * Sets the renderer, and then adds all the meshes to the renderer.
     * This function must be called when the scene graph is complete, otherwise not all of its
     * meshes will be known to the renderer
     * \param renderer The IScenegraphRenderer object that will act as its renderer
     * \throws Exception
     */
    template <class VertexType>
    void setRenderer(GLScenegraphRenderer *renderer,map<string,
                     util::PolygonMesh<VertexType> >& meshes) throw(runtime_error)
    {
      this->renderer = renderer;

      //now add all the meshes
      for (typename map<string,util::PolygonMesh<VertexType> >::iterator it=meshes.begin();
           it!=meshes.end();
           it++)
        {
          this->renderer->addMesh<VertexType>(it->first,it->second);
        }

      //pass all the texture objects
      for (map<string,string>::iterator it=textures.begin();
           it!=textures.end();
           it++)
        {
          this->renderer->addTexture(it->first,it->second);
        }

    }


    /**
     * Set the root of the scenegraph, and then pass a reference to this scene graph object
     * to all its node. This will enable any node to call functions of its associated scene graph
     * \param root
     */

    void makeScenegraph(INode *root)
    {
      this->root = root;
      this->root->setScenegraph(this);

    }

    /**
     * Draw this scene graph. It delegates this operation to the renderer
     * \param modelView
     */
    void draw(stack<glm::mat4>& modelView) {
      if ((root!=NULL) && (renderer!=NULL))
        {
          renderer->draw(root,modelView);
        }
    }

    glm::vec3 raycast(_3DRay ray, stack<glm::mat4> modelview) {
        //calculate the color here then return it;

        //default color
        glm::vec3 color = glm::vec3(0,0,0);

        HitRecord hitRecord = getRoot()->getIntersection(ray, modelview);

        if (hitRecord.hit) {
            //printf("hit\n");
            color = shade(hitRecord);
        } else {
            //printf("not hit \n");
        }

        return color;
    }

    glm::vec3 shade(HitRecord hitRecord) {
        return glm::vec3(1,1,1);
    }

    void animate(float time)
    {

    }

    void addNode(const string& name, INode *node) {
      nodes[name]=node;
    }


    INode *getRoot()
    {
      return root;
    }



    map<string, INode *> getNodes()
    {
      return nodes;
    }

    void addTexture(const string& name, const string& path)
    {
      textures[name] = path;
    }
  };
}
#endif
