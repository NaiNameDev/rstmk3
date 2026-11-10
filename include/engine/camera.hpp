#pragma once

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>

#include <engine/window.hpp>

namespace Engine {

	class Camera {
	public:
		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 forward;
		glm::vec3 up;

		Camera();

		glm::mat4 get_view();
		void camera_controll(float delta, Window& window);
		void mouse_controll(glm::vec2 velocity);
	};

};
