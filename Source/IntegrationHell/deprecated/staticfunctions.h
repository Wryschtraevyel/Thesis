//#pragma once
//
//#include "CoreMinimal.h"
//#include <iostream>
//#include <vector>
//#include <random>
//#include <libPNG/libPNG-1.5.27/png.h>
//
//class INTEGRATIONHELL_API staticfunctions
//{
//public:
//	static float random_float() {
//		static std::random_device rd;
//		static std::mt19937 gen(rd());
//		static std::uniform_real_distribution<float> dis(0.0, 1.0);
//		return dis(gen);
//	}
//
//	static float perlin_noise(float x, float y) {
//		float total = 0;
//		float persistence = 0.5;
//		int octaves = 4;
//		float frequency = 1;
//		float amplitude = 1;
//		float max_value = 0;
//		for (int i = 0; i < octaves; i++) {
//			total += amplitude * (2 * random_float() - 1) * amplitude;
//			max_value += amplitude;
//			amplitude *= persistence;
//			frequency *= 2;
//		}
//		return total / max_value;
//	}
//	static // write 2D vector array to PNG image file
//        void write_png_file(const char* file_name, int width, int height, std::vector<std::vector<float>>& data) {
//        // open file for writing
//        FILE* fp = fopen(file_name, "wb");
//        if (!fp) {
//            std::cout << "Error: could not open file " << file_name << std::endl;
//            return;
//        }
//
//        // initialize PNG structures
//        png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
//        if (!png_ptr) {
//            std::cout << "Error: could not create PNG write struct" << std::endl;
//            fclose(fp);
//            return;
//        }
//
//        png_infop info_ptr = png_create_info_struct(png_ptr);
//        if (!info_ptr) {
//            std::cout << "Error: could not create PNG info struct" << std::endl;
//            png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
//            fclose(fp);
//            return;
//        }
//
//        // set PNG parameters
//        png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_GRAY,
//            PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
//
//        // allocate PNG row pointers
//        png_bytep* row_pointers = new png_bytep[height];
//        for (int y = 0; y < height; y++) {
//            row_pointers[y] = new png_byte[width];
//            for (int x = 0; x < width; x++) {
//                row_pointers[y][x] = static_cast<png_byte>(data[x][y] * 255.0);
//            }
//        }
//
//        // write PNG image data to file
//        png_init_io(png_ptr, fp);
//        png_set_rows(png_ptr, info_ptr, row_pointers);
//        png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
//
//        // cleanup
//        for (int y = 0; y < height; y++) {
//            delete[] row_pointers[y];
//        }
//        delete[] row_pointers;
//
//        png_destroy_write_struct(&png_ptr, &info_ptr);
//        fclose(fp);
//    }
//};
//
