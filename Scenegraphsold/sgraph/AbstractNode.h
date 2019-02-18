#ifndef _ABSTRACTNODE_H_
#define _ABSTRACTNODE_H_

#include "INode.h"
#include "glm/glm.hpp"
#include <string>
using namespace std;

namespace sgraph
{

  /**
 * This abstract class implements the sgraph::INode interface. It provides default methods
 * for many of the methods, especially the ones that could throw an exception
 * Child classes that do not want these exceptions throws should override these methods
 * \author Amit Shesh
 */
  class AbstractNode:public INode
  {
    /**
     * The name given to this node
     */
  protected:
    string name;
    /**
     * The parent of this node. Each node except the root has a parent. The root's parent is null
     */
    INode *parent;
    /**
     * A reference to the sgraph::IScenegraph object that this is part of
     */
    sgraph::Scenegraph *scenegraph;

    AbstractNode(sgraph::Scenegraph *graph,const string& name)
    {
      this->parent = NULL;
      scenegraph = graph;
      setName(name);
    }

    /**
     * By default, this method checks only itself. Nodes that have children should override this
     * method and navigate to children to find the one with the correct name
     * \param name name of node to be searched
     * \return the node whose name this is, null otherwise
     */
    INode *getNode(const string& name)
    {
      if (this->name == name)
        return this;

      return NULL;
    }

    /**
     * Sets the parent of this node
     * \param parent the node that is to be the parent of this node
     */

    void setParent(INode *parent)
    {
      this->parent = parent;
    }

    /**
     * Sets the scene graph object whose part this node is and then adds itself
     * to the scenegraph (in case the scene graph ever needs to directly access this node)
     * \param graph a reference to the scenegraph object of which this tree is a part
     */
    void setScenegraph(sgraph::Scenegraph *graph)
    {
      this->scenegraph = graph;
      graph->addNode(this->name,this);
    }

    /**
     * Sets the name of this node
     * \param name the name of this node
     */
    void setName(const string& name)
    {
      this->name = name;
    }

    /**
     * Gets the name of this node
     * \return the name of this node
     */
    string getName() { return name;}


    /**
     * By default, throws an exception. Any nodes that can have children should override this
     * method
     * \param child
     * \throws runtime_error
     */

    void addChild(INode *child) throw(runtime_error)
    {
      throw runtime_error("Not a composite node");
    }

    /**
     * By default, throws an exception. Any nodes that are capable of storing transformations
     * should override this method
     * \param t
     */

    void setTransform(const glm::mat4& t) throw(runtime_error)
    {
      throw runtime_error(getName()+" is not a transform node");
    }


    /**
     * By default, throws an exception. Any nodes that are capable of storing transformations
     * should override this method
     * \param t
     */

    void setAnimationTransform(const glm::mat4& t) throw(runtime_error)
    {
      throw runtime_error(getName()+" is not a transform node");
    }

    /**
     * By default, throws an exception. Any nodes that are capable of storing material should
     * override this method
     * \param m the material object to be associated with this node
     */
    void setMaterial(const util::Material& m) throw(runtime_error)
    {
      throw runtime_error(getName()+" is not a leaf node");
    }

    void setTextureName(const string& name) throw(runtime_error) {
      throw runtime_error("Textures not supported yet!");
    }

    /**
     * Adds a new light to this node.
     * \param l
     */
    void addLight(const util::Light& l) throw(runtime_error)
    {
      throw runtime_error("Lights not supported yet!");
    }

  };
}
#endif
