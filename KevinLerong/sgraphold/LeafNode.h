#ifndef _LEAFNODE_H_
#define _LEAFNODE_H_

#include "AbstractNode.h"
#include "OpenGLFunctions.h"
#include "Material.h"
#include "glm/glm.hpp"
#include <map>
#include <stack>
#include <string>
#include <iostream>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
using namespace std;

namespace sgraph
{

/**
 * This node represents the leaf of a scene graph. It is the only type of node that has
 * actual geometry to render.
 * \author Amit Shesh
 */
class LeafNode: public AbstractNode
{
    /**
     * The name of the object instance that this leaf contains. All object instances are stored
     * in the scene graph itself, so that an instance can be reused in several leaves
     */
protected:
    string objInstanceName;
    /**
     * The material associated with the object instance at this leaf
     */
    util::Material material;

    string textureName;

public:
    LeafNode(const string& instanceOf,sgraph::Scenegraph *graph,const string& name)
        :AbstractNode(graph,name)
    {
        this->objInstanceName = instanceOf;
    }
	
	~LeafNode(){}



    /*
	 *Set the material of each vertex in this object
	 */
    void setMaterial(const util::Material& mat) throw(runtime_error)
    {
        material = mat;
    }

    /**
     * Set texture ID of the texture to be used for this leaf
     * \param name
     */
    void setTextureName(const string& name) throw(runtime_error)
    {
        textureName = name;
    }

    /*
     * gets the material
     */
    util::Material getMaterial()
    {
        return material;
    }

    INode *clone()
    {
        LeafNode *newclone = new LeafNode(this->objInstanceName,scenegraph,name);
        newclone->setMaterial(this->getMaterial());
        return newclone;
    }

    /*
     *Set the animation in this object
     */
    void setAnimation(const glm::mat4& anim) throw(runtime_error) {
        animation = anim;
        animated = true;
    }

    /**
     * Delegates to the scene graph for rendering. This has two advantages:
     * <ul>
     *     <li>It keeps the leaf light.</li>
     *     <li>It abstracts the actual drawing to the specific implementation of the scene graph renderer</li>
     * </ul>
     * \param context the generic renderer context {@link sgraph.IScenegraphRenderer}
     * \param modelView the stack of modelview matrices
     * \throws runtime_error
     */
    void draw(GLScenegraphRenderer& context,stack<glm::mat4>& modelView) throw(runtime_error)
    {
        if (objInstanceName.length()>0) {
            if (animation != glm::mat4(1.0)) {
                //qDebug(glm::to_string(animation).c_str());
            }
            context.drawMesh(objInstanceName,material,textureName, modelView.top());
        }
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

        return _lights;
    }
};
}
#endif
