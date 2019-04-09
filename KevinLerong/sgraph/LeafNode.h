#ifndef _LEAFNODE_H_
#define _LEAFNODE_H_

#include "AbstractNode.h"
#include "OpenGLFunctions.h"
#include "Material.h"
#include "glm/glm.hpp"
#include <map>
#include <stack>
#include <string>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

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
        if (objInstanceName.length()>0)
        {
            context.drawMesh(objInstanceName,material,textureName,modelView.top());
        }
    }

    HitRecord getIntersection(_3DRay ray, stack<glm::mat4>& modelview) {
        glm::mat4 transform = glm::inverse(glm::mat4(modelview.top()));
        HitRecord newOne = HitRecord();

        if (objInstanceName.find("box") != std::string::npos) {
            //newOne = intersectBox(newRay, modelview);
        }
        else if (objInstanceName.find("sphere") != std::string::npos) {
            HitRecord result = HitRecord();

//            printf("%s", glm::to_string(modelview.top()).c_str());

//            printf("ray pos: %f %f %f %f \n", ray.pos.x, ray.pos.y, ray.pos.z, ray.pos.operator[](3));
//            printf("ray dir: %f %f %f \n", ray.dir.x, ray.dir.y, ray.dir.z);

            glm::vec4 pos = ray.pos * modelview.top();
            glm::vec4 dir = ray.dir * modelview.top();


//            printf("pos: %f %f %f \n", pos.x, pos.y, pos.z);
//            printf("dir: %f %f %f \n", dir.x, dir.y, dir.z);

            float A = dir.x * dir.x + dir.y*dir.y + dir.z*dir.z;
            float B = 2* (dir.x*pos.x + dir.y*pos.y + dir.z*pos.z);
            float C = pos.x*pos.x + pos.y*pos.y + pos.z*pos.z - 1;

            //printf("A:%f B:%f C:%f \n", A, B, C);

            float d = B*B - 4 * A * C;

            if (d >= 0 ) {
                float t1 = (-1.0 * B) - sqrt(d) / (2*A);
                float t2 = (-1.0 * B) + sqrt(d) / (2*A);
                //printf("t1 %f t2 %f \n", t1, t2);
                float tMin = min(t1, t2);
                if (tMin > 0) {
                    printf("t min than 0 \n");
                    glm::vec4 inter = pos + dir * tMin;
                    modelview.push(modelview.top());
                    modelview.top() = glm::transpose(glm::inverse(modelview.top()));
                    glm::vec4 normal = inter * modelview.top();
                    normal = glm::vec4(normal.x, normal.y, normal.z, 0);
                    modelview.pop();
                    result = HitRecord(tMin, inter, normal, this->getMaterial());
                }
            }
            return result;
        }
        return newOne;
    }
};
}
#endif
