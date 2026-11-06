#pragma once

#include <functional>

#include <glm/glm.hpp>

#include <engine/framebuffer.hpp>
#include <engine/engine.hpp>

#define MIN 0
#define MAX 1

namespace Engine {

	struct triangle {
		glm::ivec2 p1, p2, p3;
		float p1d, p2d, p3d;

		glm::vec3 normal;
	};
	
	inline std::function<void(const triangle& trg, const std::vector<std::array<int16_t, 2>>& area, int16_t min_y)> fragment;
	inline std::function<glm::vec3(const glm::vec3& point)> vertex;
	
	FORCE_INLINE uint32_t vec4_to_color(glm::vec4 vec);
	FORCE_INLINE glm::vec4 color_to_vec4(uint32_t color);

	// bary
	FORCE_INLINE glm::vec3 bary_weights(const triangle& trg, const glm::ivec2& point);
	FORCE_INLINE glm::vec3 bary_weights(const glm::ivec2& p1, const glm::ivec2& p2, const glm::ivec2& p3, const glm::ivec2& point);
	FORCE_INLINE float bary_interpolate_float(const glm::vec3& weights, float a, float b, float c);
	FORCE_INLINE glm::vec4 bary_interpolate_color(const glm::vec3& weights, const glm::vec4& a, const glm::vec4& b, const glm::vec4& c);
	FORCE_INLINE glm::ivec2 bary_get_coord_inside(const glm::vec3& weights, const glm::ivec2& p1, const glm::ivec2& p2, const glm::ivec2& p3);
	FORCE_INLINE glm::ivec2 bary_get_coord_inside(const glm::vec3& weights, const triangle& trg);

	// rasterization
	FORCE_INLINE void calc_line(glm::ivec2 p1, glm::ivec2 p2, int16_t min_y, std::vector<std::array<int16_t, 2>>& area);
	FORCE_INLINE void full_culling(const std::vector<triangle>&);
	FORCE_INLINE void rasterize(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 norm);

	#include "rasterizer.inl"

};
