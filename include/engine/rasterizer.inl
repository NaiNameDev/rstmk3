FORCE_INLINE uint32_t vec4_to_color(glm::vec4 vec) {
	return ((uint32_t)(unsigned char)glm::clamp(vec.w*255, 0.0f, 255.0f) << 24) |
           ((uint32_t)(unsigned char)glm::clamp(vec.z*255, 0.0f, 255.0f) << 16) |
           ((uint32_t)(unsigned char)glm::clamp(vec.y*255, 0.0f, 255.0f) <<  8) |
           ((uint32_t)(unsigned char)glm::clamp(vec.x*255, 0.0f, 255.0f));
}
FORCE_INLINE glm::vec4 color_to_vec4(uint32_t color) {
	return glm::vec4((color & 0xFF) / 255.0f, ((color >> 8) & 0xFF) / 255.0f, ((color >> 16) & 0xFF) / 255.0f, ((color >> 24) & 0xFF) / 255.0f);
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

FORCE_INLINE int is_out(float point, float clamp_min, float clamp_max) {
	if (point < clamp_min) return LEFT_OUT;
	if (point > clamp_max) return RIGHT_OUT;
	return INSIDE;
}
FORCE_INLINE glm::ivec2 colliding_point_x(glm::ivec2 p1, glm::ivec2 p2, float clamp) {
	return glm::ivec2(clamp, p1.y + ((clamp - p1.x) * (p2.y - p1.y) / (p2.x - p1.x)));	
}
FORCE_INLINE glm::ivec2 colliding_point_y(glm::ivec2 p1, glm::ivec2 p2, float clamp) {
	return glm::ivec2(p1.x + ((clamp - p1.y) * (p2.x - p1.x) / (p2.y - p1.y)), clamp);	
}
FORCE_INLINE glm::ivec2 colliding_point_z(glm::ivec2 p1, glm::ivec2 p2, float z1, float z2, float clamp) {
	return glm::ivec2(p1.x + ((clamp - z1) * (p2.x - p1.x) / (z2 - z1)),
					  p1.y + ((clamp - z1) * (p2.y - p1.y) / (z2 - z1)));	
}
FORCE_INLINE void interpolate_with_base(triangle& trg, const triangle& base) {
	glm::vec3 weights_p1 = bary_weights(base.vertices[0].pos, base.vertices[1].pos, base.vertices[2].pos, trg.vertices[0].pos);
	glm::vec3 weights_p2 = bary_weights(base.vertices[0].pos, base.vertices[1].pos, base.vertices[2].pos, trg.vertices[1].pos);
	glm::vec3 weights_p3 = bary_weights(base.vertices[0].pos, base.vertices[1].pos, base.vertices[2].pos, trg.vertices[2].pos);

	trg.vertices[0].depth = bary_interpolate_float(weights_p1, base.vertices[0].depth, base.vertices[1].depth, base.vertices[2].depth);
	trg.vertices[1].depth = bary_interpolate_float(weights_p2, base.vertices[0].depth, base.vertices[1].depth, base.vertices[2].depth);
	trg.vertices[2].depth = bary_interpolate_float(weights_p3, base.vertices[0].depth, base.vertices[1].depth, base.vertices[2].depth);
}
FORCE_INLINE bool triangulate(
	const std::vector<vertice>& out,
	const std::vector<vertice>& in,
	std::vector<triangle>& draw,
	triangle& base_trg,
	float clamp,
	int8_t basis
) {
	if (out.size() == 1) {
		triangle new_trg1 = base_trg;
		triangle new_trg2 = base_trg;
		
		if (basis == 0) {
			new_trg1.vertices[0].pos = colliding_point_x(out[0].pos, in[0].pos, clamp);
			new_trg1.vertices[1].pos = colliding_point_x(out[0].pos, in[1].pos, clamp);
		}
		else if(basis == 1) {
			new_trg1.vertices[0].pos = colliding_point_y(out[0].pos, in[0].pos, clamp);
			new_trg1.vertices[1].pos = colliding_point_y(out[0].pos, in[1].pos, clamp);
		}
		else if (basis == 2) {
			new_trg1.vertices[0].pos = colliding_point_z(out[0].pos, in[0].pos, out[0].depth, in[0].depth, clamp);
			new_trg1.vertices[1].pos = colliding_point_z(out[0].pos, in[1].pos, out[0].depth, in[1].depth, clamp);
		}
		new_trg1.vertices[2] = in[0];

		new_trg2.vertices[0] = new_trg1.vertices[1];
		new_trg2.vertices[1] = in[0];
		new_trg2.vertices[2] = in[1];
		
		interpolate_with_base(new_trg1, base_trg);
		interpolate_with_base(new_trg2, base_trg);

		draw.push_back(new_trg1);
		draw.push_back(new_trg2);
		
		return true;
	}
	if (out.size() == 2) {
		triangle new_trg = base_trg;
		
		if (basis == 0) {
			new_trg.vertices[0].pos = colliding_point_x(in[0].pos, out[0].pos, clamp);
			new_trg.vertices[1].pos = colliding_point_x(in[0].pos, out[1].pos, clamp);
		}
		else if (basis == 1) {
			new_trg.vertices[0].pos = colliding_point_y(in[0].pos, out[0].pos, clamp);
			new_trg.vertices[1].pos = colliding_point_y(in[0].pos, out[1].pos, clamp);
		}
		else if (basis == 2) {
			new_trg.vertices[0].pos = colliding_point_z(in[0].pos, out[0].pos, in[0].depth, out[0].depth, clamp);
			new_trg.vertices[1].pos = colliding_point_z(in[0].pos, out[1].pos, in[0].depth, out[1].depth, clamp);
		}
		new_trg.vertices[2] = in[0];
		
		interpolate_with_base(new_trg, base_trg);

		draw.push_back(new_trg);
		
		return true;
	}

	return false;
}

FORCE_INLINE void full_culling(std::vector<triangle>& draw, int basis) {
	for (int i = 0; i < draw.size(); i++) {
		triangle base_trg = draw[i];
		
		bool should_break = true;
		std::vector<vertice> left_out_points, right_out_points;
		std::vector<vertice> left_in_points, right_in_points;
		for (int j = 0; j < 3; j++) {
			int status;
			if (basis == 0) status = is_out(base_trg.vertices[j].pos.x, 0, fbo_width);
			if (basis == 1) status = is_out(base_trg.vertices[j].pos.y, 0, fbo_height);
			
			if (status == INSIDE) {
				left_in_points.push_back(base_trg.vertices[j]);
				right_in_points.push_back(base_trg.vertices[j]);
				continue;
			}
			else if (status == LEFT_OUT) {
				left_out_points.push_back(base_trg.vertices[j]);
				right_in_points.push_back(base_trg.vertices[j]);
			}
			else if (status == RIGHT_OUT) {
				left_in_points.push_back(base_trg.vertices[j]);
				right_out_points.push_back(base_trg.vertices[j]);
			}
			should_break = false;
		}
		if (should_break) continue; // all inside
		
		draw.erase(draw.begin() + i);
		i--;

		if (basis == 0) {
			if (triangulate(left_out_points, left_in_points, draw, base_trg, 0, basis)) continue;
			if (triangulate(right_out_points, right_in_points, draw, base_trg, fbo_width, basis)) continue;
		}
		if (basis == 1) {
			if (triangulate(left_out_points, left_in_points, draw, base_trg, 0, basis)) continue;
			if (triangulate(right_out_points, right_in_points, draw, base_trg, fbo_height, basis)) continue;
		}
		if (basis == 2) {
			if (triangulate(left_out_points, left_in_points, draw, base_trg, near, basis)) continue;
			if (triangulate(right_out_points, right_in_points, draw, base_trg, far, basis)) continue;
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
				glm::vec3 weights = Engine::bary_weights(trg.vertices[0].pos, trg.vertices[1].pos, trg.vertices[2].pos, glm::vec2(j, i + min_y));\
\
				float depth_old = Engine::get_depth_pixel(j, i + min_y);\
				float depth = Engine::bary_interpolate_float(weights, trg.vertices[0].depth, trg.vertices[1].depth, trg.vertices[2].depth);\
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

FORCE_INLINE void rasterize(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 norm) {
	std::vector<triangle> draw;
	p1 = vertex(p1); p2 = vertex(p2); p3 = vertex(p3);
	triangle ftrg = (triangle){(vertice){p1, p1.z}, (vertice){p2, p2.z}, (vertice){p3, p3.z}, norm};
	draw.push_back(ftrg);

	// backface culling
	if (glm::cross(glm::vec3(ftrg.vertices[1].pos, 0.0f) - glm::vec3(ftrg.vertices[0].pos, 0.0f), glm::vec3(ftrg.vertices[2].pos, 0.0f) - glm::vec3(ftrg.vertices[0].pos, 0.0f)).z <= 0.0f) return;
	
	//easy culling
	glm::ivec2 points[3] = { ftrg.vertices[0].pos, ftrg.vertices[1].pos, ftrg.vertices[2].pos };
	float points_z[3] = { ftrg.vertices[0].depth, ftrg.vertices[1].depth, ftrg.vertices[2].depth };
	
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
	
	for (int j = 0; j < 3; j++) out_points[j] = is_out(points_z[j], near, far);
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
		int16_t min = std::min(trg.vertices[0].pos.y, std::min(trg.vertices[1].pos.y, trg.vertices[2].pos.y));
		int16_t max = std::max(trg.vertices[0].pos.y, std::max(trg.vertices[1].pos.y, trg.vertices[2].pos.y));
		
		std::vector<std::array<int16_t, 2>> triangle_area(max - min + 1, std::array<int16_t, 2>{32767, -32767});
	
		calc_line(trg.vertices[0].pos, trg.vertices[1].pos, min, triangle_area);
		calc_line(trg.vertices[1].pos, trg.vertices[2].pos, min, triangle_area);
		calc_line(trg.vertices[2].pos, trg.vertices[0].pos, min, triangle_area);
		
		fragment(trg, triangle_area, min);
	}
}
