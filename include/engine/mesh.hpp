#pragma once
#include <fstream>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <engine/rasterizer.hpp>
#include <engine/texture.hpp>

namespace Engine {

	struct model_vertice {
		glm::vec3 vertex;
		glm::vec3 normal;
		glm::vec2 uv;
		
		int material_id;
	};

	class Mesh {
	public:
		std::vector<model_vertice> vertices;
		glm::vec3 position, rotation, scale;
		
		std::vector<Texture> textures;
		
		void load_from_obj(std::string path);
		void load_mtl_textures(std::string path, std::string textures_path);
	
		glm::mat4 get_model();

		void draw();
	};

};
