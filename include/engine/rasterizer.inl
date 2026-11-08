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

FORCE_INLINE int is_out(int16_t point, int16_t clamp_min, int16_t clamp_max) {
	if (point < clamp_min) return LEFT_OUT;
	if (point > clamp_max) return RIGHT_OUT;
	return INSIDE;
}
FORCE_INLINE int f_is_out(float point, float clamp_min, float clamp_max) {
	if (point < clamp_min) return LEFT_OUT;
	if (point > clamp_max) return RIGHT_OUT;
	return INSIDE;
}
FORCE_INLINE glm::ivec2 colliding_point_x(glm::ivec2 p1, glm::ivec2 p2, int16_t clamp) {
	return glm::ivec2(clamp, p1.y + ((clamp - p1.x) * (p2.y - p1.y) / (p2.x - p1.x)));	
}
FORCE_INLINE glm::ivec2 colliding_point_y(glm::ivec2 p1, glm::ivec2 p2, int16_t clamp) {
	return glm::ivec2(p1.x + ((clamp - p1.y) * (p2.x - p1.x) / (p2.y - p1.y)), clamp);	
}
FORCE_INLINE glm::ivec2 colliding_point_z(glm::ivec2 p1, glm::ivec2 p2, float z1, float z2, float clamp) {
	return glm::ivec2(p1.x + ((clamp - z1) * (p2.x - p1.x) / (z2 - z1)),
					  p1.y + ((clamp - z1) * (p2.y - p1.y) / (z2 - z1)));	
}
FORCE_INLINE void interpolate_with_base(triangle& trg, const triangle& base) {
	glm::vec3 weights_p1 = bary_weights(base, trg.p1);
	glm::vec3 weights_p2 = bary_weights(base, trg.p2);
	glm::vec3 weights_p3 = bary_weights(base, trg.p3);

	trg.p1d = bary_interpolate_float(weights_p1, base_trg.p1d, base_trg.p2d, base_trg.p3d);
	trg.p2d = bary_interpolate_float(weights_p2, base_trg.p1d, base_trg.p2d, base_trg.p3d);
	trg.p3d = bary_interpolate_float(weights_p3, base_trg.p1d, base_trg.p2d, base_trg.p3d);
}
FORCE_INLINE bool triangulate(
	const std::vector<glm::ivec2>& out,
	const std::vector<glm::ivec2>& in,
	std::vector<triangle>& draw,
	triangle base_trg,
	int16_t clamp,
	int16_t basis
) {
	if (out.size() == 1) {
		triangle new_trg1 = base_trg;
		triangle new_trg2 = base_trg;
		
		if (basis == 0) {
			new_trg1.p1 = colliding_point_x(out[0], in[0], clamp);
			new_trg1.p2 = colliding_point_x(out[0], in[1], clamp);
		}
		else if(basis == 1) {
			new_trg1.p1 = colliding_point_y(out[0], in[0], clamp);
			new_trg1.p2 = colliding_point_y(out[0], in[1], clamp);
		}
		new_trg1.p3 = in[0];

		new_trg2.p1 = new_trg1.p2;
		new_trg2.p2 = in[0];
		new_trg2.p3 = in[1];
		
		glm::vec3 weights_p1 = bary_weights(base_trg, new_trg1.p1);
		glm::vec3 weights_p2 = bary_weights(base_trg, new_trg1.p2);
		glm::vec3 weights_p3 = bary_weights(base_trg, new_trg1.p3);

		new_trg1.p1d = bary_interpolate_float(weights_p1, base_trg.p1d, base_trg.p2d, base_trg.p3d);
		new_trg1.p2d = bary_interpolate_float(weights_p2, base_trg.p1d, base_trg.p2d, base_trg.p3d);
		new_trg1.p3d = bary_interpolate_float(weights_p3, base_trg.p1d, base_trg.p2d, base_trg.p3d);
		
		glm::vec3 weights_p1_2 = bary_weights(base_trg, new_trg2.p1); 
		glm::vec3 weights_p2_2 = weights_p3;
		glm::vec3 weights_p3_2 = bary_weights(base_trg, new_trg2.p3);
		
		new_trg2.p1d = bary_interpolate_float(weights_p1_2, base_trg.p1d, base_trg.p2d, base_trg.p3d);
		new_trg2.p2d = bary_interpolate_float(weights_p2_2, base_trg.p1d, base_trg.p2d, base_trg.p3d);
		new_trg2.p3d = bary_interpolate_float(weights_p3_2, base_trg.p1d, base_trg.p2d, base_trg.p3d);

		draw.push_back(new_trg1);
		draw.push_back(new_trg2);
		
		return true;
	}
	if (out.size() == 2) {
		triangle new_trg = base_trg;
		
		if (basis == 0) {
			new_trg.p1 = colliding_point_x(in[0], out[0], clamp);
			new_trg.p2 = colliding_point_x(in[0], out[1], clamp);
		}
		else if (basis == 1) {
			new_trg.p1 = colliding_point_y(in[0], out[0], clamp);
			new_trg.p2 = colliding_point_y(in[0], out[1], clamp);
		}
		new_trg.p3 = in[0];
		
		draw.push_back(new_trg);
		
		return true;
	}

	return false;
}

