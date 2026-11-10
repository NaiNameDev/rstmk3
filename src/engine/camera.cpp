#include <engine/camera.hpp>

Engine::Camera::Camera() {
	position = glm::vec3(0, 0, 0);
	rotation = glm::vec3(0, 0, 0);

	forward = glm::vec3(0, 0, -1);
	up = glm::vec3(0, 1, 0);
}

glm::mat4 Engine::Camera::get_view() {
	glm::mat4 rot_mat = glm::eulerAngleXYZ(rotation.x, rotation.y, rotation.z);
	return glm::lookAt(position, position + glm::vec3(glm::vec4(forward, 1.0f) * rot_mat), glm::vec3(glm::vec4(up, 1.0f) * rot_mat));
}
void Engine::Camera::mouse_controll(glm::vec2 velocity) {
	rotation.x += velocity.y * 0.003f;
	rotation.y += velocity.x * 0.003f;
	rotation.x = glm::clamp(rotation.x, glm::radians(-89.0f), glm::radians(89.0f));
}
void Engine::Camera::camera_controll(float delta, Window& window) {
	glm::mat4 rot_mat = glm::eulerAngleXYZ(rotation.x, rotation.y, rotation.z);
	glm::vec3 fwd = glm::vec3(glm::vec4(forward, 1.0f) * rot_mat);

	if (window.is_action_pressed(GLFW_KEY_W))
        position += fwd * delta;
	if (window.is_action_pressed(GLFW_KEY_S))
        position -= fwd * delta;
	if (window.is_action_pressed(GLFW_KEY_A))
        position -= glm::normalize(glm::cross(fwd, up)) * delta;
	if (window.is_action_pressed(GLFW_KEY_D))
        position += glm::normalize(glm::cross(fwd, up)) * delta;
	if (window.is_action_pressed(GLFW_KEY_SPACE))
        position += glm::vec3(0.0f, 1.0f, 0.0f) * delta;
	if (window.is_action_pressed(GLFW_KEY_LEFT_SHIFT))
        position += glm::vec3(0.0f, -1.0f, 0.0f) * delta;
}
