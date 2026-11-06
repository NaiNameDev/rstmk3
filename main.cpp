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
FORCE_INLINE uint32_t frag(const Engine::triangle& trg, const glm::vec3& weights, const glm::vec3& fragcoord) {
	return Engine::vec4_to_color(Engine::bary_interpolate_color(weights, glm::vec4(0,0,1,1), glm::vec4(0,1,0,1), glm::vec4(1,0,0,1)));
}
glm::vec3 vert(const glm::vec3& point) {
	return point;
}
auto f1 = GEN_FRAGMENT(frag);

void on_ready() {
	Engine::fragment = f1;
	Engine::vertex = vert;
	std::cout << "start!\n";
}
void process(float delta) {
	Engine::clear_buffers();

	u_time = glfwGetTime();
	//Engine::rasterize(glm::vec3(100, 200, 1), glm::vec3(100, 100, 1), glm::vec3(200, 150, 1),  glm::vec3(0,0,0));
	Engine::rasterize(
		glm::vec3(engine.current_scene->window.width - 1, 0,1), 
		glm::vec3(engine.current_scene->window.width - 1, engine.current_scene->window.height - 1,1),
		glm::vec3(0, engine.current_scene->window.height - 1,1), glm::vec3(0,0,0)
	);
	Engine::rasterize(
		glm::vec3(0, 0,1), 
		glm::vec3(engine.current_scene->window.width - 1, 0,1),
		glm::vec3(0, engine.current_scene->window.height - 1,1), glm::vec3(0,0,0)
	);

	glDrawPixels(engine.current_scene->window.width, engine.current_scene->window.height, GL_RGBA, GL_UNSIGNED_BYTE, (void*)Engine::fbo);

	engine.print_fps_info();
}
void on_quit() {
	Engine::free_buffers();

	std::cout << "end!\n";
}

int main() {
	engine.change_scene(new Engine::Scene(1920, 1080, "main scene window"));
	Engine::init_buffers(engine.current_scene->window.width, engine.current_scene->window.height);

	engine.ready_signal.connect(on_ready);
	engine.process_signal.connect(process);
	engine.quit_signal.connect(on_quit);
	
	engine.run();
}
