#pragma once

#include <engine/node.hpp>
#include <engine/window.hpp>

namespace Engine {

	class Scene {
	public:
		Node* root;
		Window window;

		Scene(unsigned int win_width, unsigned int win_height, std::string scene_name);

		void draw();
	};

};
