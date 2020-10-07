#ifndef USE_GLFW

#include <drm/drm_fourcc.h>
#include <cstring>
#include <iostream>

#include "Go2LibWnd.h"
#include "../Sound.h"

Go2LibWnd::Go2LibWnd() {

};

Go2LibWnd::~Go2LibWnd() {

};

void Go2LibWnd::init() {
	this->input = go2_input_create();

	this->display = go2_display_create();
	this->presenter = go2_presenter_create(this->display, DRM_FORMAT_RGB565, 0xff00ff00); // ABGR
	this->height = go2_display_height_get(this->display);
	this->width = go2_display_width_get(this->display);
    this->surface = go2_surface_create(this->display, this->width, this->height, DRM_FORMAT_RGB565);

    this->bytes_per_pixel = go2_drm_format_get_bpp(go2_surface_format_get(this->surface)) / 8;

    this->audio = go2_audio_create(44100); // 44.1 kHz
    std::cerr << "created audio. Is null? " << (NULL == this->audio) << std::endl;
};

void Go2LibWnd::destroy() {
	go2_input_destroy(this->input);
	//go2_surface_destroy(surface);
	go2_presenter_destroy(this->presenter);
	go2_display_destroy(this->display);
	go2_audio_destroy(this->audio);
};

void Go2LibWnd::setPixel(int x, int y, unsigned char *color, size_t color_size) {
	uint8_t* dst = (uint8_t*)go2_surface_map(this->surface);
	
	// swap the x and y while translating x
	// →[*][ ][ ][ ]
	//  [ ][ ][ ][ ]
	// to:
	//  [ ][*]
	//  [ ][ ]
	//  [ ][ ]
	//  [ ][ ]
	//      ↑

	int yfinal = height - 1 - x;
	int xfinal = y;
	memcpy(dst + (yfinal * go2_surface_stride_get(this->surface) + xfinal * this->bytes_per_pixel), color, color_size);
};

void Go2LibWnd::blit(int x, int y, Sprite *s, int frame) {
	uint8_t* dst = (uint8_t*)go2_surface_map(this->surface);

	s->blit(frame, x, y, dst, go2_surface_stride_get(this->surface), this->width, this->height, this->bytes_per_pixel*8);
};

void Go2LibWnd::swapBuffer() {
	go2_presenter_post(this->presenter, this->surface, 
						0, 0, this->width, this->height,
						0, 0, this->width, this->height,
						GO2_ROTATION_DEGREES_0);
};

void Go2LibWnd::getInput(BaseInput *input) {
	go2_input_gamepad_read(this->input,&this->outGamepadState);

	input->a = this->outGamepadState.buttons.a;
	input->b = this->outGamepadState.buttons.b;
	input->x = this->outGamepadState.buttons.x;
	input->y = this->outGamepadState.buttons.y;
	input->lt1 = this->outGamepadState.buttons.top_left;
	input->rt1 = this->outGamepadState.buttons.top_right;
	input->f1 = this->outGamepadState.buttons.f1;
	input->left = this->outGamepadState.dpad.left;
	input->right = this->outGamepadState.dpad.right;
	input->up = this->outGamepadState.dpad.up;
	input->down = this->outGamepadState.dpad.down;
};

void Go2LibWnd::playSound(const short* data, int frames) {
	std::cerr << "playing a sound." << std::endl;
	go2_audio_submit_fix(this->audio, data, frames);
};

void Go2LibWnd::stopSounds() {
	std::cerr << "Stoping sounds." << std::endl;
	if (NULL != this->audio) {
		go2_audio_destroy(this->audio);
	}
	this->audio = go2_audio_create(44100); // 44.1 kHz
};

int Go2LibWnd::getWidth() {
	return this->width;
}

int Go2LibWnd::getHeight() {
	return this->height;
}

#endif