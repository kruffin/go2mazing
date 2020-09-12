#pragma once
#include "lib/ugui/ugui.h"
#include "Sprite.h"

class Player {
public:
	int cell_x;
	int cell_y;
	double world_x;
	double world_y;
	int width;
	int height;
	UG_COLOR color;
	int animation; // Assumes each set of frameRow is an animation.
	Player();
	Player(int x, int y);
	~Player();
	void update(double dt, double totalTime);
	void draw();
	bool load(std::string imageFilename, int frameCols, int frameRows, int spacing);

	static const int ANIM_LEFT = 1;
	static const int ANIM_RIGHT = 2;
	static const int ANIM_UP = 0;
	static const int ANIM_DOWN = 3;

private:
	Sprite prill;
	int currentFrame;
};