FORCE_INLINE void full_culling(std::vector<triangle>& draw, int basis) {
	for (int i = 0; i < draw.size(); i++) {
		triangle base_trg = draw[i];

		glm::vec3 points[3] = {
			base_trg.p1,
			base_trg.p2,
			base_trg.p3
		};
		int out_points[3];
		if (basis == 0) for (int j = 0; j < 3; j++) out_points[j] = is_out(points[j].x, 200, fbo_width - 200);
		if (basis == 1) for (int j = 0; j < 3; j++) out_points[j] = is_out(points[j].y, 200, fbo_height - 200);
		if (out_points[0] == INSIDE && out_points[1] == INSIDE && out_points[2] == INSIDE) continue;
		
		draw.erase(draw.begin() + i);
		i--;
		
		std::vector<glm::ivec2> left_out_points, left_in_points;
		std::vector<glm::ivec2> right_out_points, right_in_points;
		for(int j = 0; j < 3; j++) {
			if (out_points[j] == LEFT_OUT) left_out_points.push_back(points[j]);
			else left_in_points.push_back(points[j]);
		}
		for(int j = 0; j < 3; j++) {
			if (out_points[j] == RIGHT_OUT) right_out_points.push_back(points[j]);
			else right_in_points.push_back(points[j]);
		}

		if (basis == 0) {
			if (triangulate(left_out_points, left_in_points, draw, base_trg, 200, basis)) continue;
			if (triangulate(right_out_points, right_in_points, draw, base_trg, fbo_width - 200, basis)) continue;
		}
		if (basis == 1) {
			if (triangulate(left_out_points, left_in_points, draw, base_trg, 200, basis)) continue;
			if (triangulate(right_out_points, right_in_points, draw, base_trg, fbo_height - 200, basis)) continue;
		}
	}
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

FORCE_INLINE void rasterize(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 norm, float near, float far) {
	std::vector<triangle> draw;
	p1 = vertex(p1); p2 = vertex(p2); p3 = vertex(p3);
	triangle ftrg = (triangle){p1, p2, p3, p1.z, p2.z, p3.z, norm};
	draw.push_back(ftrg);

	// backface culling
	if (glm::cross(glm::vec3(ftrg.p2, 0.0f) - glm::vec3(ftrg.p1, 0.0f), glm::vec3(ftrg.p3, 0.0f) - glm::vec3(ftrg.p1, 0.0f)).z <= 0.0f) return;
	
	//easy culling
	glm::ivec2 points[3] = { ftrg.p1, ftrg.p2, ftrg.p3 };
	float points_z[3] = { ftrg.p1d, ftrg.p2d, ftrg.p3d };
	
	int out_points[3];
	bool do_hard_x = true, do_hard_y = true, do_hard_z = true;
	for (int j = 0; j < 3; j++) out_points[j] = is_out(points[j].x, 200, fbo_width - 200);
	if (out_points[0] == INSIDE && out_points[1] == INSIDE && out_points[2] == INSIDE) do_hard_x = false;
	if (out_points[0] == LEFT_OUT && out_points[1] == LEFT_OUT && out_points[2] == LEFT_OUT) return;
	if (out_points[0] == RIGHT_OUT && out_points[1] == RIGHT_OUT && out_points[2] == RIGHT_OUT) return;
	
	for (int j = 0; j < 3; j++) out_points[j] = is_out(points[j].y, 200, fbo_height - 200);
	if (out_points[0] == INSIDE && out_points[1] == INSIDE && out_points[2] == INSIDE) do_hard_y = false;
	if (out_points[0] == LEFT_OUT && out_points[1] == LEFT_OUT && out_points[2] == LEFT_OUT) return;
	if (out_points[0] == RIGHT_OUT && out_points[1] == RIGHT_OUT && out_points[2] == RIGHT_OUT) return;
	
	for (int j = 0; j < 3; j++) out_points[j] = f_is_out(points_z[j], near, far);
	if (out_points[0] == INSIDE && out_points[1] == INSIDE && out_points[2] == INSIDE) do_hard_z = false;
	if (out_points[0] == LEFT_OUT && out_points[1] == LEFT_OUT && out_points[2] == LEFT_OUT) return;
	if (out_points[0] == RIGHT_OUT && out_points[1] == RIGHT_OUT && out_points[2] == RIGHT_OUT) return;
	
	// hard culling
	if (do_hard_z) {
		full_culling(draw, 2);
	}
	if (do_hard_y) {
		full_culling(draw, 1);
	}
	if (do_hard_x) {
		full_culling(draw, 0);
	}

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
