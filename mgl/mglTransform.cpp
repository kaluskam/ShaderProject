#include "mglTransform.hpp"
#include <glm/glm.hpp>
#include <GL/glew.h>

namespace mgl {
	void Transform::setTranslate(glm::vec3 t) {
		translate = t;
	}

	void Transform::setRotation(float degrees, glm::vec3 axis) {
		rotationAxis = axis;
		rotationDegrees = degrees;
	}

	void Transform::setScale(glm::vec3 s) {
		scale = s;
	}

	void Transform::calculateModelMatrix() {
		glm::mat4 T = glm::translate(glm::mat4(1.f), translate);
		glm::mat4 R = glm::rotate(T, glm::radians(rotationDegrees), rotationAxis);
		
		this->modelMatrix = glm::scale(R, scale);
	}

	glm::mat4 Transform::getModelMatrix() {
		return modelMatrix;
	}
}