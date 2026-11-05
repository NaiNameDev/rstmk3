#pragma once

#include <functional>
#include <memory>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <engine/scene.hpp>
#include <engine/signal.hpp>

#define FORCE_INLINE inline __attribute__((always_inline))

namespace Engine {

	class Engine final {
	private:
		float last; float current; float delta;

	public:
		Signal<> ready_signal;
		Signal<float> process_signal;
		Signal<> quit_signal;
		
		std::unique_ptr<Scene> current_scene;

		Engine();
		~Engine();

		void change_scene(Scene* ptr);
		void run();
		
		bool early_exit;
		void quit();

		void print_fps_info();
	};

};
