#include "camera.hpp"

camera::camera() : camera({100, 100}, 1.0) {}
camera::camera(vec2i p_image_dimension, double p_focal_length) : m_image_dimension(p_image_dimension), m_focal_length(p_focal_length), m_interval(interval::universe) {
	m_aspect_ratio = static_cast<double>(p_image_dimension[0]) / p_image_dimension[1];
	m_viewport_dimension = {2.0 * m_aspect_ratio, 2.0}; //hard coded so the height is [-1, 1] when the camera is centered at (0, 0, 0)
	/* defines left/right & up/down dir of camera */
	m_pixel_delta_u = viewportU() / static_cast<double>(m_image_dimension[0]); //ratio between our virtual viewport sizes and the image sizes | how much viewport distance per pixel width
	m_pixel_delta_v = viewportV() / static_cast<double>(m_image_dimension[1]); //now we know our viewport distance scaled to pixels. so we move half that distance across and down to have our points in the middle of what will become pixels

	m_viewport_origin = m_position - vec3{0.0, 0.0, m_focal_length} - (viewportU() + viewportV()) * 0.5; //so by going negative we are going left and up and back. so the camera wil be looking backward toward -Z
	m_pixel_origin = m_viewport_origin + (m_pixel_delta_u + m_pixel_delta_v) * 0.5;

	m_world = world(32); //default is a world of 32 objects
}


vec3 camera::viewportU() const {return {m_viewport_dimension[0], 0.0, 0.0};}
vec3 camera::viewportV() const {return {0.0, -m_viewport_dimension[1], 0.0};}

void camera::render() {
	PPM image(m_image_dimension, "camera_render.ppm"); //should create image with data on heap
	
	for(int row = 0; row < m_image_dimension[1]; row++) {
		for(int col = 0; col < m_image_dimension[0]; col++) {
			vec3 pixel_center = m_pixel_origin + (m_pixel_delta_u * static_cast<double>(col)) + (m_pixel_delta_v * static_cast<double>(row));
			vec3 ray_direction = pixel_center - m_position; //getting the ray from the pixel to the camera here (may be a bit unintuitive at first)
			ray _ray(m_position, ray_direction, m_interval); //pass along the default ray interval settings
			color pixel_color = _ray.pixelColor(m_world);
			image.draw({col, row}, pixel_color);
		}
	}
	image.writeImage();

	
}