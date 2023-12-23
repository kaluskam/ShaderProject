////////////////////////////////////////////////////////////////////////////////
//
// Mesh Loader Class
//
// Copyright (c)2022-23 by Carlos Martinho
//
////////////////////////////////////////////////////////////////////////////////

#ifndef MGL_MESH_HPP
#define MGL_MESH_HPP

#include <GL/glew.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <json.hpp>
#include <sstream>

#include <assimp/Importer.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include <string>
#include <vector>

#include "mglTransform.hpp"

using json = nlohmann::json;
namespace mgl {

class IDrawable {
public:
    virtual void draw(void) = 0;
};

class Mesh;

#define CREATE_BITANGENT

/////////////////////////////////////////////////////////////////////////// Mesh

class Mesh : public IDrawable {
 public:
  static const GLuint INDEX = 0;
  static const GLuint POSITION = 1;
  static const GLuint NORMAL = 2;
  static const GLuint TEXCOORD = 3;
  static const GLuint TANGENT = 4;
#ifdef CREATE_BITANGENT
  static const GLuint BITANGENT = 6;
#endif
  static const GLuint COLOR = 5;

  Mesh();
  ~Mesh();

  void setAssimpFlags(unsigned int flags);
  void joinIdenticalVertices();
  void generateNormals();
  void generateSmoothNormals();
  void generateTexcoords();
  void calculateTangentSpace();
  void flipUVs();

  void create(const std::string &filename);
  void draw() override;
  //void draw(bool drawChildren = true, Mesh* drawSelected = NULL);


  Transform &getTransform(void);
  void setTransform(Transform*);

  bool hasNormals();
  bool hasTexcoords();
  bool hasTangentsAndBitangents();

  void setEffect(int);
  int getEffect();

  json toJSON();
  void fromJSON(json j);

 private:
  GLuint VaoId;
  unsigned int AssimpFlags;
  bool NormalsLoaded, TexcoordsLoaded, TangentsAndBitangentsLoaded;
  Transform* transform = nullptr;
  int effect;

  struct MeshData {
    unsigned int nIndices = 0;
    unsigned int baseIndex = 0;
    unsigned int baseVertex = 0;
  };
  std::vector<MeshData> Meshes;

  std::vector<glm::vec3> Positions;
  std::vector<glm::vec3> Normals;
  std::vector<glm::vec2> Texcoords;
  std::vector<glm::vec3> Tangents;
#ifdef CREATE_BITANGENT
  std::vector<glm::vec3> Bitangents;
#endif
  std::vector<unsigned int> Indices;

  void processScene(const aiScene *scene);
  void processMesh(const aiMesh *mesh);
  void createBufferObjects();
  void destroyBufferObjects();
  json vecOfMeshDataToJSON(std::vector<MeshData> vec);
  std::vector<MeshData> toVecOfMeshData(json j);
};

////////////////////////////////////////////////////////////////////////////////
}  // namespace mgl

#endif /* MGL_MESH_HPP  */
