#include <iostream>

#include <engine/engine.hpp>
#include <engine/node.hpp>
#include <engine/scene.hpp>

#include <engine/rasterizer.hpp>
namespace Engine { // bro i hate ts shit so much, butt with this i got +200 fps :3
#define FRAG _frag
#define VERT _vert

// place for your ((ads)) uniforms

FORCE_INLINE uint32_t _frag(const triangle& trg, const glm::vec3& weights, const glm::vec3& fragcoord) {
	return vec4_to_color(bary_interpolate_color(weights, glm::vec4(0,0,1,1), glm::vec4(0,1,0,1), glm::vec4(1,0,0,1)));
}
FORCE_INLINE glm::vec3 _vert(const glm::vec3& vertex) {
	return vertex;
}

#include <engine/rasterizer.inl>
};

#include <engine/framebuffer.hpp>

Engine::Engine engine;

void on_ready() {
	std::cout << "start!\n";
}
void process(float delta) {
	Engine::clear_buffers();

	Engine::rasterize(glm::vec3(0,0,2), glm::vec3(1919,0,2), glm::vec3(1919,1079,2), glm::vec3(0,0,0));

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
