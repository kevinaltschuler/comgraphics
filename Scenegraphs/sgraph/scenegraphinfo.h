#ifndef SCENEGRAPHINFO_H
#define SCENEGRAPHINFO_H

#include "PolygonMesh.h"
#include "Scenegraph.h"
#include <string>
#include <map>
using namespace std;

namespace sgraph
{
    template<class K>
    class ScenegraphInfo
    {
    public:
      sgraph::Scenegraph *scenegraph;
      map<string,util::PolygonMesh<K> > meshes;
    };
}

#endif // SCENEGRAPHINFO_H
