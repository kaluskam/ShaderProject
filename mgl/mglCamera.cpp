////////////////////////////////////////////////////////////////////////////////
//
// Camera Abstraction Class
//
// Copyright (c)2023 by Carlos Martinho
//
////////////////////////////////////////////////////////////////////////////////

#include "./mglCamera.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace mgl {

///////////////////////////////////////////////////////////////////////// Camera

Camera::Camera(GLuint bindingpoint)
    : viewMatrix(glm::mat4(1.0f)), projectionMatrix(glm::mat4(1.0f)) {
  glGenBuffers(1, &UboId);
  glBindBuffer(GL_UNIFORM_BUFFER, UboId);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, 0, GL_STREAM_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, bindingpoint, UboId);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

Camera::~Camera() {
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
  glDeleteBuffers(1, &UboId);
}

glm::mat4 Camera::getViewMatrix() { return viewMatrix; }

void Camera::setViewMatrix(const glm::mat4 &viewmatrix) {
  viewMatrix = viewmatrix;
  glBindBuffer(GL_UNIFORM_BUFFER, UboId);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4),
                  glm::value_ptr(viewMatrix));
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

glm::mat4 Camera::getProjectionMatrix() { return projectionMatrix; }

void Camera::setProjectionMatrix(const glm::mat4 &projectionmatrix) {
  projectionMatrix = projectionmatrix;
  glBindBuffer(GL_UNIFORM_BUFFER, UboId);
  glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4),
                  glm::value_ptr(projectionMatrix));
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

ProjectionType Camera::getProjectionType() { return projectionType; }

void Camera::setProjectionType(ProjectionType p) {
    projectionType = p;
    setProjectionMatrix(projectionMatrices[projectionType]);
    //states[activeStateId].ProjectionMatrix = projectionMatrices[projectionType];
}

ViewType Camera::getViewType() { return viewType; }

void Camera::setViewType(ViewType v) {
    viewType = v;
    setViewMatrix(viewMatrices[viewType].viewMatrix);

}

void Camera::addProjectionMatrix(glm::mat4 m, ProjectionType p) {
    projectionMatrices[p] = m;
}

void Camera::addViewMatrix(ViewMatrixInfo m, ViewType v) {
    viewMatrices[v] = m;
}

Camera::ViewMatrixInfo Camera::getViewMatrixInfo() {
    return viewMatrices[viewType];
}

float Camera::getZoom() {
    return zoom;
}

void Camera::zoomIn() {
    zoom += zoomDelta;
    applyZoom();
}

void Camera::zoomOut() {
    zoom -= zoomDelta;
    applyZoom();
}

void Camera::applyZoom() {
    ViewMatrixInfo* activeVM = &viewMatrices[viewType];
    activeVM->viewMatrix = glm::lookAt(activeVM->Eye * zoom, glm::vec3(0.f), activeVM->Up);
    setViewMatrix(activeVM->viewMatrix);
}

void Camera::rotate(glm::vec3 rotationAxis) {
    ViewMatrixInfo* activeVM = &viewMatrices[viewType];
    // EYE rotation
    activeVM->Eye = rotateVec(activeVM->Eye, rotationAxis);

    // UP rotation
    activeVM->Up = rotateVec(activeVM->Up, rotationAxis);

    activeVM->viewMatrix = glm::lookAt(activeVM->Eye * zoom, glm::vec3(0.0f, 0.0f, 0.0f),
        activeVM->Up);
    setViewMatrix(activeVM->viewMatrix);
}

glm::vec3 Camera::rotateVec(glm::vec3 v, glm::vec3 axis) {
    glm::quat PQ = glm::quat(0, v.x, v.y, v.z);
    glm::quat QQ = glm::quat(
        cos(glm::radians(rotationAngle) / 2), 
        sin(glm::radians(rotationAngle) / 2) * axis.x,
        sin(glm::radians(rotationAngle) / 2) * axis.y,
        sin(glm::radians(rotationAngle) / 2) * axis.z);
    glm::quat QpQ = QQ * PQ * glm::inverse(QQ);
    glm::vec3 result = glm::vec3(QpQ.x, QpQ.y, QpQ.z);

    return result;
}
////////////////////////////////////////////////////////////////////////////////
}  // namespace mgl
