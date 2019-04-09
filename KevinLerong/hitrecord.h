#ifndef HITRECORD_H
#define HITRECORD_H

#include "View.h"
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <map>
#include <string>
using namespace std;


class HitRecord
{
public:
    HitRecord() {
    }
    ~HitRecord() {}

    float t;
    glm::vec3 inter;
    glm::vec3 normal;
    util::Material material;
private:

};

#endif // HITRECORD_H
