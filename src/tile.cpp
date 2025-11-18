#include "tile.hpp"

tileGenerator::tileGenerator() : tileGenerator(vec2i(1), vec2i(1)) {}
tileGenerator::tileGenerator(const vec2i &p_imageResolution, const vec2i &p_tileDimension) : m_imageResolution(p_imageResolution), m_tileDimension(p_tileDimension) {
	for(int dim = 0; dim < 2; dim++) { //only a 2d tile generator
		m_tileCount[dim] = (m_imageResolution[dim] + m_tileDimension[dim] - 1) / m_tileDimension[dim]; //get tiles in each direction
	}
}

int tileGenerator::totalTiles() const {return m_tileCount[0] * m_tileCount[1];}
tile tileGenerator::at(int p_id) const {
	tile _tile;
	_tile.d_id = p_id;
	_tile.d_coordinate = vec2i{
		p_id % m_tileCount[0],
		p_id / m_tileCount[0],
	};

	for(int dim = 0; dim < 2; dim++) { 
		_tile.d_start[dim] = _tile.d_coordinate[dim] * m_tileDimension[dim]; //now we have the pixel coord of the top left corner
		_tile.d_dimension[dim] = std::min(m_tileDimension[dim], m_imageResolution[dim] - _tile.d_start[dim]); //this allows for the widht and height of imperct fitting tiles
	}
	return _tile;
} 