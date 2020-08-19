#pragma once
#include "lib/ugui/ugui.h"

class Player {
public:
	int cell_x;
	int cell_y;
	int world_x;
	int world_y;
	char width;
	char height;
	UG_COLOR color;
	Player();
	Player(int x, int y);
	~Player();
	void update(double dt, double totalTime);
	void draw();

private:
	
};