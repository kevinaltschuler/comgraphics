#ifndef _LEAFNODE_H_
#define _LEAFNODE_H_

#include "AbstractNode.h"
#include "OpenGLFunctions.h"
#include "Material.h"
#include "glm/glm.hpp"
#include <map>
#include <stack>
#include <string>
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
        glm::mat4 matrixRay = glm::inverse(glm::mat4(modelview.top()));
        _3DRay newRay = ray.mul(matrixRay);
        HitRecord newOne = HitRecord();

        if (objInstanceName.find("box") != std::string::npos) {
            //newOne = intersectBox(newRay, modelview);
        }
        else if (objInstanceName.find("sphere") != std::string::npos) {
            HitRecord result = HitRecord();
            glm::vec4 pos = newRay.pos;
            glm::vec4 dir = newRay.dir;

            //printf("%f %f %f \n", dir.x, dir.y, dir.z);

            float A = dir.x * dir.x + dir.y*dir.y + dir.z*dir.z;
            float B = 2* (dir.x*(pos.x-0) +dir.y*(pos.y-0)+dir.z*(pos.z-0)); // center: (0,0,0)
            float C = (pos.x - 0) * (pos.x - 0)
              + (pos.y - 0)*(pos.y - 0)
              + (pos.z - 0)*(pos.z - 0)
              -1;

            printf("A:%f B:%f C:%f \n", A, B, C);

            float delta = B*B - 4 * A * C;

            if (delta >= 0 ) {
                float t1 = B*(-1)-((float) sqrt(delta))/(2*A);
                float t2 = B*(-1)+((float) sqrt(delta))/(2*A);
                float tMin = min(t1, t2);
                if (tMin > 0 ) {
                    printf("t min than 0 \n");
                    glm::vec4 newIntersectP = pos + dir * tMin;
                    modelview.push(modelview.top());
                    modelview.top() = glm::transpose(glm::inverse(modelview.top()));
                    glm::vec4 normal = newIntersectP * modelview.top();
                    normal = glm::vec4(normal.x, normal.y, normal.z, 0);
                    modelview.pop();
                    result = HitRecord(tMin, newIntersectP, normal, this->getMaterial());
                }
            }
            return result;
        }
        return newOne;
    }
};
}
#endif
