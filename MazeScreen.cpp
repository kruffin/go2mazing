#include <iostream>
#include <ostream>
#include <algorithm>
#include <chrono>

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
	this->tree = Goal();
	this->entrance = Goal();
	this->green_door = Goal();
	this->green_key = Key();

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

	this->entrance.update(dt, totalTime);
	this->tree.update(dt, totalTime);
	this->exit.update(dt, totalTime);
	this->player.update(dt, totalTime);
	this->goal.update(dt, totalTime);
	this->green_door.update(dt, totalTime);
	this->green_key.update(dt, totalTime);

	if (this->green_key.on_target && !this->green_key.hidden) {
		this->green_key.hidden = true;
		this->green_door.hidden = true;
	}
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
					case MazeData::DOOR_GREEN:
						path_frame = this->paths.frameCols * 3 + (this->maze_data->columns * y + x) % this->paths.frameCols;
						// c = C_DARK_ORCHID;
						break;
					case MazeData::KEY_GREEN:
						path_frame = this->paths.frameCols * 3 + (this->maze_data->columns * y + x) % this->paths.frameCols;
						// c = C_DARK_ORCHID;
						break;
					case MazeData::WALKED_GREEN_KEY:
						path_frame = this->paths.frameCols * 4 + (this->maze_data->columns * y + x) % this->paths.frameCols;
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

		this->entrance.draw();
		this->tree.draw();
		this->exit.draw();
		this->green_door.draw();
		// if (this->player.keys.find(MazeData::KEY_GREEN) == std::string::npos) {
		this->green_key.draw();
		// }
		this->player.draw();
		this->goal.draw();
	}
}

