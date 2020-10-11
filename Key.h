#pragma once
#include "Player.h"

class Key : public Player {
public:
	bool obtained;
	double world_inv_x;
	double world_inv_y;
	double speed;

	Key() : Player() {
		this->color = C_MEDIUM_SPRING_GREEN;
		this->obtained = false;
		this->world_inv_x = 50;
		this->world_inv_y = 40;
		this->speed = 40.0;
	};
	Key(int x, int y) : Player(x, y) {
		this->color = C_MEDIUM_SPRING_GREEN;
		this->obtained = false;
		this->world_inv_x = 50;
		this->world_inv_y = 40;
		this->speed = 40.0;
	};
	~Key() {

	};

	void update(double dt, double totalTime) {
		this->currentFrame = this->animation * this->prill.frameCols + (int(totalTime * 2) % this->prill.frameCols);

		if (this->obtained && this->world_x != this->world_inv_x) {
			if (this->world_x < this->world_inv_x) {
				this->world_x += dt * this->speed;
				if (this->world_x > this->world_inv_x) {
					// Went to far
					this->world_x = this->world_inv_x;
				}
			} else {
				this->world_x -= dt * this->speed;
				if (this->world_x < this->world_inv_x) {
					// Went to far
					this->world_x = this->world_inv_x;
				}
			}
		}
		if (this->obtained && this->world_y != this->world_inv_y) {
			if (this->world_y < this->world_inv_y) {
				this->world_y += dt * this->speed;
				if (this->world_y > this->world_inv_y) {
					// Went to far
					this->world_y = this->world_inv_y;
				}
			} else {
				this->world_y -= dt * this->speed;
				if (this->world_y < this->world_inv_y) {
					// Went to far
					this->world_y = this->world_inv_y;
				}
			}
		}
	};
};