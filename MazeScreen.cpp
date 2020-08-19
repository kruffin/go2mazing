#include <iostream>
#include <ostream>
#include <algorithm>

#include "MazeScreen.h"

MazeScreen::MazeScreen(int screen_width, int screen_height) {
	this->screen_width = screen_width;
	this->screen_height = screen_height;
	this->no_maze_str = std::string("No Maze Loaded.");
	this->maze_data = NULL;
	this->point_size = 5;
	this->offset_y = 50;

	this->max_cols = (this->screen_width - 2*this->point_size) / this->point_size;
	if (this->max_cols % 2 == 0) {
		this->max_cols--;
	}
	this->max_rows = (this->screen_height - 1 - this->offset_y) / this->point_size;
	if (this->max_rows % 2 == 0) {
		this->max_rows--;
	}
	this->player = Player(0,0);
	this->goal = Goal(0,0);
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
		UG_PutString(20, 10, (char *)lvl);

		short begin_x = this->screen_width/2 - (this->maze_data->columns * this->point_size / 2);
		short begin_y = (this->screen_height - this->offset_y)/2 + this->offset_y - (this->maze_data->rows * this->point_size / 2);

		this->player.world_x = begin_x + this->player.cell_x * this->point_size;
		this->player.world_y = begin_y + this->player.cell_y * this->point_size;

		this->goal.world_x = begin_x + this->goal.cell_x * this->point_size;
		this->goal.world_y = begin_y + this->goal.cell_y * this->point_size;

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
					UG_FillFrame(begin_x + x*this->point_size, begin_y + y*this->point_size,
								 begin_x + (x + 1)*this->point_size, begin_y + (y + 1)*this->point_size,
								 c);
					// UG_DrawPixel(begin_x + x, begin_y + y, c);
				}
			}
		}

		this->player.draw();
		this->goal.draw();
	}
}

bool MazeScreen::load(std::string programPath) {
	return true;
}

bool MazeScreen::handleInput(go2_gamepad_state_t *gamepad) {
	if (gamepad->buttons.top_left || gamepad->buttons.top_right) {

		if (1 == this->maze_data->level && gamepad->buttons.top_left) {
			change_scene(SCREEN_TITLE, (SceneData *)NULL);
		} else if(gamepad->buttons.top_left) {
			MazeData *data = new MazeData(std::clamp(11 + (this->maze_data->level - 2) * 2, 1, this->max_cols),
										  std::clamp(11 + (this->maze_data->level - 2) * 2, 1, this->max_rows),
										  this->maze_data->level - 1);
			change_scene(SCREEN_MAZE, data);
		}
		else {
			MazeData *data = new MazeData(std::clamp(11 + this->maze_data->level * 2, 1, this->max_cols),
										  std::clamp(11 + this->maze_data->level * 2, 1, this->max_rows),
										  this->maze_data->level + 1);
			change_scene(SCREEN_MAZE, data);
		}
		return true;
	}

	if (gamepad->dpad.left) {
		int new_x = std::clamp(this->player.cell_x - 1, -1, this->maze_data->columns);
		if (this->maze_data->get(new_x, this->player.cell_y) != MazeData::WALL) {
			this->player.cell_x = new_x;
		} else {
			// play bonk
		}
		return true;
	} else if (gamepad->dpad.right) {
		int new_x = std::clamp(this->player.cell_x + 1, -1, this->maze_data->columns);
		if (this->maze_data->get(new_x, this->player.cell_y) != MazeData::WALL) {
			this->player.cell_x = new_x;
		} else {
			// play bonk
		}
		return true;
	} else if (gamepad->dpad.up && -1 != this->player.cell_x) {
		int new_y = std::clamp(this->player.cell_y - 1, 0, this->maze_data->rows - 1);
		if (this->maze_data->get(this->player.cell_x, new_y) != MazeData::WALL) {
			this->player.cell_y = new_y;
		} else {
			// play bonk;
		}
		return true;
	} else if (gamepad->dpad.down && -1 != this->player.cell_x) {
		int new_y = std::clamp(this->player.cell_y + 1, 0, this->maze_data->rows - 1);
		if (this->maze_data->get(this->player.cell_x, new_y) != MazeData::WALL) {
			this->player.cell_y = new_y;
		} else {
			// play bonk;
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
		MazeData::maze_cell entrances[2];
		this->maze_data->generate((MazeData::maze_cell *)&entrances);

		this->player.cell_x = -1;
		this->player.cell_y = entrances[0].y * 2 + 1;

		this->goal.cell_x = (entrances[1].x + 1) * 2 + 1;
		this->goal.cell_y = entrances[1].y * 2 + 1;

	} else {
		std::cout << "Incorrect data sent to maze scene: " << data->getType() << std::endl;
		if (NULL != data) {
			delete data;
		}
	}
}