#include <iostream>

#include "Player.h"
#include "Drawing.h"

Player::Player(int x, int y) {
	this->cell_x = x;
	this->cell_y = y;
	this->world_x = 0.0;
	this->world_y = 0.0;

	this->width = 5;
	this->height = 5;
	this->color = C_CRIMSON;
	this->prill = Sprite();
	this->currentFrame = 0;
	this->animation = 0;
};

Player::Player() {
	this->cell_x = 0;
	this->cell_y = 0;
	this->world_x = 0.0;
	this->world_y = 0.0;

	this->width = 5;
	this->height = 5;
	this->color = C_CRIMSON;
	this->prill = Sprite();
	this->currentFrame = 0;
	this->animation = 0;
};

Player::~Player() {

};

void Player::update(double dt, double totalTime) {

	this->currentFrame = this->animation * this->prill.frameCols + (int(totalTime * 2) % this->prill.frameCols);

	// std::cout << "current frame: " << this->currentFrame << std::endl;
};

bool Player::load(std::string imageFilename, int frameCols, int frameRows, int spacing) {
	if (!this->prill.load(imageFilename, frameCols, frameRows, spacing)) {
		std::cout << "Failed to load image:" << imageFilename + "images/prill.png" << std::endl;
		return false;
	}

	this->width = this->prill.frameWidth;
	this->height = this->prill.frameHeight;

	return true;
}

void Player::draw() {
	// UG_FillFrame(this->world_x, this->world_y,
	// 			 this->world_x + this->width - 1, this->world_y + this->height - 1,
	// 			 this->color);

	KR_blit(int(this->world_x), int(this->world_y), &this->prill, this->currentFrame);
};