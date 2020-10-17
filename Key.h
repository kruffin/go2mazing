#pragma once
#include "Player.h"

class Key : public Player {
public:
	bool hidden;
	bool obtained;
	double world_inv_x;
	double world_inv_y;
	double speed;
	bool on_target;

	Key() : Player() {
		this->color = C_MEDIUM_SPRING_GREEN;
		this->obtained = false;
		this->world_inv_x = 50;
		this->world_inv_y = 40;
		this->speed = 40.0;
		this->hidden = false;
		this->on_target = false;
	};
	Key(int x, int y) : Player(x, y) {
		this->color = C_MEDIUM_SPRING_GREEN;
		this->obtained = false;
		this->world_inv_x = 50;
		this->world_inv_y = 40;
		this->speed = 40.0;
		this->hidden = false;
		this->on_target = false;
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

		if (!this->on_target && this->world_x == this->world_inv_x && this->world_y == this->world_inv_y) {
			this->on_target = true;
		}
	};

	void draw() {
		if (!this->hidden) {
			Player::draw();
		}
	}
};