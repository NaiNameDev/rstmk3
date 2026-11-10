FORCE_INLINE void init_buffers(int16_t x, int16_t y, float new_near, float new_far) {
	fbo = (int32_t*)malloc(sizeof(int32_t) * x * y);
	dbo = (float*)malloc(sizeof(float) * x * y);
	
	fbo_width = x;
	fbo_height = y;

	near = new_near;
	far = new_far;
}
FORCE_INLINE void free_buffers() {
	free(fbo);
	free(dbo);
	fbo = nullptr;
	dbo = nullptr;
}

FORCE_INLINE void clear_buffers(const int32_t& clear) {
	//std::memset(fbo, 0, fbo_width * fbo_height * sizeof(int32_t));
	//std::memset(dbo, 0, fbo_width * fbo_height * sizeof(float));
	for (int i = 0; i < fbo_width * fbo_height; i++) {
		fbo[i] = clear;
		dbo[i] = far;
	}
}

FORCE_INLINE void set_pixel(int16_t x, int16_t y, int32_t color) {
	fbo[y * fbo_width + x] = color;
	//if (x < fbo_width && x >= 0 && y < fbo_height && y >= 0) fbo[y * fbo_width + x] = color;
}
FORCE_INLINE int32_t get_pixel(int16_t x, int16_t y) {
	return fbo[y * fbo_width + x];
	//if (x < fbo_width && x >= 0 && y < fbo_height && y >= 0) return fbo[y * fbo_width + x];
	//return 0;
}
FORCE_INLINE void set_depth_pixel(int16_t x, int16_t y, float color) {
	dbo[y * fbo_width + x] = color;
	//if (x < fbo_width && x >= 0 && y < fbo_height && y >= 0) dbo[y * fbo_width + x] = color;
}
FORCE_INLINE float get_depth_pixel(int16_t x, int16_t y) {
	return dbo[y * fbo_width + x];
	//if (x < fbo_width && x >= 0 && y < fbo_height && y >= 0) return dbo[y * fbo_width + x];
	//return 0;
}
