#pragma once
#ifndef USE_GLFW
#include <go2/input.h>
#include <go2/display.h>
#include "../audio.h"

#include "BaseWnd.h"

class Go2LibWnd : public BaseWnd {
public:
	Go2LibWnd();
	~Go2LibWnd();

	void init();
	void destroy();
	void blit(int x, int y, Sprite *s, int frame);
	void setPixel(int x, int y, unsigned char *color, size_t color_size);
	void swapBuffer();
	void getInput(BaseInput *input);
	void playSound(const short* data, int frames);
	void stopSounds();

	int getWidth();
	int getHeight();

	go2_gamepad_state_t outGamepadState;
	go2_display_t* display;
	go2_surface_t* surface;
	go2_presenter_t* presenter;
	go2_input_t* input;
	go2_audio_t* audio;

	int height;
	int width;
	int bytes_per_pixel;
};
#endif