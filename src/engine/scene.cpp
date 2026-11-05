#include <engine/scene.hpp>

Engine::Scene::Scene(unsigned int win_width, unsigned int win_height, std::string scene_name) : window(win_width, win_height, scene_name) {
	window.create_window();
	window.make_current();
}
