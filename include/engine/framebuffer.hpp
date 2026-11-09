#pragma once

#include <glm/glm.hpp>

#include <engine/engine.hpp>

namespace Engine {
	
	inline int32_t* fbo;
	inline int16_t fbo_width, fbo_height;
	inline float* dbo;
	inline float far, near;
	
	void init_buffers(int16_t x, int16_t y, float near, float far);
	void free_buffers();

	FORCE_INLINE void clear_buffers();

	FORCE_INLINE void set_pixel(int16_t x, int16_t y, int32_t color);
	FORCE_INLINE int32_t get_pixel(int16_t x, int16_t y);
	FORCE_INLINE void set_depth_pixel(int16_t x, int16_t y, float color);
	FORCE_INLINE float get_depth_pixel(int16_t x, int16_t y);

	#include "framebuffer.inl"

};
