#ifndef _GROUPNODE_H_
#define _GROUPNODE_H_

#include "OpenGLFunctions.h"
#include "AbstractNode.h"
#include "glm/glm.hpp"
#include "Light.h"
#include <vector>
#include <stack>
#include <string>
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

    /**
     * Sets the reference to the scene graph object for this node, and then recurses down
     * to children for the same
     * \param graph a reference to the scenegraph object of which this tree is a part
     */
    void setScenegraph(sgraph::Scenegraph *graph)
    {
      AbstractNode::setScenegraph(graph);
      for (int i=0;i<children.size();i++)
        {
          children[i]->setScenegraph(graph);
        }
    }

    /**
     * To draw this node, it simply delegates to all its children
     * \param context the generic renderer context sgraph::IScenegraphRenderer
     * \param modelView the stack of modelview matrices
     */
    void draw(GLScenegraphRenderer& context,stack<glm::mat4>& modelView)
    {
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

    /**
       * Overridden version from @link{AbstractNode}. This version first collects
       * all the lights from its children, and then appends to them the lights from
       * this node as well.
       *
       * It uses the original version for getting the lights in this node.
       */
    vector<util::Light> getLightsInView(stack<glm::mat4>& modelview)
    {
      vector<util::Light> lights,templights;
      for (unsigned int i = 0; i < children.size(); i++)
        {
          templights = children[i]->getLightsInView(modelview);
          for (unsigned int j=0;j<templights.size();j++)
            {
              lights.push_back(templights[j]);
            }
        }
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
