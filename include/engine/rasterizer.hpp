#pragma once

#include <functional>

#include <glm/glm.hpp>

#include <engine/framebuffer.hpp>
#include <engine/engine.hpp>

#define MIN 0
#define MAX 1

#define LEFT_OUT -1
#define INSIDE 0
#define RIGHT_OUT 1

namespace Engine {

	struct vertice {
		glm::ivec2 pos;
		float depth;
	};
	
	struct triangle {
		vertice vertices[3];
		glm::vec3 normal;
	};

	inline std::function<void(const triangle& trg, const std::vector<std::array<int16_t, 2>>& area, int16_t min_y)> fragment;
	inline std::function<glm::vec3(const glm::vec3& point)> vertex;
	
	FORCE_INLINE uint32_t vec4_to_color(glm::vec4 vec);
	FORCE_INLINE glm::vec4 color_to_vec4(uint32_t color);

	// bary
	FORCE_INLINE glm::vec3 bary_weights(const glm::ivec2& p1, const glm::ivec2& p2, const glm::ivec2& p3, const glm::ivec2& point);
	FORCE_INLINE glm::ivec2 bary_get_coord_inside(const glm::vec3& weights, const glm::ivec2& p1, const glm::ivec2& p2, const glm::ivec2& p3);
	FORCE_INLINE float bary_interpolate_float(const glm::vec3& weights, float a, float b, float c);
	FORCE_INLINE glm::vec4 bary_interpolate_color(const glm::vec3& weights, const glm::vec4& a, const glm::vec4& b, const glm::vec4& c);

	// culling
	FORCE_INLINE int is_out(float point, float clamp_min, float clamp_max);
	FORCE_INLINE glm::ivec2 colliding_point_x(glm::ivec2 p1, glm::ivec2 p2, float clamp);
	FORCE_INLINE glm::ivec2 colliding_point_y(glm::ivec2 p1, glm::ivec2 p2, float clamp);
	FORCE_INLINE glm::ivec2 colliding_point_z(glm::ivec2 p1, glm::ivec2 p2, float z1, float z2, float clamp);
	FORCE_INLINE bool triangulate(
		const std::vector<glm::ivec2>& out,
		const std::vector<glm::ivec2>& in,
		std::vector<triangle>& draw,
		triangle& base_trg,
		int16_t limit,
		int16_t basis
	);
	FORCE_INLINE void full_culling(std::vector<triangle>&, int basis);

	// rasterization
	FORCE_INLINE void calc_line(glm::ivec2 p1, glm::ivec2 p2, int16_t min_y, std::vector<std::array<int16_t, 2>>& area);
	FORCE_INLINE void rasterize(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 norm);

	#include "rasterizer.inl"

};
