#ifndef _INODE_H_
#define _INODE_H_

#include "OpenGLFunctions.h"
#include <glm/glm.hpp>
#include "Light.h"
#include "Material.h"
#include <vector>
#include <stack>
#include <string>
using namespace std;



namespace sgraph
{
  class Scenegraph;
  class GLScenegraphRenderer;

  /**
 * This interface represents all the operations offered by any type of node in our scenegraph.
 * Not all types of nodes are able to offer all types of operations.
 * This is implemented by the sgraph::AbstractNode throwing an exception for all such methods, and
 * appropriate nodes overriding these methods
 *
 * \author Amit Shesh
 */
  class INode
  {
  public:
    /**
     * In the scene graph rooted at this node, get the node whose name is as given
     * \param name name of node to be searched
     * \return the node reference if it exists, null otherwise
     */
    virtual INode *getNode(const string& name)=0;
    INode(){}

    virtual ~INode(){}

    /**
     * Draw the scene graph rooted at this node, using the modelview stack and context
     * \param context the generic renderer context {@link sgraph.IScenegraphRenderer}
     * \param modelView the stack of modelview matrices
     */
    virtual void draw(GLScenegraphRenderer& context,stack<glm::mat4>& modelView)=0;

    /**
     * Return a deep copy of the scene graph subtree rooted at this node
     * \return a reference to the root of the copied subtree
     */
    virtual INode *clone()=0;

    /**
     * Set the parent of this node. Each node except the root has a parent
     * \param parent the node that is to be the parent of this node
     */
    virtual void setParent(INode *parent)=0;

    /**
     * Traverse the scene graph rooted at this node, and store references to the scenegraph object
     * \param graph a reference to the scenegraph object of which this tree is a part
     */

    virtual void setScenegraph(sgraph::Scenegraph *graph)=0;

    /**
     * Set the name of this node. The name is not guaranteed to be unique in the tree, but it should be.
     * \param name the name of this node
     */
    virtual void setName(const string& name)=0;


    /**
     * Get the name of this node
     * \return the name of this node
     */
    virtual string getName()=0;

    /**
     * Add a child to this node. Not all types of nodes have the capability of having children.
     * If the node cannot have a child, this method throws a runtime exception
     * \param node the node that must be added as a child to this node
     * \throws runtime_error if this node is unable to have children (i.e. leaves)
     */
    virtual void addChild(INode *node) throw(runtime_error)=0;

    /**
     * Set the transformation associated with this node. Not all types of nodes can have transformations.
     * If the node cannot store a transformation, this method throws a runtime_error
     * \param m the transformation matrix associated with this transformation
     * \throws runtime_error if this node is unable to store a transformation (all nodes except TransformNode)
     */
    virtual void setTransform(const glm::mat4& m) throw(runtime_error)=0;


    /**
     * Set the animation transformation associated with this node. Not all types of nodes can have transformations.
     * If the node cannot store an animation transformation, this method throws a runtime_error
     * \param m the animation tranformation matrix associated with this node
     * \throws runtime_error if this node is unable to store a transformation (all nodes except TransformNode)
     */
    virtual void setAnimationTransform(const glm::mat4& m) throw(runtime_error)=0;


    /**
     * Set the material associated with this node. Not all types of nodes can have materials associated with them.
     * If the node cannot have a material, this method throws a runtime_error
     * \param m the material object to be associated with this node
     * \throws runtime_error if this node is unable to store a material (all nodes except leaves)
     */
    virtual void setMaterial(const util::Material& m) throw(runtime_error)=0;

    /**
     * Sets the texture to be associated with this node. Not all types of nodes
     * can have textures associated with them. If the node cannot have a texture, this
     * method throws a runtime_error.
     * \param name
     * \throws runtime_error
     */
    virtual void setTextureName(const string& name) throw(runtime_error)=0;

    /**
     * Adds a new light to this node.
     * \param l
     */
    virtual void addLight(const util::Light& l)=0;

    /**
       * Return a list of all lights in this scene graph in the view coordinate
       * system This function is called on the root of the scene graph. It is
       * assumed that the modelview.peek is set to the world-to-view
       * transformation.
       */
    virtual vector<util::Light> getLightsInView(stack<glm::mat4>& modelview)=0;
  };
}

#endif
