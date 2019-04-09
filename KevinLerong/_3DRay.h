#ifndef _3DRAY_H_
#define _3DRAY_H_

#include <glm/gtc/matrix_transform.hpp>
using namespace std;


class _3DRay
{
public:
    _3DRay(glm::vec4 _pos, glm::vec4 _dir) {
        pos = _pos;
        dir = _dir;
    }
    ~_3DRay() {}

    glm::vec4 pos;
    glm::vec4 dir;

    _3DRay mul(glm::mat4 transform) {
        _3DRay ray = _3DRay(this->pos * transform, this->dir * transform);

        return ray;
    }
private:

};


#endif //3DRAY_H
