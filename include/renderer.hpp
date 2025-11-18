#pragma once
#include <atomic>

#include "PPM.hpp"
#include "world.hpp"
#include "camera.hpp"
#include "tile.hpp"
class renderer {
public:
	renderer();
	renderer(PPM * const p_ppm, const world * const p_world, const cameraConfig &p_config);
	
	void renderImage(int p_sampleCount, int p_bounceLimit, const vec2i &p_tileDimension);
	void renderImageMT(int p_sampleCount, int p_bounceLimit, const vec2i &p_tileDimension, int p_threadsSaved = 2);

private:
	void threadFunction(std::atomic<int> &p_atomicId, int p_sampleCount, int p_bounceLimit);
	void renderTile(const tile &p_tile, int p_sampleCount, int p_bounceLimit);

	const world * const m_world;
	camera m_camera;
	PPM * const m_image;
	tileGenerator m_tileGenerator;
};