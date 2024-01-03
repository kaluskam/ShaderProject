#ifndef MGL_TRANSFORM_HPP
#define MGL_TRANSFORM_HPP

#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"

namespace mgl {
	class Transform {

	public:
		glm::vec3 translate = glm::vec3(0, 0, 0);
		float rotationDegrees = 0;
		glm::vec3 rotationAxis = glm::vec3(0, 1, 0);
		glm::vec3 scale = glm::vec3(1.0f);
		glm::mat4 modelMatrix = glm::mat4(1.0f);

		Transform() {}
		virtual ~Transform() {}

		void setTranslate(glm::vec3 t);
		void setRotation(float degrees, glm::vec3 axis);
		void setScale(glm::vec3 s);
		void calculateModelMatrix();
		glm::mat4 getModelMatrix();

	};
}
#endif // !MGL_TRANSFORM_HPP
