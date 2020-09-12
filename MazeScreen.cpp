#include <iostream>
#include <ostream>
#include <algorithm>
#include <random>

#include "MazeScreen.h"
#include "Sound.h"
#include "Drawing.h"

MazeScreen::MazeScreen(int screen_width, int screen_height) {
	this->screen_width = screen_width;
	this->screen_height = screen_height;
	this->no_maze_str = std::string("No Maze Loaded.");
	this->maze_data = NULL;
	this->point_size = 11;//5;
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
	this->walls = Sprite();
	this->exit = Goal();

	this->maze_complete = -1.0;
	this->complete_generator = std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count());
	this->complete_dist = std::uniform_int_distribution<int>(0, 6);
}

MazeScreen::~MazeScreen() {
	if (NULL != this->maze_data) {
		std::cerr << "Deleting maze data." << std::endl;
		delete this->maze_data;
	}
	if (NULL != this->bonk_audio) {
		std::cerr << "Freeing bonk audio." << std::endl;
		free(this->bonk_audio);
	}
}

void MazeScreen::update(double dt, double totalTime) {
	if (this->maze_complete > 0) {
		double time_since_complete = double(clock() - this->maze_complete) / double(CLOCKS_PER_SEC);

		if (time_since_complete > 6.0) {
			MazeData *data = new MazeData(std::clamp(11 + this->maze_data->level * 2, 1, this->max_cols),
										  std::clamp(11 + this->maze_data->level * 2, 1, this->max_rows),
										  this->maze_data->level + 1);
			change_scene(SCREEN_MAZE, data);
		} else {
			this->goal.world_x = this->begin_x + this->goal.cell_x * this->point_size + sin(totalTime - this->maze_complete / double(CLOCKS_PER_SEC)) * 10.0;
			this->goal.world_y += dt * 20.0;

			// chase after the cake
			if (time_since_complete < 1.0) {
				this->player.animation = Player::ANIM_RIGHT;
				this->player.world_x += dt * (this->point_size + 1);
			} else {
				this->player.animation = Player::ANIM_DOWN;

				this->player.world_y += dt * (15.0 + this->complete_dist(this->complete_generator));				
			}
		}
	}

	this->exit.update(dt, totalTime);
	this->player.update(dt, totalTime);
	this->goal.update(dt, totalTime);
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

		for (short x = 0; x < this->maze_data->columns; ++x) {
			for (short y = 0; y < this->maze_data->rows; ++y) {
				char px = this->maze_data->get(x, y);
				UG_COLOR c = -1;
				int wall_frame = -1;
				int path_frame = -1;
				switch (px) {
					case MazeData::WALL:
						// c = C_INDIGO;
						wall_frame = (this->maze_data->columns * y + x) % this->walls.frameCols;
						break;
					case MazeData::PATH:
						path_frame = (this->maze_data->columns * y + x) % this->paths.frameCols;
						break;
					case MazeData::WALKED:
						path_frame = this->paths.frameCols + (this->maze_data->columns * y + x) % this->paths.frameCols;
						// c = C_DARK_ORCHID;
						break;
					case MazeData::REWALKED:
						path_frame = this->paths.frameCols * 2 + (this->maze_data->columns * y + x) % this->paths.frameCols;
						// c = C_DARK_ORCHID;
						break;
					case MazeData::UNDEFINED:
						c = C_BLACK;
						break;
				}
				if (-1 != c) {
					UG_FillFrame(this->begin_x + x*this->point_size, this->begin_y + y*this->point_size,
								 this->begin_x + (x + 1)*this->point_size, this->begin_y + (y + 1)*this->point_size,
								 c);
					// UG_DrawPixel(begin_x + x, begin_y + y, c);
				}
				if (-1 != wall_frame) {
					KR_blit(this->begin_x + x*this->point_size, this->begin_y + y*this->point_size,
							&this->walls, wall_frame);
				}
				if (-1 != path_frame) {
					KR_blit(this->begin_x + x*this->point_size, this->begin_y + y*this->point_size,
							&this->paths, path_frame);
				}
			}
		}

		this->exit.draw();
		this->player.draw();
		this->goal.draw();
	}
}

