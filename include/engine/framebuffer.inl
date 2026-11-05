void init_buffers(int16_t x, int16_t y) {
	fbo = (int32_t*)malloc(sizeof(int32_t) * x * y);
	dbo = (float*)malloc(sizeof(float) * x * y);
	
	fbo_width = x;
	fbo_height = y;
}
void free_buffers() {
	free(fbo);
	free(dbo);
	fbo = nullptr;
	dbo = nullptr;
}

FORCE_INLINE void clear_buffers() {
	std::memset(fbo, 0, fbo_width * fbo_height * sizeof(int32_t));
	std::memset(dbo, 0, fbo_width * fbo_height * sizeof(float));
}

FORCE_INLINE void set_pixel(int16_t x, int16_t y, int32_t color) {
	fbo[y * fbo_width + x] = color;
}
FORCE_INLINE int32_t get_pixel(int16_t x, int16_t y) {
	return fbo[y * fbo_width + x];
}
FORCE_INLINE void set_depth_pixel(int16_t x, int16_t y, float color) {
	dbo[y * fbo_width + x] = color;
}
FORCE_INLINE float get_depth_pixel(int16_t x, int16_t y) {
	return dbo[y * fbo_width + x];
}
