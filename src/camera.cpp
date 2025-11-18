#include "camera.hpp"
#include "constant.hpp"
#include "random.hpp"

camera::camera() : camera(nullptr, vec3(0.0), 0.0, constant::PI * 0.5) {}
camera::camera(const world* const p_world, const vec3 &p_position, double p_defocusAngle, double p_fov) : m_world(p_world), m_position(p_position), m_defocusAngle(p_defocusAngle), m_fov(p_fov) {
	init(vec2i(1), vec3(0.0), vec3(0.0), 0.0);
}

void camera::init(const vec2i &p_imageResolution, const vec3 &p_target, const vec3 &p_upVector, double p_focusDistance) {	
	double h = std::tan(m_fov / 2);
	vec2i viewport_dimension;
	viewport_dimension[1] = 2 * h * p_focusDistance;
	viewport_dimension[0] = viewport_dimension[1] * (static_cast<double>(p_imageResolution[0]) / p_imageResolution[1]);

	vec3 w = (m_position - p_target).unit();
	vec3 u = p_upVector.cross(w).unit();
	vec3 v = w.cross(u);

	vec3 viewportU = static_cast<double>(viewport_dimension[0]) * u;
	vec3 viewportV = static_cast<double>(viewport_dimension[1]) * -v;

	m_pixelDeltaU = viewportU / static_cast<double>(p_imageResolution[0]);
	m_pixelDeltaV = viewportV / static_cast<double>(p_imageResolution[1]);

	vec3 viewport_origin = m_position - (p_focusDistance * w) - (0.5 * viewportU) - (0.5 * viewportV);
	m_pixelOrigin = viewport_origin + 0.5 * (m_pixelDeltaU + m_pixelDeltaV);

	double defocus_radius = p_focusDistance * std::tan(0.5 * m_defocusAngle);
	m_defocusU = u * defocus_radius;
	m_defocusV = v * defocus_radius;
}



vec3 camera::randomDefocus() const {
	vec3 point = rng::insideUnitCircle();
	return m_position + (point[0] * m_defocusU) + (point[1] * m_defocusV);
}

color camera::renderPixel(const vec2i &p_pixelCoordinate, int p_sampleCount, int p_bounceLimit) const {
	vec3 pixel_center = m_pixelOrigin + (m_pixelDeltaU * static_cast<double>(p_pixelCoordinate[0])) + (m_pixelDeltaV * static_cast<double>(p_pixelCoordinate[1]));
	
	color pixel_color(0.0);
	double random_scalar = 1.0;
	for(int ray_id = 0; ray_id < p_sampleCount; ray_id++) { //now calculate ray for each sample moving its direction slightly
		//lets randomly adjust the location where the ray crosses the viewport based on the pixel size
		vec3 offset = (m_pixelDeltaU * rng::decimal(-random_scalar, random_scalar)) + (m_pixelDeltaV * rng::decimal(-random_scalar, random_scalar));
		
		//making sure the 0th ray is true
		vec3 ray_direction = pixel_center;
		if(ray_id) {ray_direction += offset;} //add random offset on any "auxilery samples"
		
		vec3 origin = m_defocusAngle <= 0.0 ? m_position : randomDefocus();
		
		ray_direction -= origin; //getting the ray from the pixel to the camera here (may be a bit unintuitive at first)
		ray _ray(origin, ray_direction.unit(), interval::forward); //pass along the default ray interval settings
		pixel_color += _ray.traceColor(*m_world, p_bounceLimit); //get me the combined color of the full path of the ray
	}
	pixel_color /= p_sampleCount;
	//pixel_color = toGamma(pixel_color); //convert to gamma space using default gamma 2.2
	//pixel_color = toSRGB(pixel_color); //convert to srgb gamma space
	return pixel_color;
}