#ifndef _3DRAY_H
#define _3DRAY_H

#include <glm/gtc/matrix_transform.hpp>
using namespace std;


class _3DRay
{
public:
    _3DRay(glm::vec3 _pos, glm::vec3 _dir) {
        pos = _pos;
        dir = _dir;
    }
    ~_3DRay() {}

    glm::vec3 pos;
    glm::vec3 dir;
private:

};


#endif //3DRAY_H
