#pragma once

#include <iostream>

#include <stb/stb_image.h>

namespace Engine {

	class Texture {
	private:
		std::vector<char> texture;
	public:
		int size_x;
		int size_y;
		int channels;
		
		Texture(std::string path);
		
		void load_texture(std::string path);
		
		void clear();
	};

}
