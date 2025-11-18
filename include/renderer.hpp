#pragma once
#include "PPM.hpp"
#include "world.hpp"
#include "camera.hpp"
class renderer {
public:
	renderer();
	renderer(PPM * const p_ppm, const world * const p_world, const cameraConfig &p_config);
	void renderImage(int p_sampleCount, int p_bounceLimit) const;

private:
	const world * const m_world;
	camera m_camera;
	PPM * const m_image;
};