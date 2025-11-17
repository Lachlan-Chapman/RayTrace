#include "camera.hpp"
#include "constants.hpp"

camera::camera() : camera({100, 100}, 1.0, 1, 1) {}
camera::camera(vec2i p_image_dimension, double p_focal_length, int p_sample_count, int p_max_bounce) : m_image_dimension(p_image_dimension), m_focal_length(p_focal_length), m_interval(interval::forward), m_sample_count(p_sample_count), m_max_bounce(p_max_bounce), m_vertical_fov(constants::PI / 2) {
	m_aspect_ratio = static_cast<double>(p_image_dimension[0]) / p_image_dimension[1];
	m_viewport_dimension = {2.0 * m_aspect_ratio, 2.0}; //hard coded so the height is [-1, 1] when the camera is centered at (0, 0, 0)

	double h = std::tan(m_vertical_fov / 2);
	m_viewport_dimension[1] = 2 * h * m_focal_length;
	m_viewport_dimension[0] = m_viewport_dimension[1] * (static_cast<double>(m_image_dimension[0]) / m_image_dimension[1]);
	

	/* defines left/right & up/down dir of camera */
	m_pixel_delta_u = viewportU() / static_cast<double>(m_image_dimension[0]); //ratio between our virtual viewport sizes and the image sizes | how much viewport distance per pixel width
	m_pixel_delta_v = viewportV() / static_cast<double>(m_image_dimension[1]); //now we know our viewport distance scaled to pixels. so we move half that distance across and down to have our points in the middle of what will become pixels

	m_viewport_origin = m_position - vec3{0.0, 0.0, m_focal_length} - 0.5 * viewportU() - 0.5 * viewportV();
 //so by going negative we are going left and up and back. so the camera wil be looking backward toward -Z
	m_pixel_origin = m_viewport_origin + (m_pixel_delta_u + m_pixel_delta_v) * 0.5;

	m_world = world(32); //default is a world of 32 objects
	m_sample_scale = 1.0 / m_sample_count;
}


vec3 camera::viewportU() const {return {m_viewport_dimension[0], 0.0, 0.0};}
vec3 camera::viewportV() const {return {0.0, -m_viewport_dimension[1], 0.0};}

vec3 camera::toGamma(vec3 p_color, double p_gamma = 2.2) const {
	vec3 gamma_converted;
	for(int i = 0; i < 3; i++) {
		gamma_converted[i] = std::pow(p_color[i], 1 / p_gamma);
	}
	return gamma_converted;
}
vec3 camera::toSRGB(vec3 p_color) const {
	vec3 srgb_converted;
	for(int i = 0; i < 3; i++) {
		if(p_color[i] <= 0.0031308) {
			srgb_converted[i] = 12.92 * p_color[i];
		} else {
			srgb_converted[i] = 1.055 * std::pow(p_color[i], 1.0/2.4) - 0.055;
		}
	}
	return srgb_converted;
}

void camera::render() {
	PPM image(m_image_dimension, "image.ppm"); //should create image with data on heap
	
	for(int row = 0; row < m_image_dimension[1]; row++) {
		for(int col = 0; col < m_image_dimension[0]; col++) {

			vec3 pixel_center = m_pixel_origin + (m_pixel_delta_u * static_cast<double>(col)) + (m_pixel_delta_v * static_cast<double>(row));
			
			color pixel_color(0.0);
			double random_scalar = 1.0;
			for(int ray_id = 0; ray_id < m_sample_count; ray_id++) { //now calculate ray for each sample moving its direction slightly
				//double random_scalar = ; //max pixel size offset possible
				//lets randomly adjust the location where the ray crosses the viewport based on the pixel size
				vec3 offset = (m_pixel_delta_u * rng::decimal(-random_scalar, random_scalar)) + (m_pixel_delta_v * rng::decimal(-random_scalar, random_scalar));
				
				//making sure the 0th ray is true
				vec3 ray_direction = pixel_center;
				if(ray_id) {ray_direction += offset;} //add random offset on any "auxilery samples"
				ray_direction -= m_position; //getting the ray from the pixel to the camera here (may be a bit unintuitive at first)

				ray _ray(m_position, ray_direction, m_interval); //pass along the default ray interval settings
				pixel_color += _ray.traceColor(m_world, m_max_bounce); //get me the combined color of the full path of the ray
			}
			pixel_color *= m_sample_scale;
			//pixel_color = toGamma(pixel_color); //convert to gamma space using default gamma 2.2
			//pixel_color = toSRGB(pixel_color); //convert to srgb gamma space
			image.draw({col, row}, pixel_color);
			//std::cout << pixel_color << std::endl;
		}
	}
	image.writeImage();

	
}

vec3 camera::getRay(vec2i p_pixel) {
	vec3 pixel_center = m_pixel_origin + (m_pixel_delta_u * static_cast<double>(p_pixel[0])) + (m_pixel_delta_v * static_cast<double>(p_pixel[1]));
	vec3 ray_direction = pixel_center - m_position; //getting the ray from the pixel to the camera here (may be a bit unintuitive at first)

	vec3 random_offset = {
		rng::decimal(0, 0.5),
		rng::decimal(0, 0.5),
		0 //keep the depth the same.
	}; //used to adjust the direction to sample different points

	return ray_direction + random_offset;
}