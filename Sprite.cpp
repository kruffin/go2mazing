#include <iostream>
#include <fstream>
#include <cstring>

#include "Sprite.h"

Sprite::Sprite() {

	this->crt_exponent = 2.2;

	#if defined(NeXT)
		this->lut_exponent = 1.0 / 2.2;
	#elif defined(sgi)
		this->lut_exponent = 1.0 / 1.7;
	#elif defined(Macintosh)
		this->lut_exponent = 1.8 / 2.61;
	#else
		this->lut_exponent = 1.0;
	#endif

	char *gamma;
	if ((gamma = std::getenv("SCREEN_GAMMA")) != NULL) {
		this->display_exponent = std::atof(gamma);
	} else {
		this->display_exponent = this->lut_exponent * this->crt_exponent;
	}

	this->data = NULL;
};

Sprite::~Sprite() {
	if (NULL != this->data) {
		free(this->data);
	}
};

bool Sprite::load(std::string filename, int frameCols, int frameRows, int spacingPixels) {
	if (NULL != this->data) {
		free(this->data);
		this->data = NULL;
	}

	// http://www.libpng.org/pub/png/book/chapter13.html#png.ch13.div.8
	// http://www.libpng.org/pub/png/libpng.html
	std::cout << "Loading file: " << filename << std::endl;
	FILE *imageFile;
	imageFile = fopen(const_cast<char *>(filename.c_str()), "rb");

	if (!imageFile) {
		return false;
	}

	png_structp pngFile = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!pngFile) {
		fclose(imageFile);
		return false;
	}

	png_infop pngInfo = png_create_info_struct(pngFile);
	if (!pngInfo) {
		png_destroy_read_struct(&pngFile, NULL, NULL);
		fclose(imageFile);
		return false;
	}

	if (setjmp(png_jmpbuf(pngFile))) {
		png_destroy_read_struct(&pngFile, &pngInfo, NULL);
		fclose(imageFile);
	}

	png_init_io(pngFile, imageFile);
	// png_set_sig_bytes(pngFile, 8);
	png_read_info(pngFile, pngInfo);

	png_get_IHDR(pngFile, pngInfo, &(this->width), &(this->height), &(this->bitDepth), &(this->colorType), NULL, NULL, NULL);

	this->frameCols = frameCols;
	this->frameRows = frameRows;
	this->frameSpacingPixels = spacingPixels;
	this->frameWidth = this->width / this->frameCols - this->frameCols * this->frameSpacingPixels;
	this->frameHeight = this->height / this->frameRows - this->frameRows * this->frameSpacingPixels;
	std::cout << "bit depth: " << this->bitDepth << "| color type: " << this->colorType << "| width: " << this->width << "| height: " << this->height << std::endl;
	std::cout << "cols/rows: " << this->frameCols << ", " << this->frameRows << "| frame width: " << this->frameWidth << "| frame height: " << this->frameHeight << std::endl;

	if (this->colorType == PNG_COLOR_TYPE_PALETTE) {
        png_set_expand(pngFile);
	}
    if (this->colorType == PNG_COLOR_TYPE_GRAY && this->bitDepth < 8) {
        png_set_expand(pngFile);
    }
    if (png_get_valid(pngFile, pngInfo, PNG_INFO_tRNS)) {
        png_set_expand(pngFile);
    }
    if (this->bitDepth == 16) {
        png_set_strip_16(pngFile);
    }
    if (this->colorType == PNG_COLOR_TYPE_GRAY ||
        this->colorType == PNG_COLOR_TYPE_GRAY_ALPHA) {
        png_set_gray_to_rgb(pngFile);
	}

	double gamma;
	if (png_get_gAMA(pngFile, pngInfo, &gamma)) {
        png_set_gamma(pngFile, this->display_exponent, gamma);
	}

	png_read_update_info(pngFile, pngInfo);
	this->rowBytes = png_get_rowbytes(pngFile, pngInfo);
	int channels = (int)png_get_channels(pngFile, pngInfo);

	this->data = (unsigned char *)malloc(this->rowBytes * this->height);
	if (this->data == NULL) {
		png_destroy_read_struct(&pngFile, &pngInfo, NULL);
		fclose(imageFile);
		return false;
	}

	png_bytepp rows = (png_bytepp)malloc(this->height * sizeof(png_bytep));
	if (rows == NULL) {
		png_destroy_read_struct(&pngFile, &pngInfo, NULL);
		fclose(imageFile);
		free(this->data);
		this->data = NULL;
		return false;
	}

	for(int i = 0; i < this->height; ++i) {
		rows[i] = this->data + i * this->rowBytes;
	}

	png_read_image(pngFile, rows);


	free(rows);
	png_destroy_read_struct(&pngFile, &pngInfo, NULL);
	fclose(imageFile);

	return true;
};

void Sprite::blit(int frame, int x, int y, uint8_t*dest, int stride, int dest_width, int dest_height, int dest_bpp) {
	if (NULL == this->data) {
		return;
	}

	int frame_row = frame / this->frameCols;
	int frame_col = frame - frame_row * this->frameCols;
	int src_start_x = frame_col * this->frameWidth + frame_col * this->frameSpacingPixels;
	int src_start_y = frame_row * this->frameHeight + frame_row * this->frameSpacingPixels;
	int src_end_x = src_start_x + this->frameWidth;
	int src_end_y = src_start_y + this->frameHeight;

	for (int src_y = src_start_y; src_y < src_end_y; ++src_y) {
		for (int src_x = src_start_x; src_x < src_end_x; ++src_x) {
			int dest_y = dest_height - 1 - x - (src_x - src_start_x);
			int dest_x = (src_y - src_start_y) + y;
			if (dest_y < 0 || dest_y >= dest_height ||
				dest_x < 0 || dest_x >= dest_width) {
				continue; // outside the destination bounds.
			}

			png_bytep cur_pixel = this->data + src_y * this->rowBytes + src_x * 4;

			if (cur_pixel[3] == 0) {
				// Ignore transparent pixels.
				continue;
			}
			//https://www.barth-dev.de/about-rgb565-and-how-to-convert-into-it/
			uint16_t rgb565;
			rgb565 = 	((cur_pixel[0] & 0b11111000) << 8) + 
						((cur_pixel[1] & 0b11111100) << 3) +
						(cur_pixel[2] >> 3);

			memcpy(	dest + (dest_y * stride + dest_x*dest_bpp/8), 
			(unsigned char*)&rgb565, sizeof(rgb565));
		}
	}
	
};

Sprite::Pixel Sprite::getPixel(int frame, int x, int y) {
	if (NULL == this->data) {
		return Sprite::Pixel { 0, 0 };
	}

	int frame_row = frame / this->frameCols;
	int frame_col = frame - frame_row * this->frameCols;
	int src_start_x = frame_col * this->frameWidth + frame_col * this->frameSpacingPixels;
	int src_start_y = frame_row * this->frameHeight + frame_row * this->frameSpacingPixels;
	// int src_end_y = src_start_y + this->frameHeight - 1;

	png_bytep cur_pixel = this->data + (src_start_y + y) * this->rowBytes + (src_start_x + x) * 4;

	uint16_t rgb565;
	rgb565 = 	((cur_pixel[0] & 0b11111000) << 8) + 
				((cur_pixel[1] & 0b11111100) << 3) +
				(cur_pixel[2] >> 3);

	return Sprite::Pixel { rgb565, cur_pixel[3] };
};