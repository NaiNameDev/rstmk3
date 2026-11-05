#include <engine/engine.hpp>

Engine::Engine::Engine()
	: early_exit(false), current_scene(nullptr) {
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW\n";
		exit(1);
	}
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
}

Engine::Engine::~Engine() {
	glfwTerminate();
}

void Engine::Engine::change_scene(Scene* ptr) {
	current_scene.reset(ptr);
}

void Engine::Engine::run() {
	last = glfwGetTime();
	
	ready_signal.emit();
	while(!current_scene->window.should_close()) {
		current = (float)glfwGetTime();
		delta = current - last;
		last = current;

		current_scene->window.clear();
		
		process_signal.emit(delta);

		current_scene->window.swap_buffers();
		
		if (early_exit) break;
	}
	quit_signal.emit();
}

void Engine::Engine::quit() {
	early_exit = true;
}

void Engine::Engine::print_fps_info() {
	if (delta > 0.001f) {
		std::cout << "FPS: " << 1.0f / delta << ", DELTA: " << delta << "\n";
	}
}
