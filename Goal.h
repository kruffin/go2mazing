#pragma once
#include "Player.h"

class Goal : public Player {
public:
	bool hidden;

	Goal() : Player() {
		this->color = C_MEDIUM_SPRING_GREEN;
		this->hidden = false;
	};
	Goal(int x, int y) : Player(x, y) {
		this->color = C_MEDIUM_SPRING_GREEN;
		this->hidden = false;
	};
	~Goal() {

	};

	void draw() {
		if (!this->hidden) {
			Player::draw();
		}
	}
};