#include <atomic>
#include <thread>

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

void renderer::renderTile(const tile &p_tile, int p_sampleCount, int p_bounceLimit) {
	for(int row = 0; row < p_tile.d_dimension[1]; row++) {
		int _y = p_tile.d_start[1] + row;
		for(int col = 0; col < p_tile.d_dimension[0]; col++) {
			int _x = p_tile.d_start[0] + col;
			color pixel_color = m_camera.renderPixel(vec2i{_x, _y}, p_sampleCount, p_bounceLimit);
			m_image->draw(vec2i{_x, _y}, pixel_color);
		}
	}
}


void renderer::renderImage(int p_sampleCount, int p_bounceLimit, const vec2i &p_tileDimension) {
	tileGenerator _generator(m_image->m_resolution, p_tileDimension);
	int tile_count = _generator.totalTiles();
	for(int id = 0; id < tile_count; id++) {
		renderTile(_generator[id], p_sampleCount, p_bounceLimit);
	}
	m_image->writeImage();
}

void renderer::threadFunction(std::atomic<int> &p_atomicId, int p_sampleCount, int p_bounceLimit) {
	int tile_count = m_tileGenerator.totalTiles();
	for(;;) {
		int tile_id = p_atomicId.fetch_add(1, std::memory_order_relaxed); //order relaxed is focused on the atomicity of the operator | the cpu can order other loads and stores around it ensuring atomic operation
		if(tile_id >= tile_count) {break;} //no more tiles to render
		tile _tile = m_tileGenerator[tile_id]; //get tile for this id
		renderTile(_tile, p_sampleCount, p_bounceLimit);
	}

}

void renderer::renderImageMT(int p_sampleCount, int p_bounceLimit, const vec2i &p_tileDimension, int p_threadsSaved) {
	//pre thread setup
	m_tileGenerator = tileGenerator(m_image->m_resolution, p_tileDimension);
	std::atomic<int> tile_id{0};

	//thread setup
	int system_thread_count = static_cast<int>(std::thread::hardware_concurrency());
	std::clog << "System Thread Count " << system_thread_count << std::endl;
	int thread_count = std::max<int>(system_thread_count - p_threadsSaved, 1); //minimum 1 thread but ideally reserve p_threadsSaved (default = 2) for the os
	thread_count = std::min<int>(thread_count, system_thread_count); //dont exceed the available threads
	std::clog << "Using Thread Count " << thread_count << std::endl;
	
	std::thread *threads = new std::thread[thread_count];
	for(int thread_id = 0; thread_id < thread_count; thread_id++) {
		threads[thread_id] = std::thread(
			&renderer::threadFunction,
			this,
			std::ref(tile_id),
			p_sampleCount,
			p_bounceLimit
		);
	}

	for(int thread_id = 0; thread_id < thread_count; thread_id++) {
		threads[thread_id].join();
	}

	delete[] threads;

	m_image->writeImage();
}