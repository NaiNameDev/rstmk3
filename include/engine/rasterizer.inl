FORCE_INLINE uint32_t vec4_to_color(glm::vec4 vec) {
	return ((uint32_t)(unsigned char)glm::clamp(vec.w*255, 0.0f, 255.0f) << 24) |
           ((uint32_t)(unsigned char)glm::clamp(vec.z*255, 0.0f, 255.0f) << 16) |
           ((uint32_t)(unsigned char)glm::clamp(vec.y*255, 0.0f, 255.0f) <<  8) |
           ((uint32_t)(unsigned char)glm::clamp(vec.x*255, 0.0f, 255.0f));
}
FORCE_INLINE glm::vec4 color_to_vec4(uint32_t color) {
	return glm::vec4((color & 0xFF) / 255.0f, ((color >> 8) & 0xFF) / 255.0f, ((color >> 16) & 0xFF) / 255.0f, ((color >> 24) & 0xFF) / 255.0f);
}

FORCE_INLINE glm::vec3 bary_weights(const triangle& trg, const glm::ivec2& point) {
	glm::vec2 v0 = trg.p2 - trg.p1, v1 = trg.p3 - trg.p1, v2 = point - trg.p1;
	
	float den = 1.0f / (v0.x * v1.y - v1.x * v0.y);
	float v = (v2.x * v1.y - v1.x * v2.y) * den;
	float w = (v0.x * v2.y - v2.x * v0.y) * den;
	float u = 1.0f - v - w;	
	
	return glm::vec3(u, v, w);
}
FORCE_INLINE glm::vec3 bary_weights(const glm::ivec2& p1, const glm::ivec2& p2, const glm::ivec2& p3, const glm::ivec2& point) {
	glm::vec2 v0 = p2 - p1, v1 = p3 - p1, v2 = point - p1;
	float den = 1.0f / (v0.x * v1.y - v1.x * v0.y);
	float v = (v2.x * v1.y - v1.x * v2.y) * den;
	float w = (v0.x * v2.y - v2.x * v0.y) * den;
	float u = 1.0f - v - w;	
	
	return glm::vec3(u, v, w);
}

FORCE_INLINE float bary_interpolate_float(const glm::vec3& weights, float a, float b, float c) {
	return a * weights.x + b * weights.y + c * weights.z;
}
FORCE_INLINE glm::vec4 bary_interpolate_color(const glm::vec3& weights, const glm::vec4& a, const glm::vec4& b, const glm::vec4& c) {
	return glm::vec4(a.x * weights.x + b.x * weights.y + c.x * weights.z, 
					a.y * weights.x + b.y * weights.y + c.y * weights.z,
					a.z * weights.x + b.z * weights.y + c.z * weights.z,
					a.w * weights.x + b.w * weights.y + c.w * weights.z);
}
FORCE_INLINE glm::ivec2 bary_get_coord_inside(const glm::vec3& weights, const glm::ivec2& p1, const glm::ivec2& p2, const glm::ivec2& p3) {
	return glm::ivec2(weights.x * p1.x + weights.y * p2.x + weights.z * p3.x,
					 weights.x * p1.y + weights.y * p2.y + weights.z * p3.y);
}
FORCE_INLINE glm::ivec2 bary_get_coord_inside(const glm::vec3& weights, const triangle& trg) {
	return glm::ivec2(weights.x * trg.p1.x + weights.y * trg.p2.x + weights.z * trg.p3.x,
					 weights.x * trg.p1.y + weights.y * trg.p2.y + weights.z * trg.p3.y);
}

FORCE_INLINE void calc_line(glm::ivec2 p1, glm::ivec2 p2, int16_t min_y, std::vector<std::array<int16_t, 2>>& area) {
	int dx = abs(p1.x - p2.x);
	int dy = abs(p1.y - p2.y);
	int sx = (p1.x < p2.x ? 1 : -1);
	int sy = (p1.y < p2.y ? 1 : -1);
	int er = dx - dy;

	while (1) {
		area[p1.y - min_y][MIN] = std::min((int16_t)p1.x, area[p1.y - min_y][MIN]);
		area[p1.y - min_y][MAX] = std::max((int16_t)p1.x, area[p1.y - min_y][MAX]);
	
		if (p1.x == p2.x && p1.y == p2.y) return;
		
		int e2 = 2 * er;
		if (e2 > -dy) {
			er -= dy;
			p1.x += sx;
		}
		if (e2 < dx) {
			er += dx;
			p1.y += sy;
		}
	}
} 

#define GEN_FRAGMENT(FRAG)\
std::function<void(const Engine::triangle& trg, const std::vector<std::array<int16_t, 2>>& area, int16_t min_y)>\
	([](const Engine::triangle& trg, const std::vector<std::array<int16_t, 2>>& area, int16_t min_y) {\
		int size = area.size();\
	 	for (int i = 0; i < size; i++) {\
			for (int j = area[i][MIN]; j < area[i][MAX]; j++) {\
				glm::vec3 weights = Engine::bary_weights(trg.p1, trg.p2, trg.p3, glm::vec2(j, i + min_y));\
\
				float depth_old = Engine::get_depth_pixel(j, i + min_y);\
				float depth = Engine::bary_interpolate_float(weights, trg.p1d, trg.p2d, trg.p3d);\
				uint32_t color_old = Engine::get_pixel(j, i + min_y);\
				uint32_t color = FRAG(trg, weights, glm::vec3(j, i + min_y, depth));\
\
				Engine::set_pixel(j, i + min_y, color);\
				Engine::set_depth_pixel(j, i + min_y, depth);\
				if (depth_old > depth) {\
					Engine::set_pixel(j, i + min_y, color_old);\
					Engine::set_depth_pixel(j, i + min_y, depth_old);\
				}\
			}\
		}\
	}\
)

FORCE_INLINE void full_culling(const std::vector<triangle>& draw) {
	for (auto& trg : draw) {
		
	}
}

FORCE_INLINE void rasterize(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 norm) {
	std::vector<triangle> draw;
	p1 = vertex(p1); p2 = vertex(p2); p3 = vertex(p3);
	triangle ftrg = (triangle){p1, p2, p3, p1.z, p2.z, p3.z, norm};
	draw.push_back(ftrg);

	// viewport backface culling
	if (glm::cross(glm::vec3(ftrg.p2, 0.0f) - glm::vec3(ftrg.p1, 0.0f), glm::vec3(ftrg.p3, 0.0f) - glm::vec3(ftrg.p1, 0.0f)).z <= 0.0f) return;
	full_culling(draw);

	// draw
	for (auto& trg : draw) {
		int16_t min = std::min(trg.p1.y, std::min(trg.p2.y, trg.p3.y));
		int16_t max = std::max(trg.p1.y, std::max(trg.p2.y, trg.p3.y));
		
		std::vector<std::array<int16_t, 2>> triangle_area(max - min + 1, std::array<int16_t, 2>{32767, -32767});
	
		calc_line(trg.p1, trg.p2, min, triangle_area);
		calc_line(trg.p2, trg.p3, min, triangle_area);
		calc_line(trg.p3, trg.p1, min, triangle_area);
		
		fragment(trg, triangle_area, min);
	}
}
