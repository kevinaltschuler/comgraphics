#ifndef _SCENEXMLREADER_H_
#define _SCENEXMLREADER_H_

#include "OpenGLFunctions.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <QXmlDefaultHandler>
#include <qxml.h>
#include "ObjImporter.h"
#include "INode.h"
#include "TransformNode.h"
#include "LeafNode.h"
#include "GroupNode.h"
#include "ScenegraphInfo.h"
#include <string>
#include <vector>
#include <map>
#include <fstream>
using namespace std;

namespace sgraph
{
  template <class T> class MyHandler;
  /**
 * A SAX parser for parsing the scene graph and compiling an {@link
 * sgraph.IScenegraph} object from it.
 *
 * \author Amit Shesh
 */
  class SceneXMLReader {
  public:
    template <class K>
    static sgraph::ScenegraphInfo<K> importScenegraph(const string& filename) throw(runtime_error)
    {

      MyHandler<K> handler;
      QFile xmlFile(QString::fromStdString(filename));
      if (!xmlFile.open(QIODevice::ReadOnly | QIODevice::Text))
        throw runtime_error("Could not open file: "+filename);
      QXmlInputSource source(&xmlFile);

      QXmlSimpleReader reader;


      reader.setContentHandler(&handler);

      bool answer = reader.parse(source);

      sgraph::ScenegraphInfo<K> info;
      info.scenegraph = NULL;

      if (answer)
        {
          info.scenegraph = handler.getScenegraph();
          info.meshes = handler.getMeshes();
        }
      else
        {
          printf("Parsing unsuccessful because %s\n",handler.errorString().toLatin1().constData());
        }

      return info;
    }
  };



  template<class K>
  class MyHandler: public QXmlDefaultHandler
  {
  private:
    sgraph::Scenegraph *scenegraph;
    map<string,util::PolygonMesh<K>> meshes;
    INode *node;
    stack<INode *> stackNodes;
    glm::mat4 transform;
    util::Material material;
    map<string, sgraph::INode *> subgraph;
    vector<float> data;

  public:
    sgraph::Scenegraph *getScenegraph() {
      return scenegraph;
    }

    map<string,util::PolygonMesh<K>> getMeshes()
    {
      return meshes;
    }

    MyHandler()
    {
    }

    bool startDocument()
    {
      node = NULL;
      scenegraph = new sgraph::Scenegraph();
      transform = glm::mat4(1.0);
      return true;
    }
    


    bool startElement( const QString & namespaceURI,const QString & localName,const QString & qName,const QXmlAttributes & atts)
    {
      if (qName.compare("scene")==0)
        {
          stackNodes.push(new sgraph::GroupNode(scenegraph, "Root of scene graph"));
          subgraph[stackNodes.top()->getName()] = stackNodes.top();
        }
      else if (qName.compare("group")==0)
        {
          string name = "";
          string copyof = "";
          string fromfile = "";
          for (int i = 0; i < atts.count(); i++)
            {
              if (atts.qName(i).compare("name")==0)
                name = atts.value(i).toLatin1().constData();
              else if (atts.qName(i).compare("copyof")==0)
                copyof = atts.value(i).toLatin1().constData();
              else if (atts.qName(i).compare("from")==0)
                fromfile = atts.value(i).toLatin1().constData();
            }
          if ((copyof.length() > 0) && (subgraph.count(copyof)==1))
            {
              node = subgraph[copyof]->clone();
              node->setName(name);
            }
          else if (fromfile.length() > 0)
            {
              sgraph::ScenegraphInfo<K> tempsginfo;
              tempsginfo = sgraph::SceneXMLReader::importScenegraph<K>(fromfile);

              node = new sgraph::GroupNode(scenegraph,name);

              for (typename map<string,util::PolygonMesh<K>>::iterator it=tempsginfo.meshes.begin();
                   it!=tempsginfo.meshes.end();it++)
                {
                  meshes[it->first] = it->second;
                }
              //rename all the nodes in tempsg to prepend with the name of the group node
              map<string, INode *> nodes = tempsginfo.scenegraph->getNodes();
              for (map<string,INode *>::iterator it=nodes.begin();it!=nodes.end();it++)
                {
                  it->second->setName(name + "-" + it->second->getName());
                  scenegraph->addNode(it->second->getName(), it->second);
                }

              node->addChild(tempsginfo.scenegraph->getRoot());
            }
          else
            {
              node = new sgraph::GroupNode(scenegraph, name);
            }
          stackNodes.top()->addChild(node);

          stackNodes.push(node);
          subgraph[stackNodes.top()->getName()] = stackNodes.top();
        }
      else if (qName.compare("transform")==0)
        {
          string name = "";
          for (int i = 0; i < atts.count(); i++)
            {
              if (atts.qName(i).compare("name")==0)
                name = atts.value(i).toLatin1().constData();
            }
          node = new sgraph::TransformNode(scenegraph, name);
          stackNodes.top()->addChild(node);

          stackNodes.push(node);
          subgraph[stackNodes.top()->getName()] = stackNodes.top();
        }
      else if (qName.compare("object")==0)
        {
          string name = "";
          string objectname = "";
          for (int i = 0; i < atts.count(); i++)
            {
              if (atts.qName(i).compare("name")==0)
                {
                  name = atts.value(i).toLatin1().constData();
                }
              else if (atts.qName(i).compare("instanceof")==0)
                {
                  objectname = atts.value(i).toLatin1().constData();
                }
            }
          if (objectname.length() > 0)
            {
              node = new sgraph::LeafNode(objectname, scenegraph, name);
              stackNodes.top()->addChild(node);

              stackNodes.push(node);
              subgraph[stackNodes.top()->getName()] = stackNodes.top();
            }
        }
      else if (qName.compare("instance")==0)
        {
          string name = "";
          string path = "";
          for (int i = 0; i < atts.count(); i++)
            {
              if (atts.qName(i).compare("name")==0)
                {
                  name = atts.value(i).toLatin1().constData();
                }
              else if (atts.qName(i).compare("path")==0)
                {
                  path = atts.value(i).toLatin1().constData();
                  if (path.substr(path.length()-4).compare(".obj")!=0)
                    path = path + ".obj";
                }
            }
          if ((name.length() > 0) && (path.length() > 0))
            {
              util::PolygonMesh<K> mesh;
              ifstream in(path.c_str());
              mesh = util::ObjImporter<K>::importFile(in, false);
              meshes[name] = mesh;
            }
        }

      return true;
    }

