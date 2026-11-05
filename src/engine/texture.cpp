#include <engine/texture.hpp>

Engine::Texture::Texture(std::string path) {load_texture(path);}

void Engine::Texture::load_texture(std::string path) {
	unsigned char* tmp_texture = stbi_load(path.c_str(), &size_x, &size_y, &channels, 0);
	if (tmp_texture == NULL) {
		std::cerr << "ERR: Failed to load texture" << std::endl;
		exit(1);
	}
	texture = std::vector<char>(tmp_texture, tmp_texture + (size_x * size_y * channels));
	free(tmp_texture);
}
