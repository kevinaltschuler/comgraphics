#ifndef _ISCENEGRAPH_H_
#define _ISCENEGRAPH_H_


#include "OpenGLFunctions.h"
#include <glm/glm.hpp>
#include "IVertexData.h"
#include "GLScenegraphRenderer.h"
#include "INode.h"
#include "ShaderProgram.h"
using namespace util;

#include <map>
#include <stack>
using namespace std;
namespace sgraph
{

    /**
 * This virtual class captures all the operations that a scene graph should offer.
 * It is designed to be a generic scene graph that is independent of the actual
 * rendering library. It achieves this by working with an
 * sgraph::IScenegraphRenderer virtual class.
 *
 * The scene graph provides functions to store actual geometry
 * (object instances) within itself so that they can be shared within many
 * leaves. Each node of the scene graph keeps a reference to it, enabling any
 * node of the scene graph to directly call its functions.
 *
 * Conversely the scene graph also stores references to all the nodes keyed by
 * their name. This way the scene graph can directly refer to any of its nodes
 * by name instead of traversing the tree every time to find it. This is useful
 * when nodes must be identified and animated in specific ways.
 * \author Amit Shesh
 */

    class IScenegraph
    {
        /**
     * Set the renderer to this scene graph. All specific rendering commands
     * are delegated to this renderer, making the scene graph independent of
     * the rendering implementation
     *
     * \param renderer The IScenegraphRenderer object that will act as its renderer
     */
    public:
        virtual void setRenderer(GLScenegraphRenderer *renderer)=0;

        /**
     * initialize the supplied root to the be the root of this scene graph.
     * This is supposed to overwrite any previous roots
     * \param root
     */
        virtual void makeScenegraph(INode *root)=0;

        /**
     * Draw this scene graph, using the stack of modelview matrices provided.
     * The scene graph will use this stack as it navigates its tree.
     * \param modelView
     */
        virtual void draw(stack<glm::mat4>& modelView)=0;

        /**
     * Add a polygon mesh that will be used by one or more leaves in this scene
     * graph
     * \param name a unique name by which this mesh may be referred to in future
     * \param obj the {@link util::PolygonMesh} object
     */
        virtual void addPolygonMesh(const string& name,
                                    util::PolygonMesh<VertexType>& obj)=0;

        /**
     * Specific scene graph implementations should put code that animates
     * specific nodes in the scene graph, based on a time provided by the caller
     * \param time provides a simple time reference for animation
     */
        virtual void animate(float time)=0;

        /**
     * Adds a node to itself. This should be stored in a suitable manner by an
     * implementation, so that it is possible to look up a specific node by name
     * \param name (hopefully unique) name given to this node
     * \param node the node object
     */
        virtual void addNode(const string& name,INode *node)=0;

        /**
     * Get the root of this scene graph
     * \return the root of this scene graph
     */
        INode *getRoot();

        /**
     * Get a mapping of all (name,mesh) pairs that have been added to this scene
     * graph. This function is useful in case all meshes of one scene graph have
     * to be added to another in an attempt to merge two scene graphs
     *
     */
        virtual map<string,util::PolygonMesh<VertexType> *> getPolygonMeshes()=0;


        /**
     * Get a mapping of all (name,INode) pairs for all nodes in this scene graph.
     * This function is useful in case all meshes of one scene graph have to be added to another
     * in an attempt to merge two scene graphs
     */
        virtual map<string,INode *> getNodes()=0;
        /**
     * Add a new texture by this name
     * \param name
     * \param path
     */
        virtual void addTexture(const string& name,const string& path)=0;
        virtual void dispose()=0;
    };
}

#endif