    bool endElement( const QString&, const QString&, const QString& qName)
    {
      if (qName.compare("scene")==0)
        {
          if (stackNodes.top()->getName().compare("Root of scene graph")==0)
            scenegraph->makeScenegraph(stackNodes.top());
        }
      else if ((qName.compare("group")==0) ||
               (qName.compare("transform")==0) ||
               (qName.compare("object")==0))
        {
          stackNodes.pop();
        }
      else if (qName.compare("set")==0)
        {
          stackNodes.top()->setTransform(transform);
          transform = glm::mat4(1.0);
        }
      else if (qName.compare("scale")==0)
        {
          if (data.size()!=3)
            return false;
          transform = transform * glm::scale(glm::mat4(1.0),glm::vec3(data[0],data[1],data[2]));
          data.clear();
        }
      else if (qName.compare("rotate")==0)
        {
          if (data.size()!=4)
            return false;
          transform = transform * glm::rotate(glm::mat4(1.0),
                                              glm::radians(data[0]),
              glm::vec3(data[1],data[2],data[3]));
          data.clear();
        }
      else if (qName.compare("translate")==0)
        {
          if (data.size()!=3)
            return false;
          transform = transform * glm::translate(glm::mat4(1.0),glm::vec3(data[0],data[1],data[2]));
          data.clear();
        }
      else if (qName.compare("material")==0)
        {
          stackNodes.top()->setMaterial(material);
          material = util::Material();
        }
      else if (qName.compare("color")==0)
        {
          if (data.size()!=3)
            return false;
          material.setAmbient(data[0],data[1],data[2]);
          material.setEmission(material.getAmbient());
          material.setDiffuse(material.getAmbient());
          material.setSpecular(material.getAmbient());
          material.setShininess(1.0f);
          data.clear();
        }
      else if (qName.compare("ambient")==0)
        {
          if (data.size()!=3)
            return false;
          material.setAmbient(data[0],data[1],data[2]);
          data.clear();
        }
      else if (qName.compare("diffuse")==0)
        {
          if (data.size()!=3)
            return false;
          material.setDiffuse(data[0],data[1],data[2]);
          data.clear();
        }
      else if (qName.compare("specular")==0)
        {
          if (data.size()!=3)
            return false;
          material.setSpecular(data[0],data[1],data[2]);
          data.clear();
        }
      else if (qName.compare("emissive")==0)
        {
          if (data.size()!=3)
            return false;
          material.setEmission(data[0],data[1],data[2]);
          data.clear();
        }
      else if (qName.compare("shininess")==0)
        {
          if (data.size()!=1)
            return false;
          material.setShininess(data[0]);
          data.clear();
        }
      else if (qName.compare("absorption")==0)
        {
          if (data.size()!=1)
            return false;
          material.setAbsorption(data[0]);
          data.clear();
        }
      else if (qName.compare("reflection")==0)
        {
          if (data.size()!=1)
            return false;
          material.setReflection(data[0]);
          data.clear();
        }
      else if (qName.compare("transparency")==0)
        {
          if (data.size()!=1)
            return false;
          material.setTransparency(data[0]);
          data.clear();
        }
      else if (qName.compare("refractive")==0)
        {
          if (data.size()!=1)
            return false;
          material.setRefractiveIndex(data[0]);
          data.clear();
        }
    return true;

    }


    bool characters(const QString& text)
    {
      int c;
      float f;
      QString info;

      info = text;
      info.remove(QRegExp("[\n\t\r]*")); //remove tabs and newlines

      QStringList list = info.split(' ',QString::SkipEmptyParts);


      for (int i=0;i<list.length();i++)
        {
          c=sscanf(list.at(i).toLatin1().constData(),"%f",&f);
          printf("Read number: %f\n",f);
          data.push_back(f);
          if (c!=1)
            return true;

        }

      //all the data numbers are in the data array

      return true;
    }

  };
}
#endif





