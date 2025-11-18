#include "camera.hpp"
#include "constant.hpp"
#include "random.hpp"

camera::camera() : camera(32) {}
camera::camera(int p_objectCount) : m_world(world(p_objectCount)) {
	setCameraMeta(vec2i{10, 10}, 90.0, constant::PI / 18.0, 3.4);
	setPosition(vec3(0.0), vec3{0.0, 0.0, -1.0});
	setRayTraceMeta(1, 1);
}

void camera::setCameraMeta(const vec2i &p_screenResolution, double p_fov, double p_defocusAngle, double p_focusDistance = 10.0) {
	m_imageDimension = p_screenResolution;
	m_aspectRatio = static_cast<double>(m_imageDimension[0]) / m_imageDimension[1];
	m_fov = p_fov;
	m_defocusAngle = p_defocusAngle;
	m_focusDistance = p_focusDistance;
}

void camera::setPosition(const vec3 &p_position, const vec3 &p_target) {
	m_position = p_position;
	m_target = p_target;
	
	m_focalLength = (m_position - m_target).magnitude();
	double h = std::tan(m_fov / 2);
	m_viewportDimension[1] = 2 * h * m_focusDistance;
	m_viewportDimension[0] = m_viewportDimension[1] * (static_cast<double>(m_imageDimension[0]) / m_imageDimension[1]);

	m_w = (m_position - m_target).unit();
	m_u = vec3{0.0, 1.0, 0.0}.cross(m_w);
	m_v = m_w.cross(m_u);

	m_viewportU = m_viewportDimension[0] * m_u;
	m_viewportV = m_viewportDimension[1] * -m_v;

	m_pixelDeltaU = m_viewportU / static_cast<double>(m_imageDimension[0]);
	m_pixelDeltaV = m_viewportV / static_cast<double>(m_imageDimension[1]);

	m_viewportOrigin = p_position - (m_focusDistance * m_w) - (0.5 * m_viewportU) - (0.5 * m_viewportV);
	m_pixelOrigin = m_viewportOrigin + 0.5 * (m_pixelDeltaU + m_pixelDeltaV);

	double defocus_radius = m_focusDistance * std::tan(0.5 * m_defocusAngle);
	m_defocusU = m_u * defocus_radius;
	m_defocusV = m_v * defocus_radius;
}

void camera::setRayTraceMeta(int p_sampleCount, int p_bounceLimit) {
	m_sampleCount = p_sampleCount;
	m_sampleScale = 1.0 / p_sampleCount;
	m_bounceLimit = p_bounceLimit;
}

vec3 camera::toGamma(const vec3 &p_color, double p_gamma) const {
	vec3 gamma_converted;
	for(int i = 0; i < 3; i++) {
		gamma_converted[i] = std::pow(p_color[i], 1 / p_gamma);
	}
	return gamma_converted;
}
vec3 camera::toSRGB(const vec3 &p_color) const {
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
	return m_position + (point[0] * m_defocusU) + (point[1] * m_defocusV);
}

void camera::render() const {
	PPM image(m_imageDimension, "image.ppm"); //should create image with data on heap
	
	for(int row = 0; row < m_imageDimension[1]; row++) {
		for(int col = 0; col < m_imageDimension[0]; col++) {

			vec3 pixel_center = m_pixelOrigin + (m_pixelDeltaU * static_cast<double>(col)) + (m_pixelDeltaV * static_cast<double>(row));
			
			color pixel_color(0.0);
			double random_scalar = 1.0;
			for(int ray_id = 0; ray_id < m_sampleCount; ray_id++) { //now calculate ray for each sample moving its direction slightly
				//double random_scalar = ; //max pixel size offset possible
				//lets randomly adjust the location where the ray crosses the viewport based on the pixel size
				vec3 offset = (m_pixelDeltaU * rng::decimal(-random_scalar, random_scalar)) + (m_pixelDeltaV * rng::decimal(-random_scalar, random_scalar));
				
				//making sure the 0th ray is true
				vec3 ray_direction = pixel_center;
				if(ray_id) {ray_direction += offset;} //add random offset on any "auxilery samples"
				
				vec3 origin = m_defocusAngle <= 0.0 ? m_position : randomDefocus();
				
				ray_direction -= origin; //getting the ray from the pixel to the camera here (may be a bit unintuitive at first)
				ray _ray(origin, ray_direction, m_interval); //pass along the default ray interval settings
				pixel_color += _ray.traceColor(m_world, m_bounceLimit); //get me the combined color of the full path of the ray
			}
			pixel_color *= m_sampleScale;
			//pixel_color = toGamma(pixel_color); //convert to gamma space using default gamma 2.2
			//pixel_color = toSRGB(pixel_color); //convert to srgb gamma space
			image.draw({col, row}, pixel_color);
			//std::cout << pixel_color << std::endl;
		}
	}
	image.writeImage();
	std::clog << "Rendered Using " << m_sampleCount << " Samples @ " << m_bounceLimit << " Bounces" << std::endl;	
}