#include <iostream>

#include <engine/engine.hpp>
#include <engine/node.hpp>
#include <engine/scene.hpp>

#include <engine/rasterizer.hpp>
#include <engine/framebuffer.hpp>

Engine::Engine engine;

// shaders
//global vars
float u_time = 0.0f;
uint32_t flat_color = 0xFFFFFFFF;
FORCE_INLINE uint32_t rainbow(const Engine::triangle& trg, const glm::vec3& weights, const glm::vec3& fragcoord) {
	return Engine::vec4_to_color(Engine::bary_interpolate_color(weights, glm::vec4(0,0,1,1), glm::vec4(0,1,0,1), glm::vec4(1,0,0,1)));
}
FORCE_INLINE uint32_t flat(const Engine::triangle& trg, const glm::vec3& weights, const glm::vec3& fragcoord) {
	return flat_color;
}
glm::vec3 vert_sin(const glm::vec3& point) {
	return point + glm::vec3(sin(u_time) * 80.0f, cos(u_time) * 80.0f, 0.0f);
}
glm::vec3 vert_static(const glm::vec3& point) {
	return point;
}
auto f1 = GEN_FRAGMENT(rainbow);
auto f2 = GEN_FRAGMENT(flat);

void on_ready() {
	Engine::fragment = f1;
	std::cout << "start!\n";
}
void process(float delta) {
	Engine::clear_buffers();

	Engine::vertex = vert_static;
	u_time = glfwGetTime();
	flat_color = 0xFFFFFFFF;
	Engine::rasterize(
		glm::vec3(engine.current_scene->window.width - 1, 0, 2), 
		glm::vec3(engine.current_scene->window.width - 1, engine.current_scene->window.height - 1, 2),
		glm::vec3(0, engine.current_scene->window.height - 1, 2), glm::vec3(1,1,1)
	);
	flat_color = 0xFFFF00FF;
	Engine::rasterize(
		glm::vec3(0, 0,1), 
		glm::vec3(engine.current_scene->window.width - 1, 0,1),
		glm::vec3(0, engine.current_scene->window.height - 1,1), glm::vec3(0,0,0)
	);
	flat_color = 0xFF00FF00;
	Engine::vertex = vert_sin;
	Engine::rasterize(
		glm::vec3(100, 200, 2),
		glm::vec3(100, 100, 2),
		glm::vec3(200, 150, 2), 
		glm::vec3(0,0,0)
	);

	glDrawPixels(engine.current_scene->window.width, engine.current_scene->window.height, GL_RGBA, GL_UNSIGNED_BYTE, (void*)Engine::fbo);

	engine.print_fps_info();
}
void on_quit() {
	Engine::free_buffers();

	std::cout << "end!\n";
}

int main() {
	engine.change_scene(new Engine::Scene(1280, 720, "main scene window"));
	Engine::init_buffers(engine.current_scene->window.width, engine.current_scene->window.height);

	engine.ready_signal.connect(on_ready);
	engine.process_signal.connect(process);
	engine.quit_signal.connect(on_quit);
	
	engine.run();
}
