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

FORCE_INLINE void fill_area(const triangle& trg, const std::vector<std::array<int16_t, 2>>& area, int16_t min_y) {
	for (int i = 0; i < area.size(); i++) {
		int length = area[i][MAX] - area[i][MIN] + 1;

		// FRAGMENT "SHADER" CALLS
		if (length == 0) {
			glm::ivec2 pos = glm::ivec2(area[i][MIN], min_y + i);
			glm::vec3 weights = bary_weights(trg, pos);
			float depth = bary_interpolate_float(weights, trg.p1d, trg.p2d, trg.p3d);

			if (get_depth_pixel(pos.x, pos.y) < depth) {
				set_pixel(pos.x, pos.y, FRAG(trg, weights, glm::vec3(pos.x, pos.y, depth)));
				set_depth_pixel(pos.x, pos.y, depth);
			}
			continue;
		}
		for (int j = 0; j < length; j++) {
			glm::ivec2 pos = glm::ivec2(area[i][MIN] + j, min_y + i);
			glm::vec3 weights = bary_weights(trg, pos);
			float depth = bary_interpolate_float(weights, trg.p1d, trg.p2d, trg.p3d);

			if (get_depth_pixel(pos.x, pos.y) < depth) {
				set_pixel(pos.x, pos.y, FRAG(trg, weights, glm::vec3(pos.x, pos.y, depth)));
				set_depth_pixel(pos.x, pos.y, depth);
			}
		}
	}
}

FORCE_INLINE void rasterize(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 norm) {
	std::vector<triangle> draw;
	// form 3d to clipspace
	p1 = VERT(p1); p2 = VERT(p2); p3 = VERT(p3);
	triangle ftrg = (triangle){p1, p2, p3, p1.z, p2.z, p3.z, norm};
	draw.push_back(ftrg);

	// add culling here :3

	// draw
	for (auto& trg : draw) {
		int16_t min = std::min(trg.p1.y, std::min(trg.p2.y, trg.p3.y));
		int16_t max = std::max(trg.p1.y, std::max(trg.p2.y, trg.p3.y));
		
		std::vector<std::array<int16_t, 2>> triangle_area(max - min + 1, std::array<int16_t, 2>{32767, -32767});
	
		calc_line(trg.p1, trg.p2, min, triangle_area);
		calc_line(trg.p2, trg.p3, min, triangle_area);
		calc_line(trg.p3, trg.p1, min, triangle_area);
		
		fill_area(trg, triangle_area, min);
	}
}