bool MazeScreen::load(std::string programPath) {
	if (!this->player.load(programPath + "images/candle_11.png", 4, 4, 0)) {
		return false;
	}
	if (!this->goal.load(programPath + "images/cake_11.png", 4, 1, 0)) {
		return false;
	}
	if (!this->walls.load(programPath + "images/walls_11.png", 8, 1, 0)) {
		return false;
	}
	if (!this->paths.load(programPath + "images/paths_11.png", 8, 3, 0)) {
		return false;
	}
	if (!this->exit.load(programPath + "images/exit_33.png", 1, 1, 0)) {
		return false;
	}


	drwav wav;
	if (!drwav_init_file(&wav, (programPath + "sounds/bonk.wav").c_str(), NULL)) {
		std::cout << "Failed to load sound." << std::endl;
		return false;
	} else {
		std::cout << "Channels: " << wav.channels << std::endl;
		this->bonk_audio = (drwav_int16*)malloc(wav.totalPCMFrameCount * wav.channels * sizeof(drwav_int16));
		this->bonk_samples = drwav_read_pcm_frames_s16(&wav, wav.totalPCMFrameCount, this->bonk_audio);
		this->bonk_frame_count = wav.totalPCMFrameCount;
		std::cout << "Samples: " << this->bonk_samples << " Frames: " << this->bonk_frame_count << std::endl;
		drwav_uninit(&wav);
	}

	return true;
}

