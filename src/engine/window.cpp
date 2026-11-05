#include <engine/window.hpp>

Engine::Window::Window(unsigned int nwidth, unsigned int nheight, std::string nname) {
	width = nwidth; height = nheight; name = nname;
}

void Engine::Window::create_window() {
	glfw_window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
	
	if (glfw_window == NULL) {
		glfwTerminate();
		std::cerr << "ERR: cant create window \"" << name << "\"\n";
		exit(-1);
	}

	glfwSetWindowSize(glfw_window, width, height);
	
	window_owner_table[glfw_window] = this;
	glfwSetFramebufferSizeCallback(glfw_window,
		[](GLFWwindow* win, int w, int h) {
			window_owner_table[win]->width = w;
			window_owner_table[win]->height = h;
			
			window_owner_table[win]->resize_event.emit(w, h);

			glViewport(0, 0, w, h);
		}
	);
	glfwSetCursorPosCallback(glfw_window, 
		[](GLFWwindow* win, double x, double y) {
			window_owner_table[win]->mouse_move_event.emit(x, y);
		}
	);
	glfwSetMouseButtonCallback(glfw_window, 
		[](GLFWwindow* win, int button, int action, int mods) {
			window_owner_table[win]->mouse_button_event.emit(button, action);
		}
	);
	glfwSetKeyCallback(glfw_window,
		[](GLFWwindow* win, int key, int scancode, int action, int mods) {
			window_owner_table[win]->key_press_event.emit(key, action);
		}
	);
}

void Engine::Window::destroy_window() {
	glfwDestroyWindow(glfw_window);
}

Engine::Window::~Window() {
	destroy_window();
}

void Engine::Window::make_current() {
	glfwMakeContextCurrent(glfw_window);
}

void Engine::Window::clear() {
	glClear(GL_COLOR_BUFFER_BIT);
}
void Engine::Window::swap_buffers() {
	glfwSwapBuffers(glfw_window);
	glfwPollEvents();
}
bool Engine::Window::should_close() {
	return glfwWindowShouldClose(glfw_window);
}
bool Engine::Window::is_action_pressed(unsigned int action) {
	return glfwGetKey(glfw_window, action) == GLFW_PRESS;
}
bool Engine::Window::is_action_just_pressed(unsigned int action) {
	if (key_map[action] == false && (glfwGetKey(glfw_window, action) == GLFW_PRESS)) {
		key_map[action] = true;
		return true;
	}
	if (glfwGetKey(glfw_window, action) == GLFW_RELEASE && key_map[action] == true) key_map[action] = false;
	return false;
}
