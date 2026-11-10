#include <engine/mesh.hpp>

void Engine::Mesh::load_from_obj(std::string path) {
	vertices.clear();

	std::vector<float> tv = {}; //vertexes
	std::vector<float> tn = {}; //noramls
	std::vector<float> tu = {}; //UVs
	unsigned int material_ids_cnt = -1;

	char mod = 'S'; // S = start
	std::vector<std::string> key_words = {"o", "f", "vn", "vt", "v", "usemtl", "s", "#", "g"};
	std::string key_words_mod = "ofntvm   ";
	std::string word = "";

	std::ifstream t(path);
	t.seekg(0, std::ios::end);
	size_t size = t.tellg();
	std::string file(size, ' ');
	t.seekg(0);
	t.read(&file[0], size); 

	for(int i = 0; i < file.length(); i++) {
		if (file[i] == ' ' || file[i] == '\n') {
			if (word != "") {
				int sk = 0;
				for (int j = 0; j < key_words.size(); j++) {
					if (word == key_words[j]) {
						mod = key_words_mod[j];
						sk = 1;
					}
				}
				
				if (sk == 0) {
					switch(mod) {
						case 'm':
							material_ids_cnt++;
							break;
						case 'o':
							break;
						case 'v':
							tv.push_back(std::stof(word));
							break;
						case 'n':
							tn.push_back(std::stof(word));
							break;
						case 't':
							tu.push_back(std::stof(word));
							break;
						case 'f':
							std::vector<int> tmp_nums = {};
							std::string number = "";

							for (char ch : word) {
								if (ch != '/') {number.push_back(ch); continue;}
								tmp_nums.push_back(std::stoi(number)); number = "";
							}
							tmp_nums.push_back(std::stoi(number));
							
							model_vertice new_vert;
							
							//vertexes
							new_vert.vertex.x = (tv[tmp_nums[0] * 3 - 3]);
							new_vert.vertex.y = (tv[tmp_nums[0] * 3 - 2]);
							new_vert.vertex.z = (tv[tmp_nums[0] * 3 - 1]);

							//normals
							new_vert.normal.x = (tn[tmp_nums[2] * 3 - 3]);
							new_vert.normal.y = (tn[tmp_nums[2] * 3 - 2]);
							new_vert.normal.z = (tn[tmp_nums[2] * 3 - 1]);
							
							//UVs
							new_vert.uv.x = (tu[tmp_nums[1] * 2 - 2]);
							new_vert.uv.y = (tu[tmp_nums[1] * 2 - 1]);

							//material_id
							new_vert.material_id = material_ids_cnt;
							
							vertices.push_back(new_vert);

							break;
					}
				}
				word = "";
			}
		}
		else word.push_back(file[i]);
	}
}

void Engine::Mesh::load_mtl_textures(std::string path, std::string textures_path) {
	std::ifstream t(path);
	t.seekg(0, std::ios::end);
	size_t size = t.tellg();
	std::string file(size, ' ');
	t.seekg(0);
	t.read(&file[0], size); 
	
	int found = 0;
	std::string word;

	for(int i = 0; i < file.length(); i++) {
		if (file[i] == ' ' || file[i] == '\n') {
			if (word != "") {
				if (found) {
					Texture tx(textures_path + word);
					textures.push_back(tx);
					found = 0;
				}
				if (word == "map_Kd") {
					found = 1;
				}
			}

			word = "";
		}
		else word.push_back(file[i]);
	}
}

glm::mat4 Engine::Mesh::get_model() {
	return glm::scale(glm::translate(glm::mat4(1.0f), position), scale) * glm::mat4_cast(glm::quat(rotation));
}

void Engine::Mesh::draw() {
	for (int i = 3; i < vertices.size() + 1; i+=3) {
		rasterize(vertices[i - 3].vertex, vertices[i - 2].vertex, vertices[i - 1].vertex, vertices[i-1].normal);
	}
}
