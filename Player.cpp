#include "Player.h"

Player::Player(int x, int y) {
	this->cell_x = x;
	this->cell_y = y;
	this->world_x = 0;
	this->world_y = 0;

	this->width = 5;
	this->height = 5;
	this->color = C_CRIMSON;
};

Player::Player() {
	this->cell_x = 0;
	this->cell_y = 0;
	this->world_x = 0;
	this->world_y = 0;

	this->width = 5;
	this->height = 5;
	this->color = C_CRIMSON;
};

Player::~Player() {

};

void Player::update(double dt, double totalTime) {

};

void Player::draw() {
	UG_FillFrame(this->world_x, this->world_y,
				 this->world_x + this->width - 1, this->world_y + this->height - 1,
				 this->color);
};