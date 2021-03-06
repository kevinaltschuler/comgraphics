#ifndef _GROUPNODE_H_
#define _GROUPNODE_H_

#include "OpenGLFunctions.h"
#include "AbstractNode.h"
#include "glm/glm.hpp"
#include "Light.h"
#include <vector>
#include <stack>
#include <string>
#include <iostream>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;

namespace sgraph
{
  /**
 * This class represents a group node in the scenegraph. A group node is simply a logical grouping
 * of other nodes. It can have an arbitrary number of children. Its children can be nodes of any type
 * \author Amit Shesh
 */
  class GroupNode:public AbstractNode
  {
    /**
     * A list of its children
     */
  protected:
    vector<INode *> children;

  public:
    GroupNode(sgraph::Scenegraph *graph,const string& name)
      :AbstractNode(graph,name)
    {      
    }
	
    ~GroupNode()
    {
      for (int i=0;i<children.size();i++)
        {
          delete children[i];
        }
    }

    /**
     * Searches recursively into its subtree to look for node with specified name.
     * \param name name of node to be searched
     * \return the node whose name this is if it exists within this subtree, null otherwise
     */
    INode *getNode(const string& name)
    {
      INode *n = AbstractNode::getNode(name);
      if (n!=NULL)
      {
        return n;
      }

      int i=0;
      INode *answer = NULL;

      while ((i<children.size()) && (answer == NULL))
      {
        answer = children[i]->getNode(name);
        i++;
      }
      return answer;
    }

    /*
     *Set the animation in this object
     */
    void setAnimation(const glm::mat4& anim) throw(runtime_error) {
        animation = anim;
        animated = true;
        for (int i=0;i<children.size();i++) {
           // qDebug("%d", i);
            children[i]->setAnimation(anim);
        }

    }

    /**
     * Sets the reference to the scene graph object for this node, and then recurses down
     * to children for the same
     * \param graph a reference to the scenegraph object of which this tree is a part
     */
    void setScenegraph(sgraph::Scenegraph *graph)
    {
      AbstractNode::setScenegraph(graph);
      for (int i=0;i<children.size();i++) {
        children[i]->setScenegraph(graph);
      }
    }

    /**
     * To draw this node, it simply delegates to all its children
     * \param context the generic renderer context sgraph::IScenegraphRenderer
     * \param modelView the stack of modelview matrices
     */
    void draw(GLScenegraphRenderer& context, stack<glm::mat4>& modelView) {
        if (animated) {
            //qDebug(glm::to_string(animation).c_str());
        }
      for (int i=0;i<children.size();i++)
        {
          children[i]->draw(context,modelView);
        }
    }

    /**
     * Makes a deep copy of the subtree rooted at this node
     * \return a deep copy of the subtree rooted at this node
     */
    INode *clone()
    {
      vector<INode *> newc;

      for (int i=0;i<children.size();i++)
        {
          newc.push_back(children[i]->clone());
        }

      GroupNode *newgroup = new GroupNode(scenegraph,name);

      for (int i=0;i<children.size();i++)
        {
          try
          {
            newgroup->addChild(newc[i]);
          }
          catch (runtime_error e)
          {

          }
        }
      return newgroup;
    }

    /**
     * Since a group node is capable of having children, this method overrides the default one
     * in sgraph::AbstractNode and adds a child to this node
     * \param child
     * \throws runtime_error this class does not throw this exception
     */
    void addChild(INode *child) throw(runtime_error)
    {
      children.push_back(child);
      child->setParent(this);
    }

    /**
     * Get a list of all its children, for convenience purposes
     * \return a list of all its children
     */

    vector<INode *> getChildren()
    {
      return children;
    }

    std::vector<util::Light> getLights(stack<glm::mat4>& modelview) throw(runtime_error) {

        std::vector<util::Light> _lights = {};

        glm::mat4 top = modelview.top();
        for (int i = 0; i < nodeLights.size(); i++) {
            util::Light l = nodeLights.at(i);
            glm::vec4 pos = nodeLights.at(i).getPosition();
            pos = top * pos;
            l.setPosition(pos);
            _lights.push_back(l);
        }

        for (auto child : children) {
            std::vector<util::Light> __lights = child->getLights(modelview);
            _lights.insert(_lights.end(), __lights.begin(), __lights.end());

        }
        return _lights;
    }
  };
}

#endif
