//#include "Heatmap.h"
//#include "staticfunctions.h"
//
//Heatmap::Heatmap(
//	float crater_intensity,
//	float median_crater_diameter,
//	float crater_diameter_deviation,
//	float max_crater_diameter,
//	float min_crater_diameter,
//	float average_terrain_height,
//	float terrain_height_deviation,
//	float max_terrain_height,
//	float min_terrain_height
//){
//	this->crater_intensity = 
//		crater_intensity >= 0 && crater_intensity <= 80 
//		? crater_intensity 
//		: throw std::invalid_argument("crater_intensity must be a number between 0 and 80");
//	this->min_crater_diameter = 
//		min_crater_diameter > 0 && min_crater_diameter < 50 
//		? min_crater_diameter 
//		: throw std::invalid_argument("min_crater_diameter must be a number between 0 and 50");
//	this->max_crater_diameter = 
//		max_crater_diameter < 1500 && max_crater_diameter > min_crater_diameter 
//		? max_crater_diameter 
//		: throw std::invalid_argument("max_crater_diameter must be a number between 'min_crater_diameter' and 1500");
//	this->median_crater_diameter = 
//		median_crater_diameter < max_crater_diameter && median_crater_diameter > min_crater_diameter 
//		? median_crater_diameter 
//		: throw std::invalid_argument("median_crater_diameter must be a number between 'min_crater_diameter' and 'max_crater_diameter'");
//	this->crater_diameter_deviation = 
//		crater_diameter_deviation > 0 && crater_diameter_deviation < 1 
//		? crater_diameter_deviation 
//		: throw std::invalid_argument("crater_diameter_deviation must be a number between 0 and 1");
//	this->terrain_height_deviation = 
//		terrain_height_deviation > 0 && terrain_height_deviation < 1 
//		? terrain_height_deviation 
//		: throw std::invalid_argument("terrain_height_deviation must be a number between 0 and 1");
//	this->average_terrain_height = 
//		average_terrain_height > min_terrain_height && average_terrain_height < max_terrain_height 
//		? average_terrain_height 
//		: throw std::invalid_argument("average_terrain_height must be a number between 'min_terrain_height' and 'max_terrain_height'");
//	this->max_terrain_height = 
//		max_terrain_height < 250 && max_terrain_height > min_terrain_height 
//		? max_terrain_height 
//		: throw std::invalid_argument("max_terrain_height must be a number between 'min_terrain_height' and 250");
//	this->min_terrain_height = 
//		min_terrain_height > -50 && min_terrain_height < 150 
//		? min_terrain_height 
//		: throw std::invalid_argument("min_terrain_height must be a number between -50 and 150");
//
//	//generating the base heatmap, 1 pixel = 25m
//	heatmap = std::vector<std::vector<float>>( 40000, std::vector<float>( 40000, 0.0 ) );
//
//	generateHeatmap();
//}
//const std::vector<std::vector<float>>& Heatmap::getHeatmap()
//{
//	return this->heatmap;
//}
//void Heatmap::generateHeatmap()
//{
//	for (int x = 0; x < heatmap.size(); x++) {
//		for (int y = 0; y < heatmap.at(0).size(); y++) {
//			float noise_value = staticfunctions::perlin_noise(x / 10.0, y / 10.0);
//			float scaled_value = min_terrain_height + noise_value * (max_terrain_height - min_terrain_height);
//			heatmap[x][y] = scaled_value;
//		}
//	}
//	
//}
//void Heatmap::printPNG() {
//	staticfunctions::write_png_file("heatmap.png", heatmap.size(), heatmap.at(0).size(), heatmap);
//}
//Heatmap::~Heatmap()
//{
//	heatmap.~vector();
//}