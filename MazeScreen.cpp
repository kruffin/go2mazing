#include <iostream>
#include <ostream>

#include "MazeScreen.h"

MazeScreen::MazeScreen(int screen_width, int screen_height) {
	this->screen_width = screen_width;
	this->screen_height = screen_height;
	this->no_maze_str = std::string("No Maze Loaded.");
	this->maze_data = NULL;
}

MazeScreen::~MazeScreen() {
	if (NULL != this->maze_data) {
		delete this->maze_data;
	}
}

void MazeScreen::update(double dt, double totalTime) {

}

void MazeScreen::draw() {
	if (NULL == this->maze_data) {
		UG_FontSelect(&FONT_22X36);
		UG_FillScreen(C_PALE_GOLDEN_ROD);

		UG_SetForecolor(C_BLACK);
		UG_SetBackcolor(C_PALE_GOLDEN_ROD);

		//std::cout << "x: " << g_x << " y: " << g_y << std::endl;
		UG_PutString(20, 20, const_cast<char*>(this->no_maze_str.c_str()));
	} else {
		UG_FillScreen(C_PALE_GOLDEN_ROD);
		UG_FontSelect(&FONT_22X36);
		UG_SetForecolor(C_BLACK);
		UG_SetBackcolor(C_PALE_GOLDEN_ROD);
		char lvl[50];
		sprintf(lvl, "Lvl: %d", this->maze_data->level);
		UG_PutString(20, 20, (char *)lvl);

		short point_size = 5;
		short begin_x = this->screen_width/2 - (this->maze_data->columns * point_size / 2);
		short begin_y = this->screen_height/2 - (this->maze_data->rows * point_size / 2);

		for (short x = 0; x < this->maze_data->columns; ++x) {
			for (short y = 0; y < this->maze_data->rows; ++y) {
				char px = this->maze_data->get(x, y);
				UG_COLOR c;
				switch (px) {
					case MazeData::WALL:
						c = C_INDIGO;
						break;
					case MazeData::PATH:
						c = -1;
						break;
					case MazeData::WALKED:
						c = C_DARK_ORCHID;
						break;
					case MazeData::UNDEFINED:
						c = C_BLACK;
						break;
				}
				if (-1 != c) {
					UG_FillFrame(begin_x + x*point_size, begin_y + y*point_size,
								 begin_x + (x + 1)*point_size, begin_y + (y + 1)*point_size,
								 c);
					// UG_DrawPixel(begin_x + x, begin_y + y, c);
				}
			}
		}
	}
}

bool MazeScreen::load(std::string programPath) {
	return true;
}

bool MazeScreen::handleInput(go2_gamepad_state_t *gamepad) {
	if (gamepad->dpad.left || gamepad->dpad.right ||
		gamepad->dpad.up || gamepad->dpad.down ||
		gamepad->buttons.a || gamepad->buttons.b ||
		gamepad->buttons.x || gamepad->buttons.y ||
		gamepad->buttons.top_left || gamepad->buttons.top_right) {

		if (10 == this->maze_data->level) {
			change_scene(SCREEN_TITLE, (SceneData *)NULL);
		} else {
			MazeData *data = new MazeData(11 + this->maze_data->level * 2,
										  11 + this->maze_data->level * 2,
										  this->maze_data->level + 1);
			change_scene(SCREEN_MAZE, data);
		}
		return true;
	}
	return false;
}

void MazeScreen::setSceneData(SceneData *data) {
	if (data->getType() == SceneData::TYPE_SCENE_MAZE) {
		if (NULL !=	 this->maze_data) {
			delete this->maze_data;
		}
		this->maze_data = static_cast<MazeData *>(data);
		this->maze_data->generate();
	} else {
		std::cout << "Incorrect data sent to maze scene: " << data->getType() << std::endl;
		if (NULL != data) {
			delete data;
		}
	}
}