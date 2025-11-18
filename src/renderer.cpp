#include "renderer.hpp"

renderer::renderer() : renderer(nullptr, nullptr, cameraConfig{}) {}
renderer::renderer(PPM * const p_ppm, const world * const p_world, const cameraConfig &p_config) : m_image(p_ppm), m_world(p_world), m_camera(
	p_world,
	p_config.d_position,
	p_config.d_defocusAngle,
	p_config.d_fov
) {
	m_camera.init(
		p_ppm->m_resolution,
		p_config.d_target,
		p_config.d_upVector,
		p_config.d_focusDistance
	);
}


void renderer::renderImage(int p_sampleCount, int p_bounceLimit) const {
	vec2i image_resolution = m_image->m_resolution;
	for(int col = 0; col < image_resolution[0]; col++) {
		for(int row = 0; row < image_resolution[1]; row++) {
			color pixel_color = m_camera.renderPixel({col, row}, p_sampleCount, p_bounceLimit);
			m_image->draw({col, row}, pixel_color);
		}
	}
	m_image->writeImage();
}