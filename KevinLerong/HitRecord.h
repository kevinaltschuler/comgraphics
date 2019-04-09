#ifndef HITRECORD_H
#define HITRECORD_H

#include "OpenGLFunctions.h"
#include <glm/glm.hpp>
#include "Material.h"

using namespace std;


class HitRecord
{
public:
    HitRecord() {
    }

    HitRecord(float time, glm::vec4 intr, glm::vec4 nor, util::Material mat) {
        t = time;
        inter = intr;
        normal = nor;
        material = mat;
        hit = true;
    }
    ~HitRecord() {}

    float t;
    glm::vec4 inter;
    glm::vec4 normal;
    util::Material material;
    bool hit = false;
private:
};

#endif // HITRECORD_H