bool MazeScreen::load(std::string programPath) {
	if (!this->player.load(programPath + "images/candle_girl_11.png", 4, 4, 0)) {
		return false;
	}
	if (!this->goal.load(programPath + "images/cake_11.png", 4, 1, 0)) {
		return false;
	}
	if (!this->walls.load(programPath + "images/walls_11.png", 8, 1, 0)) {
		return false;
	}
	if (!this->paths.load(programPath + "images/paths_11.png", 8, 5, 0)) {
		return false;
	}
	if (!this->exit.load(programPath + "images/exit_33.png", 1, 2, 0)) {
		return false;
	}
	if (!this->entrance.load(programPath + "images/exit_33.png", 1, 2, 0)) {
		return false;
	}
	if (!this->tree.load(programPath + "images/trees1_22.png", 4, 1, 0)) {
		return false;
	}
	if (!this->green_door.load(programPath + "images/door_11.png", 18, 1, 0)) {
		return false;
	}
	if (!this->green_key.load(programPath + "images/key_11.png", 10, 1, 0)) {
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

void MazeScreen::movePLayer(int new_x, int new_y, int direction) {
	const char floor_tile = this->maze_data->get(new_x, new_y);
	const char old_floor_tile = this->maze_data->get(this->player.cell_x, this->player.cell_y);

	// std::cout << "old tile (" << this->player.cell_x << ", " << this->player.cell_y << ") new tile: (" << new_x << ", " << new_y << ")" << std::endl;
	if (floor_tile != MazeData::WALL 
		&& 
		(floor_tile != MazeData::DOOR_GREEN || this->player.keys.find(MazeData::KEY_GREEN) != std::string::npos)) {
		// only overwrite these particular tiles

		this->maze_data->replaceAllTiles(MazeData::WALKED, MazeData::REWALKED);
		std::deque<MazeData::maze_cell> walkPath = this->maze_data->findPath(new_x, new_y);
		for(int i = 0; i < walkPath.size(); ++i) {
			MazeData::maze_cell c = walkPath[i];
			const char tile = this->maze_data->get(c.x, c.y);
			if (MazeData::REWALKED == tile || MazeData::PATH == tile) {
				this->maze_data->set(c.x, c.y, MazeData::WALKED);
			}
		}

		if (MazeData::KEY_GREEN == floor_tile) {
			this->player.keys += MazeData::KEY_GREEN;
			this->green_key.obtained = true;
			// Color path green
			this->maze_data->replaceAllTiles(MazeData::WALKED, MazeData::WALKED_GREEN_KEY);
		}

		if (MazeData::DOOR_GREEN == floor_tile) {
			// We walked on the door; make it opened.
			this->green_door.hidden = true;
		}

		this->player.cell_x = new_x;
		this->player.cell_y = new_y;
		this->player.animation = direction;

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
		this->player.animation = direction;
	}
};

bool MazeScreen::handleInput(BaseInput *gamepad) {
	if (this->maze_complete > 0) {
		return false; // Don't allow input
	}

	if (gamepad->lt1 || gamepad->rt1) {

		if (1 == this->maze_data->level && gamepad->lt1) {
			change_scene(SCREEN_TITLE, (SceneData *)NULL);
		} else if(gamepad->lt1) {
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


	if (gamepad->left) {
		int new_x = std::clamp(this->player.cell_x - 1, -1, this->maze_data->columns);
		MazeData::maze_cell new_cell = this->rideRoad(this->player.cell_x, this->player.cell_y, new_x, this->player.cell_y);
		this->movePLayer(new_cell.x, new_cell.y, Player::ANIM_LEFT);
		return true;
	} else if (gamepad->right) {
		int new_x = std::clamp(this->player.cell_x + 1, -1, this->maze_data->columns);
		MazeData::maze_cell new_cell = this->rideRoad(this->player.cell_x, this->player.cell_y, new_x, this->player.cell_y);
		this->movePLayer(new_cell.x, new_cell.y, Player::ANIM_RIGHT);
		return true;
	} else if (gamepad->up && -1 != this->player.cell_x) {
		int new_y = std::clamp(this->player.cell_y - 1, 0, this->maze_data->rows - 1);
		MazeData::maze_cell new_cell = this->rideRoad(this->player.cell_x, this->player.cell_y, this->player.cell_x, new_y);
		this->movePLayer(new_cell.x, new_cell.y, Player::ANIM_UP);
		return true;
	} else if (gamepad->down && -1 != this->player.cell_x) {
		int new_y = std::clamp(this->player.cell_y + 1, 0, this->maze_data->rows - 1);
		MazeData::maze_cell new_cell = this->rideRoad(this->player.cell_x, this->player.cell_y, this->player.cell_x, new_y);
		this->movePLayer(new_cell.x, new_cell.y, Player::ANIM_DOWN);
		return true;
	}
	return false;
}

MazeData::maze_cell MazeScreen::rideRoad(int start_x, int start_y, int end_x, int end_y) {
	//std::cout << "start x:" << start_x << ", start y:" << start_y << "end x:" << end_x << ", end y:" << end_y << std::endl;
	if (start_x == end_x && start_y == end_y) {
		return {start_x, start_y};
	} else if (start_x < 0) { // The player starts outside of the maze.
		return {end_x, end_y};
	}

	// Count neighbors; if more than one stop; if only one and of same type, continue
	MazeData::complexity_cell * previous = this->maze_data->findGraphNode(start_x, start_y);
	MazeData::complexity_cell * current = this->maze_data->findGraphNode(end_x, end_y);
	if (NULL == current) {
		// Probably a wall.
		return {start_x, start_y};
	}
	MazeData::complexity_cell * tmp = NULL;
	while (tmp = this->nextOnRoad(current, previous)) {
		previous = current;
		current = tmp;
	}

	return {current->x, current->y};
};

// Get the next cell on the current road (same color) or NULL if the road stops.
MazeData::complexity_cell * MazeScreen::nextOnRoad(MazeData::complexity_cell *target, MazeData::complexity_cell *excluded) {
	MazeData::complexity_cell *to_ret = NULL;
	char target_type = this->maze_data->get(target->x, target->y);
	char excluded_type = this->maze_data->get(excluded->x, excluded->y);
	// emergent behavior here since while walking our previous spot is marked as walked while new
	// prospective spots are still unwalked paths. this means our normal walking still moves one
	// tile at a time, while going back over old paths will allow us to ride it <- insert cowboy dance.
	if (target->parent != excluded && target->parent != NULL && target_type == excluded_type) {
		to_ret = target->parent;
	}

	for (int i = 0; i < target->children.size(); ++i) {
		if (target->children[i] != excluded) {
			target_type = this->maze_data->get(target->children[i]->x, target->children[i]->y);
			if (to_ret != NULL || target_type != excluded_type) {
				return NULL; // Too many options or not the same type.
			} else {
				to_ret = target->children[i];
			}
		}
	}

	return to_ret;
};

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
		this->player.keys = std::string(""); // clear out the keys

		this->goal.world_x = this->begin_x + this->goal.cell_x * this->point_size;
		this->goal.world_y = this->begin_y + this->goal.cell_y * this->point_size;

		this->exit.world_x = this->goal.world_x;
		this->exit.world_y = std::max(double(this->begin_y), this->goal.world_y + (this->goal.height / 2) - (this->exit.height / 2));


		this->tree.world_x = std::max(this->begin_x - this->tree.width, 0);
		this->tree.world_y = this->player.world_y - (this->tree.height / 2);

		this->entrance.world_x = this->begin_x - this->entrance.width;
		this->entrance.world_y = std::max(double(this->begin_y), 
										this->player.world_y + (this->player.height / 2) - (this->entrance.height / 2));
		this->entrance.animation = 1;

		int door_x = 0, door_y = 0;
		int key_x = 0, key_y = 0;
		for (int x = 0; x < this->maze_data->columns; ++x) {
			for (int y = 0; y < this->maze_data->rows; ++y) {
				const char tile = this->maze_data->get(x, y);
				if (MazeData::DOOR_GREEN == tile) {
					door_x = x;
					door_y = y;
				} else if (MazeData::KEY_GREEN == tile) {
					key_x = x;
					key_y = y;
				}
			}
		}
		this->green_door.world_x = this->begin_x + door_x * this->point_size;
		this->green_door.world_y = this->begin_y + door_y * this->point_size;
		this->green_door.hidden = false;
		this->green_key.world_x = this->begin_x + key_x * this->point_size;
		this->green_key.world_y = this->begin_y + key_y * this->point_size;
		this->green_key.obtained = false;
		this->green_key.hidden = false;
		this->green_key.on_target = false;
		this->green_key.world_inv_x = this->green_door.world_x;
		this->green_key.world_inv_y = this->green_door.world_y;
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