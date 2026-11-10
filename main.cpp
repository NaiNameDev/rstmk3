#include <iostream>

#include <glm/gtc/matrix_transform.hpp>

#include <engine/engine.hpp>
#include <engine/node.hpp>
#include <engine/scene.hpp>
#include <engine/framebuffer.hpp>
#include <engine/rasterizer.hpp>
#include <engine/camera.hpp>
#include <engine/mesh.hpp>

Engine::Engine engine;

// shaders
// global vars
float u_time = 0.0f;
int32_t clear_color = 0xFFDDBBCC;

glm::mat4 fin_mat;

FORCE_INLINE uint32_t rainbow(const Engine::triangle& trg, const glm::vec3& weights, const glm::vec3& fragcoord) {
	return Engine::vec4_to_color(Engine::bary_interpolate_color(weights, glm::vec4(0,0,1,1), glm::vec4(0,1,0,1), glm::vec4(1,0,0,1)));
}
FORCE_INLINE uint32_t overdraw(const Engine::triangle& trg, const glm::vec3& weights, const glm::vec3& fragcoord) {
	if (Engine::get_pixel(fragcoord.x, fragcoord.y) != clear_color) return 0xFF0000FF;
	return 0xFFFFFFFF;
}
glm::vec3 vert_static(glm::vec3 point) {
	glm::vec4 p = fin_mat * glm::vec4(point, 1.0f);
	if (p.w < Engine::near) p.w = Engine::near;
	return glm::vec3(p.x / p.w, p.y / p.w, p.z / p.w);
}
auto f1 = GEN_FRAGMENT(rainbow);
auto f2 = GEN_FRAGMENT(overdraw);



Engine::Mesh teto;
Engine::Camera main_camera;

glm::mat4 screen_mat;

glm::vec2 old;
void mouse_input(double x, double y) {
	main_camera.mouse_controll(glm::vec2(x, y) - old);
	old = glm::vec2(x, y);
}

void on_ready() {
	glfwSetInputMode(engine.current_scene->window.glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	engine.current_scene->window.mouse_move_event.connect(mouse_input);

	Engine::init_buffers(engine.current_scene->window.width, engine.current_scene->window.height, 0.01f, 100.0f);
	Engine::fragment = f1;
	Engine::vertex = vert_static;
	
	glm::mat4 proj = glm::perspective(
		glm::radians(100.0f), // <- fov
		(float)engine.current_scene->window.width / (float)engine.current_scene->window.height,
		Engine::near,
		Engine::far
	);
	glm::mat4 to_screen = glm::mat4(1.0f);
	to_screen[0][0] = engine.current_scene->window.width / 2.0f;
	to_screen[1][1] = engine.current_scene->window.height / 2.0f;
	to_screen[3][0] = to_screen[0][0];
	to_screen[3][1] = to_screen[1][1];
	to_screen[2][2] = (Engine::far - Engine::near) / 2.0f;
	to_screen[3][2] = (Engine::far + Engine::near) / 2.0f;

	screen_mat = to_screen * proj;

	teto.load_from_obj("./test/obj/tetokasane.obj");
	teto.scale = glm::vec3(0.005f, 0.005f, 0.005f);
	teto.position.z = -0.7f;
	teto.position.y = -0.3f;

	std::cout << "start!\n";
}
void process(float delta) {
	Engine::clear_buffers(clear_color);
	
	main_camera.camera_controll(delta * 0.3f, engine.current_scene->window);

	fin_mat = screen_mat * main_camera.get_view() * teto.get_model();
	teto.draw();

	glDrawPixels(engine.current_scene->window.width, engine.current_scene->window.height, GL_RGBA, GL_UNSIGNED_BYTE, (void*)Engine::fbo);

	engine.print_fps_info();
}
void on_quit() {
	Engine::free_buffers();

	std::cout << "end!\n";
}

int main() {
	engine.change_scene(new Engine::Scene(1280, 720, "main scene window"));

	engine.ready_signal.connect(on_ready);
	engine.process_signal.connect(process);
	engine.quit_signal.connect(on_quit);
	
	engine.run();
}
