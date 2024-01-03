////////////////////////////////////////////////////////////////////////////////
//
// Mesh Loader Class
//
// Copyright (c)2022-23 by Carlos Martinho
//
////////////////////////////////////////////////////////////////////////////////

#include "./mglMesh.hpp"

namespace mgl {

////////////////////////////////////////////////////////////////////////////////

Mesh::Mesh() {
  NormalsLoaded = false;
  TexcoordsLoaded = false;
  TangentsAndBitangentsLoaded = false;
  MaterialsLoaded = false;
  VaoId = -1;
  AssimpFlags = aiProcess_Triangulate;
}

Mesh::~Mesh() { destroyBufferObjects(); }

void Mesh::setAssimpFlags(unsigned int flags) { AssimpFlags = flags; }

void Mesh::joinIdenticalVertices() {
  AssimpFlags |= aiProcess_JoinIdenticalVertices;
}

void Mesh::generateNormals() { AssimpFlags |= aiProcess_GenNormals; }

void Mesh::generateSmoothNormals() {
  AssimpFlags |= aiProcess_GenSmoothNormals;
}

void Mesh::generateTexcoords() { AssimpFlags |= aiProcess_GenUVCoords; }

void Mesh::calculateTangentSpace() {
  AssimpFlags |= aiProcess_CalcTangentSpace;
}

void Mesh::flipUVs() { AssimpFlags |= aiProcess_FlipUVs; }

bool Mesh::hasNormals() { return NormalsLoaded; }

bool Mesh::hasTexcoords() { return TexcoordsLoaded; }

bool Mesh::hasTangentsAndBitangents() { return TangentsAndBitangentsLoaded; }

bool Mesh::hasMaterials() { return MaterialsLoaded; }

////////////////////////////////////////////////////////////////////////////////

void Mesh::setTransform(Transform* t) {
    transform = t;
}


////////////////////////////////////////////////////////////////////////////////

void Mesh::processMesh(const aiMesh *mesh) {
  NormalsLoaded = mesh->HasNormals();
  TexcoordsLoaded = mesh->HasTextureCoords(0);
  TangentsAndBitangentsLoaded = mesh->HasTangentsAndBitangents();

  for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
    const aiVector3D &aiPosition = mesh->mVertices[i];
    Positions.push_back(glm::vec3(aiPosition.x, aiPosition.y, aiPosition.z));
    if (NormalsLoaded) {
      const aiVector3D &aiNormal = mesh->mNormals[i];
      Normals.push_back(glm::vec3(aiNormal.x, aiNormal.y, aiNormal.z));
    }
    if (TexcoordsLoaded) {
      const aiVector3D &aiTexcoord = mesh->mTextureCoords[0][i];
      Texcoords.push_back(glm::vec2(aiTexcoord.x, aiTexcoord.y));
    }
    if (TangentsAndBitangentsLoaded) {
      const aiVector3D &aiTangent = mesh->mTangents[i];
      Tangents.push_back(glm::vec3(aiTangent.x, aiTangent.y, aiTangent.z));

#ifdef CREATE_BITANGENT
      const aiVector3D &aiBitangent = mesh->mBitangents[i];
      Bitangents.push_back(
          glm::vec3(aiBitangent.x, aiBitangent.y, aiBitangent.z));
#endif
    }
  }
  for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
    const aiFace &face = mesh->mFaces[i];
    Indices.push_back(face.mIndices[0]);
    Indices.push_back(face.mIndices[1]);
    Indices.push_back(face.mIndices[2]);
  }
}

void Mesh::processScene(const aiScene *scene) {
  Meshes.resize(scene->mNumMeshes);
  if (scene->HasMaterials()) {
      material = *scene->mMaterials[0];
  }
  unsigned int n_vertices = 0;
  unsigned int n_indices = 0;
  for (unsigned int i = 0; i < Meshes.size(); i++) {
    Meshes[i].nIndices = scene->mMeshes[i]->mNumFaces * 3;
    Meshes[i].baseVertex = n_vertices;
    Meshes[i].baseIndex = n_indices;

    n_vertices += scene->mMeshes[i]->mNumVertices;
    n_indices += Meshes[i].nIndices;
  }
  Positions.reserve(n_vertices);
  Normals.reserve(n_vertices);
  Texcoords.reserve(n_vertices);
  Indices.reserve(n_indices);

  for (unsigned int i = 0; i < Meshes.size(); i++) {
    processMesh(scene->mMeshes[i]);
  }

#ifdef DEBUG
  std::cout << "Loaded " << Meshes.size() << " mesh(es) [" << n_vertices
            << " vertices, " << n_indices << " indices, " << n_indices / 3
            << " triangles]" << std::endl;
#endif
}

