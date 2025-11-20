#include "camera.hpp"
#include "constant.hpp"
#include "random.hpp"

camera::camera() : camera(nullptr, vec3d(0.0), 0.0, constant::PI * 0.5) {}
camera::camera(const world* const p_world, const vec3d &p_position, double p_defocusAngle, double p_fov) : m_world(p_world), m_position(p_position), m_defocusAngle(p_defocusAngle), m_fov(p_fov) {
	init(vec2i(1), vec3d(0.0), vec3d(0.0), 0.0);
}

void camera::init(const vec2i &p_imageResolution, const vec3d &p_target, const vec3d &p_upVector, double p_focusDistance) {	
	double h = std::tan(m_fov / 2);
	vec2i viewport_dimension;
	viewport_dimension[1] = 2 * h * p_focusDistance;
	viewport_dimension[0] = viewport_dimension[1] * (static_cast<double>(p_imageResolution[0]) / p_imageResolution[1]);

	vec3d w = (m_position - p_target).unit();
	vec3d u = cross(p_upVector, w).unit();
	vec3d v = cross(w, u);

	vec3d viewportU = static_cast<double>(viewport_dimension[0]) * u;
	vec3d viewportV = static_cast<double>(viewport_dimension[1]) * -v;

	m_pixelDeltaU = viewportU / static_cast<double>(p_imageResolution[0]);
	m_pixelDeltaV = viewportV / static_cast<double>(p_imageResolution[1]);

	vec3d viewport_origin = m_position - (p_focusDistance * w) - (0.5 * viewportU) - (0.5 * viewportV);
	m_pixelOrigin = viewport_origin + 0.5 * (m_pixelDeltaU + m_pixelDeltaV);

	double defocus_radius = p_focusDistance * std::tan(0.5 * m_defocusAngle);
	m_defocusU = u * defocus_radius;
	m_defocusV = v * defocus_radius;

	m_fPosition = m_position;
	m_fPixelDeltaU = m_pixelDeltaU;
	m_fPixelDeltaV = m_pixelDeltaV;
	m_fPixelOrigin = m_pixelOrigin;
	m_fDefocusU = m_defocusU;
	m_fDefocusV = m_defocusV;
}



vec3f camera::randomDefocus() const {
	vec3f point = rng::insideUnitCircle();
	return m_fPosition + (point[0] * m_fDefocusU) + (point[1] * m_fDefocusV);
}

color camera::renderPixel(const vec2i &p_pixelCoordinate, int p_sampleCount, int p_bounceLimit) const {
	vec3f pixel_center = m_fPixelOrigin + (m_fPixelDeltaU * static_cast<float>(p_pixelCoordinate[0])) + (m_fPixelDeltaV * static_cast<float>(p_pixelCoordinate[1]));
	
	color pixel_color(0.0);
	float random_scalar = 1.0;
	for(int ray_id = 0; ray_id < p_sampleCount; ray_id++) { //now calculate ray for each sample moving its direction slightly
		//lets randomly adjust the location where the ray crosses the viewport based on the pixel size
		vec3f offset = (m_fPixelDeltaU * rng::decimal(-random_scalar, random_scalar)) + (m_fPixelDeltaV * rng::decimal(-random_scalar, random_scalar));
		
		//making sure the 0th ray is true
		vec3f ray_direction = pixel_center;
		if(ray_id) {ray_direction += offset;} //add random offset on any "auxilery samples"
		
		vec3f origin = m_defocusAngle <= 0.0 ? m_fPosition : randomDefocus();
		
		ray_direction -= origin; //getting the ray from the pixel to the camera here (may be a bit unintuitive at first)
		ray _ray(origin, ray_direction.unit(), interval::forward); //pass along the default ray interval settings
		pixel_color += _ray.traceColor(*m_world, p_bounceLimit); //get me the combined color of the full path of the ray
	}
	pixel_color /= p_sampleCount;
	return pixel_color;
}