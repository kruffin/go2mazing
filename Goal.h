#pragma once
#include "Player.h"

class Goal : public Player {
public:
	Goal() : Player() {
		this->color = C_MEDIUM_SPRING_GREEN;
	};
	Goal(int x, int y) : Player(x, y) {
		this->color = C_MEDIUM_SPRING_GREEN;
	};
	~Goal() {

	};
};