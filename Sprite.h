#pragma once

#include <png.h>
#include <string>

class Sprite {
public:
	int frameCols;
	int frameRows;

	Sprite();
	~Sprite();

	bool load(std::string filename, int frameCols, int frameRows, int spacingPixels);
	void blit(int frame, int x, int y, uint8_t*dest, int stride, int dest_width, int dest_height, int dest_bpp);
private:
	int frameWidth;
	int frameHeight;
	int frameSpacingPixels;
	png_uint_32 width;
	png_uint_32 height;
	int bitDepth;
	int colorType;
	unsigned char *data;
	png_uint_32 rowBytes;

	double lut_exponent;
	double crt_exponent;
	double display_exponent;
};