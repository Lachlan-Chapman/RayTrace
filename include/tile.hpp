#pragma once
#include "vec.hpp"
struct tile {
	int d_id;
	vec2i d_coordinate; //tile coordinate in tile space so the (0, 0)th tile
	vec2i d_start; //the start pixel coord
	vec2i d_dimension; //how far to go across and down when rendering
};

class tileGenerator { //converts a resolution image into an array of tile objects given the set size (allows for non full tiles)
public:
	tileGenerator();
	tileGenerator(const vec2i &p_imageResolution, const vec2i &p_tileDimension);
	int totalTiles() const;
	tile operator[](int p_id) const {return at(p_id);} //hide the fact that this generates and act as if the entire set

private:
	tile at(int p_id) const; //creates a tile obj from id
	vec2i m_imageResolution, m_tileDimension, m_tileCount;
};