void Mesh::create(const std::string &filename) {
  Assimp::Importer importer;
  const aiScene *scene = importer.ReadFile(filename, AssimpFlags);
  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode) {
    std::cout << "Error while loading:" << importer.GetErrorString()
              << std::endl;
    exit(EXIT_FAILURE);
  }

#ifdef DEBUG
  std::cout << "Processing [" << filename << "]" << std::endl;
#endif

  processScene(scene);
  createBufferObjects();
}

void Mesh::createBufferObjects() {
    GLuint buffNum = 7;
  GLuint boId[7];

  glGenVertexArrays(1, &VaoId);
  glBindVertexArray(VaoId);
  {
    glGenBuffers(buffNum, boId);

    glBindBuffer(GL_ARRAY_BUFFER, boId[POSITION]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Positions[0]) * Positions.size(),
                 &Positions[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(POSITION);
    glVertexAttribPointer(POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0);

    if (NormalsLoaded) {
      glBindBuffer(GL_ARRAY_BUFFER, boId[NORMAL]);
      glBufferData(GL_ARRAY_BUFFER, sizeof(Normals[0]) * Normals.size(),
                   &Normals[0], GL_STATIC_DRAW);
      glEnableVertexAttribArray(NORMAL);
      glVertexAttribPointer(NORMAL, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }

    if (TexcoordsLoaded) {
      glBindBuffer(GL_ARRAY_BUFFER, boId[TEXCOORD]);
      glBufferData(GL_ARRAY_BUFFER, sizeof(Texcoords[0]) * Texcoords.size(),
                   &Texcoords[0], GL_STATIC_DRAW);
      glEnableVertexAttribArray(TEXCOORD);
      glVertexAttribPointer(TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, 0);
    }

    if (TangentsAndBitangentsLoaded) {
      glBindBuffer(GL_ARRAY_BUFFER, boId[TANGENT]);
      glBufferData(GL_ARRAY_BUFFER, sizeof(Tangents[0]) * Tangents.size(),
                   &Tangents[0], GL_STATIC_DRAW);
      glEnableVertexAttribArray(TANGENT);
      glVertexAttribPointer(TANGENT, 3, GL_FLOAT, GL_FALSE, 0, 0);

#ifdef CREATE_BITANGENT
      glBindBuffer(GL_ARRAY_BUFFER, boId[BITANGENT]);
      glBufferData(GL_ARRAY_BUFFER, sizeof(Bitangents[0]) * Bitangents.size(),
                   &Bitangents[0], GL_STATIC_DRAW);
      glEnableVertexAttribArray(BITANGENT);
      glVertexAttribPointer(BITANGENT, 3, GL_FLOAT, GL_FALSE, 0, 0);
#endif
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boId[INDEX]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices[0]) * Indices.size(),
                 &Indices[0], GL_STATIC_DRAW);

    ////////////////////// COLORS //////////////////////////////
    //glBindBuffer(GL_ARRAY_BUFFER, boId[COLOR]);
    //std::vector<glm::vec4> colors;
    //for (int i = 0; i < Positions.size(); i++) {
    //    colors.push_back(glm::vec4(1.f, 0.f, 0.f, 1.f));
    //}
    //glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(float) *colors.size(), &colors[0], GL_STATIC_DRAW);

    //glEnableVertexAttribArray(1);
    //glVertexAttribPointer(COLOR, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    //////////////////////////////////////////////////////////////////
  }
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glDeleteBuffers(buffNum, boId);
}

void Mesh::destroyBufferObjects() {
  glBindVertexArray(VaoId);
  glDisableVertexAttribArray(POSITION);
  glDisableVertexAttribArray(NORMAL);
  glDisableVertexAttribArray(TEXCOORD);
  glDisableVertexAttribArray(TANGENT);
#ifdef CREATE_BITANGENT
  glDisableVertexAttribArray(BITANGENT);
#endif
  glDisableVertexAttribArray(COLOR);
  glDeleteVertexArrays(1, &VaoId);
  glBindVertexArray(0);
}

void Mesh::draw() {
  glBindVertexArray(VaoId);
  for (MeshData &mesh : Meshes) {
    glDrawElementsBaseVertex(
        GL_TRIANGLES, mesh.nIndices, GL_UNSIGNED_INT,
        reinterpret_cast<void *>((sizeof(unsigned int) * mesh.baseIndex)),
        mesh.baseVertex);
  }
  glBindVertexArray(0);
}

void Mesh::setEffect(int e) {
    this->effect = e;
}

int Mesh::getEffect() {
    return effect;
}

json glmVec3ToJSON(glm::vec3 v) {
    json j = json::array();
    j.push_back(v.x);
    j.push_back(v.y);
    j.push_back(v.z);

    return j;
}

json glmVec2ToJSON(glm::vec2 v) {
    json j = json::array();
    j.push_back(v.x);
    j.push_back(v.y);

    return j;
}


//template <typename T>
//json vectorToJSON(std::vector<T> vec) {
//    json j = json::array();
//    for (int i = 0; i < vec.size(); i++) {
//        j.push_back(glmVecToString<T>(vec[i]));
//    }
//    return j;
//}

json vecOfGlmVec3ToJSON(std::vector<glm::vec3> vec) {
    json j = json::array();
    for (int i = 0; i < vec.size(); i++) {
        j.push_back(glmVec3ToJSON(vec[i]));
    }
    return j;
}

json vecOfGlmVec2ToJSON(std::vector<glm::vec2> vec) {
    json j = json::array();
    for (int i = 0; i < vec.size(); i++) {
        j.push_back(glmVec2ToJSON(vec[i]));
    }
    return j;
}

json vecOfIntToJSON(std::vector<unsigned int> vec) {
    json j = json::array();
    for (int i = 0; i < vec.size(); i++) {
        j.push_back(vec[i]);
    }
    return j;
}

json Mesh::vecOfMeshDataToJSON(std::vector<MeshData> vec) {
    json j = json::array();
    for (int i = 0; i < vec.size(); i++) {
        json jMeshData = json::object();
        jMeshData["nIndices"] = vec[i].nIndices;
        jMeshData["baseIndex"] = vec[i].baseIndex;
        jMeshData["baseVertex"] = vec[i].baseVertex;
        j.push_back(jMeshData);
    }
    return j;
}

json Mesh::toJSON() {
    json j;

    j["Positions"] = vecOfGlmVec3ToJSON(Positions);
    j["Normals"] = vecOfGlmVec3ToJSON(Normals);
    j["Texcoords"] = vecOfGlmVec2ToJSON(Texcoords);
    j["Tangents"] = vecOfGlmVec3ToJSON(Tangents);
    j["Bitangents"] = vecOfGlmVec3ToJSON(Bitangents);
    j["Indices"] = vecOfIntToJSON(Indices);
    j["Meshes"] = vecOfMeshDataToJSON(Meshes);
    return j;
}

std::vector<glm::vec3> toVecOfGlmVec3(json j) {
    std::vector<glm::vec3> result;
   
    for (json::iterator it = j.begin(); it != j.end(); ++it) {
        float vecArr[3] = { 0, 0, 0 };
        int i = 0;
        for (json::iterator it2 = it->begin(); it2 != it->end(); it2++) {
            vecArr[i++] = it2.value();
        }
        result.push_back(glm::vec3(vecArr[0], vecArr[1], vecArr[2]));
    }

    return result;
}

std::vector<glm::vec2> toVecOfGlmVec2(json j) {
    std::vector<glm::vec2> result;
    for (json::iterator it = j.begin(); it != j.end(); ++it) {
        float vecArr[2] = { 0, 0 };
        int i = 0;
        for (json::iterator it2 = it->begin(); it2 != it->end(); it2++) {
            vecArr[i++] = it2.value();
        }
        result.push_back(glm::vec2(vecArr[0], vecArr[1]));
    }
    return result;
}

std::vector<unsigned int> toVecOfUint(json j) {
    std::vector<unsigned int> result;
    for (int i = 0; i < j.size(); i++) {
        result.push_back(j[i]);
    }
    return result;
}

std::vector<Mesh::MeshData> Mesh::toVecOfMeshData(json j) {
    std::vector<MeshData> result;
    for (int i = 0; i < j.size(); i++) {
        MeshData md{};
        md.nIndices = j[i]["nIndices"];
        md.baseIndex = j[i]["baseIndex"];
        md.baseVertex = j[i]["baseVertex"];
        result.push_back(md);
    }
    return result;
}

void Mesh::fromJSON(json j) {
    Positions = toVecOfGlmVec3(j["Positions"]);
    Normals = toVecOfGlmVec3(j["Normals"]);
    Texcoords = toVecOfGlmVec2(j["Texcoords"]);
    Tangents = toVecOfGlmVec3(j["Tangents"]);
    Bitangents = toVecOfGlmVec3(j["Bitangents"]);
    Indices = toVecOfUint(j["Indices"]);
    Meshes = toVecOfMeshData(j["Meshes"]);
}

Transform* Mesh::getTransform() {
    return transform;
}
////////////////////////////////////////////////////////////////////////////////
}  // namespace mgl
