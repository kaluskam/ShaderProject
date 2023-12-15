////////////////////////////////////////////////////////////////////////////////
//
// Camera Abstraction Class
//
// Copyright (c)2023 by Carlos Martinho
//
////////////////////////////////////////////////////////////////////////////////

#ifndef MGL_CAMERA_HPP
#define MGL_CAMERA_HPP

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <map>
#include <vector>

namespace mgl {

class Camera;

///////////////////////////////////////////////////////////////////////// Camera
enum ProjectionType {
    PERSPECTIVE, ORTHOGRAPHIC
};

enum ViewType {
    FROM_Y, FROM_Z
};

class Camera {
public:
    struct ViewMatrixInfo {
        glm::vec3 Eye;
        glm::vec3 Up;
        glm::mat4 viewMatrix;
    };

 private:
    std::map<ProjectionType, glm::mat4> projectionMatrices;
    std::map<ViewType, ViewMatrixInfo> viewMatrices;

    float zoom = 1.f;
    float zoomDelta = 0.05f;

    float rotationAngle = 2;

    ProjectionType projectionType = PERSPECTIVE;
    ViewType viewType = FROM_Y;
    GLuint UboId;
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix; 

 public:
  explicit Camera(GLuint bindingpoint);
  virtual ~Camera();
  glm::mat4 getViewMatrix();
  void setViewMatrix(const glm::mat4 &viewmatrix);
  glm::mat4 getProjectionMatrix();
  void setProjectionMatrix(const glm::mat4 &projectionmatrix);
  ProjectionType getProjectionType();
  void setProjectionType(ProjectionType);
  ViewType getViewType();
  void setViewType(ViewType);
  void addProjectionMatrix(glm::mat4, ProjectionType);
  void addViewMatrix(ViewMatrixInfo, ViewType);
  ViewMatrixInfo getViewMatrixInfo();
  float getZoom();
  void zoomIn();
  void zoomOut();
  void rotate(glm::vec3);

private:
  void applyZoom();
  glm::vec3 rotateVec(glm::vec3, glm::vec3);
};

////////////////////////////////////////////////////////////////////////////////
}  // namespace mgl

#endif /* MGL_CAMERA_HPP */
