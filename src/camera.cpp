#include "camera.hpp"
#include "constant.hpp"

camera::camera() : camera(32) {}
camera::camera(int p_object_count) : m_world(world(p_object_count)) {
	setCameraMeta(vec2i{10, 10}, 90.0, constant::PI / 18.0, 3.4);
	setPosition(vec3(0.0), vec3{0.0, 0.0, -1.0});
	setRayTraceMeta(1, 1);
}

void camera::setCameraMeta(const vec2i &p_screen_res, double p_fov, double p_defocus_angle, double p_focus_distance = 10.0) {
	m_image_dimension = p_screen_res;
	m_aspect_ratio = static_cast<double>(m_image_dimension[0]) / m_image_dimension[1];
	m_fov = p_fov;
	m_defocus_angle = p_defocus_angle;
	m_focus_distance = p_focus_distance;
}

void camera::setPosition(const vec3 &p_position, const vec3 &p_target) {
	m_position = p_position;
	m_target = p_target;
	
	m_focal_length = (m_position - m_target).magnitude();
	double h = std::tan(m_fov / 2);
	m_viewport_dimension[1] = 2 * h * m_focus_distance;
	m_viewport_dimension[0] = m_viewport_dimension[1] * (static_cast<double>(m_image_dimension[0]) / m_image_dimension[1]);

	m_w = (m_position - m_target).unit();
	m_u = vec3{0.0, 1.0, 0.0}.cross(m_w);
	m_v = m_w.cross(m_u);

	m_viewport_u = m_viewport_dimension[0] * m_u;
	m_viewport_v = m_viewport_dimension[1] * -m_v;

	m_pixel_delta_u = m_viewport_u / static_cast<double>(m_image_dimension[0]);
	m_pixel_delta_v = m_viewport_v / static_cast<double>(m_image_dimension[1]);

	m_viewport_origin = p_position - (m_focus_distance * m_w) - (0.5 * m_viewport_u) - (0.5 * m_viewport_v);
	m_pixel_origin = m_viewport_origin + 0.5 * (m_pixel_delta_u + m_pixel_delta_v);

	double defocus_radius = m_focus_distance * std::tan(0.5 * m_defocus_angle);
	m_defocus_u = m_u * defocus_radius;
	m_defocus_v = m_v * defocus_radius;
}

void camera::setRayTraceMeta(int p_sample_count, int p_bounce_limit) {
	m_sample_count = p_sample_count;
	m_sample_scale = 1.0 / p_sample_count;
	m_bounce_limit = p_bounce_limit;
}

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

vec3 camera::randomDefocus() const {
	vec3 point = rng::insideUnitCircle();
	return m_position + (point[0] * m_defocus_u) + (point[1] * m_defocus_v);
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
				
				vec3 origin = m_defocus_angle <= 0.0 ? m_position : randomDefocus();
				
				ray_direction -= origin; //getting the ray from the pixel to the camera here (may be a bit unintuitive at first)
				ray _ray(origin, ray_direction, m_interval); //pass along the default ray interval settings
				pixel_color += _ray.traceColor(m_world, m_bounce_limit); //get me the combined color of the full path of the ray
			}
			pixel_color *= m_sample_scale;
			//pixel_color = toGamma(pixel_color); //convert to gamma space using default gamma 2.2
			//pixel_color = toSRGB(pixel_color); //convert to srgb gamma space
			image.draw({col, row}, pixel_color);
			//std::cout << pixel_color << std::endl;
		}
	}
	image.writeImage();
	std::clog << "Rendered Using " << m_sample_count << " Samples @ " << m_bounce_limit << " Bounces" << std::endl;	
}