#pragma once
#include "../Sprite.h"

struct BaseInput {
	bool a;
	bool b;
	bool x;
	bool y;
	bool lt1;
	bool rt1;

	bool f1;

	bool left;
	bool right;
	bool up;
	bool down;
};

class BaseWnd {
public:
	BaseWnd() {};
	~BaseWnd() {};

	virtual void init() {};
	virtual void destroy() {};
	virtual void blit(int x, int y, Sprite *s, int frame) {};
	virtual void setPixel(int x, int y, unsigned char *color, size_t color_size) {};
	virtual void swapBuffer() {};
	virtual void getInput(BaseInput *input) {};
	virtual void playSound(const short* data, int frames) {};
	virtual void stopSounds() {};

	virtual int getWidth() {return 0;};
	virtual int getHeight() {return 0;};
};