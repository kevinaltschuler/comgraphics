#ifndef _TRANSFORMNODE_H_
#define _TRANSFORMNODE_H_

#include "AbstractNode.h"
#include "OpenGLFunctions.h"
#include "glm/glm.hpp"
#include "Light.h"
using namespace std;
#include <vector>
#include <stack>
#include <string>
using namespace std;

namespace sgraph
{

  /**
 * This node represents a transformation in the scene graph. It has only one child. The transformation
 * can be viewed as changing from its child's coordinate system to its parent's coordinate system
 * This also stores an animation transform that can be tweaked at runtime
 * \author Amit Shesh
 */
  class TransformNode: public AbstractNode
  {
    /**
     * Matrices storing the static and animation transformations separately, so that they can be
     * changed separately
     */
  protected:
    glm::mat4 transform,animation_transform;

    /**
     * A reference to its only child
     */
    INode *child;

  public:
    TransformNode(sgraph::Scenegraph *graph,const string& name)
      :AbstractNode(graph,name)
    {
      this->transform = glm::mat4(1.0);
      animation_transform = glm::mat4(1.0);
      child = NULL;
    }

    ~TransformNode()
    {
      if (child!=NULL)
        delete child;
    }

    /**
     * Creates a deep copy of the subtree rooted at this node
     * \return a deep copy of the subtree rooted at this node
     */
    INode *clone()
    {
      INode *newchild;

      if (child!=NULL)
        {
          newchild = child->clone();
        }
      else
        {
          newchild = NULL;
        }

      TransformNode *newtransform = new TransformNode(scenegraph,name);
      newtransform->setTransform(this->transform);
      newtransform->setAnimationTransform(animation_transform);

      if (newchild!=NULL)
        {
          try
          {
            newtransform->addChild(newchild);
          }
          catch (runtime_error e)
          {

          }
        }
      return newtransform;
    }

    /**
     * Determines if this node has the specified name and returns itself if so. Otherwise it recurses
     * into its only child
     * \param name name of node to be searched
     */
    INode *getNode(const string& name)
    {
      INode *n = AbstractNode::getNode(name);
      if (n!=NULL)
        return n;

      if (child!=NULL)
        {
          return child->getNode(name);
        }

      return NULL;
    }

    /**
     * Since this node can have a child, it override this method and adds the child to itself
     * This will overwrite any children set for this node previously.
     * \param child the child of this node
     * \throws runtime_error if a child already exists
     */
    void addChild(INode *child) throw(runtime_error)
    {
      if (this->child!=NULL)
        throw runtime_error("Transform node already has a child");
      this->child = child;
      this->child->setParent(this);
    }

    /**
     * Draws the scene graph rooted at this node
     * After preserving the current top of the modelview stack, this "post-multiplies" its
     * animation transform and then its transform in that order to the top of the model view
     * stack, and then recurses to its child. When the child is drawn, it restores the modelview
     * matrix
     * \param context the generic renderer context sgraph::IScenegraphRenderer
     * \param modelView the stack of modelview matrices
     */

    void draw(GLScenegraphRenderer& context,stack<glm::mat4>& modelView)
    {
      modelView.push(modelView.top());
      modelView.top() = modelView.top()
          * animation_transform
          * transform;
      if (child!=NULL)
        child->draw(context,modelView);
      modelView.pop();
    }


    /**
     * Sets the animation transform of this node
     * \param mat the animation transform of this node
     */
    void setAnimationTransform(const glm::mat4& mat) throw(runtime_error)
    {
      animation_transform = mat;
    }

    /**
     * Gets the transform at this node (not the animation transform)
     */
    glm::mat4 getTransform()
    {
      return transform;
    }

    /**
     * Sets the transformation of this node
     * \param t
     * \throws runtime_error this implementation does not throw this exception
     */
    void setTransform(const glm::mat4& t) throw(runtime_error)
    {
      this->transform = t;
    }

    /**
     * Gets the animation transform of this node
     * \return
     */
    glm::mat4 getAnimationTransform()
    {
      return animation_transform;
    }

    /**
     * Sets the scene graph object of which this node is a part, and then recurses to its child
     * \param graph a reference to the scenegraph object of which this tree is a part
     */
    void setScenegraph(sgraph::Scenegraph *graph)
    {
      AbstractNode::setScenegraph(graph);
      if (child!=NULL)
        {
          child->setScenegraph(graph);
        }
    }

    /**
       * Overridden version from @link{AbstractNode}. This version first modifies
       * the modelview matrix to include its transformation and animation
       * transformation. Then it collects all the lights from its child, and then
       * appends to them the lights from this node as well.
       *
       * It uses the original version for getting the lights in this node.
       */
    vector<util::Light> getLightsInView(stack<glm::mat4>& modelview)
    {
      vector<util::Light> lights,templights;

      modelview.push(modelview.top());
      modelview.top() = modelview.top() * animation_transform * transform;
      if (child != NULL)
        {
          templights = child->getLightsInView(modelview);
          for (unsigned int i=0;i<templights.size();i++)
            {
              lights.push_back(templights[i]);
            }
        }
      modelview.pop();

      //now get the lights from this node's lights
      templights = AbstractNode::getLightsInView(modelview);
      for (unsigned int i=0;i<templights.size();i++)
        {
          lights.push_back(templights[i]);
        }
      return lights;
    }
  };
}
#endif