bool MazeScreen::handleInput(BaseInput *gamepad) {
	if (this->maze_complete > 0) {
		return false; // Don't allow input
	}
	// if (gamepad->buttons.top_left || gamepad->buttons.top_right) {

	// 	if (1 == this->maze_data->level && gamepad->buttons.top_left) {
	// 		change_scene(SCREEN_TITLE, (SceneData *)NULL);
	// 	} else if(gamepad->buttons.top_left) {
	// 		MazeData *data = new MazeData(std::clamp(11 + (this->maze_data->level - 2) * 2, 1, this->max_cols),
	// 									  std::clamp(11 + (this->maze_data->level - 2) * 2, 1, this->max_rows),
	// 									  this->maze_data->level - 1);
	// 		change_scene(SCREEN_MAZE, data);
	// 	}
	// 	// else {
	// 	// 	MazeData *data = new MazeData(std::clamp(11 + this->maze_data->level * 2, 1, this->max_cols),
	// 	// 								  std::clamp(11 + this->maze_data->level * 2, 1, this->max_rows),
	// 	// 								  this->maze_data->level + 1);
	// 	// 	change_scene(SCREEN_MAZE, data);
	// 	// }
	// 	return true;
	// }


	if (gamepad->left) {
		int new_x = std::clamp(this->player.cell_x - 1, -1, this->maze_data->columns);
		const char floor_tile = this->maze_data->get(new_x, this->player.cell_y);
		if (floor_tile != MazeData::WALL) {
			if (MazeData::WALKED != floor_tile) {
				this->maze_data->set(this->player.cell_x, this->player.cell_y, MazeData::WALKED);
			} else {
				this->maze_data->set(this->player.cell_x, this->player.cell_y, MazeData::REWALKED);
			}

			this->player.cell_x = new_x;
			this->player.animation = Player::ANIM_LEFT;

			this->player.world_x = this->begin_x + this->player.cell_x * this->point_size;
			this->player.world_y = this->begin_y + this->player.cell_y * this->point_size;

		} else {
			// play bonk
			play_sound(const_cast<short *>(this->bonk_audio), int(this->bonk_frame_count));
			this->player.animation = Player::ANIM_LEFT;
		}
		return true;
	} else if (gamepad->right) {
		int new_x = std::clamp(this->player.cell_x + 1, -1, this->maze_data->columns);
		const char floor_tile = this->maze_data->get(new_x, this->player.cell_y);
		if (floor_tile != MazeData::WALL) {
			if (MazeData::WALKED != floor_tile) {
				if (-1 != this->player.cell_x) {
					this->maze_data->set(this->player.cell_x, this->player.cell_y, MazeData::WALKED);
				}
			} else {
				this->maze_data->set(this->player.cell_x, this->player.cell_y, MazeData::REWALKED);
			}

			this->player.cell_x = new_x;
			this->player.animation = Player::ANIM_RIGHT;

			this->player.world_x = this->begin_x + this->player.cell_x * this->point_size;
			this->player.world_y = this->begin_y + this->player.cell_y * this->point_size;

			if (new_x == this->goal.cell_x - 1) {
				// Reached end of maze.
				this->maze_complete = clock();
				this->maze_data->set(this->player.cell_x, this->player.cell_y, MazeData::WALKED);
			}
		} else {
			// play bonk
			play_sound(const_cast<short *>(this->bonk_audio), int(this->bonk_frame_count));
			this->player.animation = Player::ANIM_RIGHT;
		}
		return true;
	} else if (gamepad->up && -1 != this->player.cell_x) {
		int new_y = std::clamp(this->player.cell_y - 1, 0, this->maze_data->rows - 1);
		const char floor_tile = this->maze_data->get(this->player.cell_x, new_y);
		if (floor_tile != MazeData::WALL) {
			if (MazeData::WALKED != floor_tile) {
				this->maze_data->set(this->player.cell_x, this->player.cell_y, MazeData::WALKED);
			} else {
				this->maze_data->set(this->player.cell_x, this->player.cell_y, MazeData::REWALKED);
			}

			this->player.cell_y = new_y;
			this->player.animation = Player::ANIM_UP;

			this->player.world_x = this->begin_x + this->player.cell_x * this->point_size;
			this->player.world_y = this->begin_y + this->player.cell_y * this->point_size;

		} else {
			// play bonk
			play_sound(const_cast<short *>(this->bonk_audio), int(this->bonk_frame_count));
			this->player.animation = Player::ANIM_UP;
		}
		return true;
	} else if (gamepad->down && -1 != this->player.cell_x) {
		int new_y = std::clamp(this->player.cell_y + 1, 0, this->maze_data->rows - 1);
		const char floor_tile = this->maze_data->get(this->player.cell_x, new_y);
		if (floor_tile != MazeData::WALL) {
			if (MazeData::WALKED != floor_tile) {
				this->maze_data->set(this->player.cell_x, this->player.cell_y, MazeData::WALKED);
			} else {
				this->maze_data->set(this->player.cell_x, this->player.cell_y, MazeData::REWALKED);
			}
			

			this->player.cell_y = new_y;
			this->player.animation = Player::ANIM_DOWN;

			this->player.world_x = this->begin_x + this->player.cell_x * this->point_size;
			this->player.world_y = this->begin_y + this->player.cell_y * this->point_size;

		} else {
			// play bonk
			play_sound(const_cast<short *>(this->bonk_audio), int(this->bonk_frame_count));
			this->player.animation = Player::ANIM_DOWN;
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
		this->maze_complete = -1.0;

		this->begin_x = this->screen_width/2 - (this->maze_data->columns * this->point_size / 2);
		this->begin_y = (this->screen_height - this->offset_y)/2 + this->offset_y - (this->maze_data->rows * this->point_size / 2);

		this->player.world_x = this->begin_x + this->player.cell_x * this->point_size;
		this->player.world_y = this->begin_y + this->player.cell_y * this->point_size;

		this->goal.world_x = this->begin_x + this->goal.cell_x * this->point_size;
		this->goal.world_y = this->begin_y + this->goal.cell_y * this->point_size;

		this->exit.world_x = this->goal.world_x;
		this->exit.world_y = std::max(double(this->begin_y), this->goal.world_y + (this->goal.height / 2) - (this->exit.height / 2));

		std::cout << "goal world pos: (" << this->goal.world_x << ", " << this->goal.world_y << ")" << std::endl;
		std::cout << "exit world pos: (" << this->exit.world_x << ", " << this->exit.world_y << ")" << std::endl;
		std::cout << "exit dimensions: (" << this->exit.width << ", " << this->exit.height << ")" << std::endl;

	} else {
		std::cout << "Incorrect data sent to maze scene: " << data->getType() << std::endl;
		if (NULL != data) {
			delete data;
		}
	}
}