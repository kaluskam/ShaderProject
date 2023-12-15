#ifndef MGL_TRANSFORM_HPP
#define MGL_TRANSFORM_HPP

#include <glm/glm.hpp>

namespace mgl {
	class Transform {

	public:
		glm::vec3 translate;
		glm::vec3 rotation;
		glm::vec3 scale;

		Transform();
		virtual ~Transform();

		void setTranslate(glm::vec3 t);
		void setRotation(glm::vec3 r);
		void setScale(glm::vec3 s);

	};
}
#endif // !MGL_TRANSFORM_HPP
