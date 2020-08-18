#pragma once

#include <random>
#include <chrono>

#include <iostream>
#include <ostream>

class SceneData {
public:
	SceneData() {};
	virtual ~SceneData() {};
	virtual int getType() { return TYPE_SCENE_BASE; }

	static const int TYPE_SCENE_BASE = 0;
	static const int TYPE_SCENE_TITLE = 1;
	static const int TYPE_SCENE_MAZE = 2;
};

class Scene {
public:
	Scene() {};
	virtual ~Scene() {};
	virtual void setSceneData(SceneData *data) {};
};

typedef enum  {
	SCREEN_TITLE = 0,
	SCREEN_MAZE,
} SceneType;


// The passed in data must be freed by the changed to scene.
void change_scene(SceneType scene, SceneData *data);


class MazeData : public SceneData {
public:
	static const char WALL = 'w';
	static const char PATH = 'p';
	static const char WALKED = 'x';
	static const char UNDEFINED = 'u';
	int columns;
	int rows;
	int level;
	char *maze;
	MazeData(int cols, int rows, int level) {
		this->columns = cols;
		this->rows = rows;
		this->level = level;
		this->maze = new char[columns*rows];
	};
	~MazeData() {
		delete this->maze;
	};
	int getType() {
		return SceneData::TYPE_SCENE_MAZE;
	}
	char get(int x, int y) {
		return this->maze[x + y*columns];
	}
	void set(int x, int y, char val) {
		this->maze[x + y*columns] = val;
	}
	void generate() {
		// Put walls all around it
		for (int x = 0; x < columns; ++x) {
			for (int y = 0; y < rows; ++y) {
				if (0 == y || 0 == x || rows-1 == y || columns-1 == x ||
					y % 2 == 0 || x % 2 == 0) {
					this->set(x, y, MazeData::WALL);
				} else {
					this->set(x, y, MazeData::UNDEFINED);
				}
			}
		}

		floodFill();
	}

	struct maze_cell {
		int x;
		int y;
	};


	void floodFill() {
		// minus 1 for the top wall; divide by 2 for every other
		// being a valid path; subtract a final 1 for zero-index.
		int path_rows = (this->rows - 1) / 2 - 1;
		int path_cols = (this->columns - 1) / 2 - 1;

		// Choose start location
		// Always pick cell on the left side.
		std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
		std::uniform_int_distribution<int> dist(0, path_rows);
		int start_cell_y = dist(generator);
		int starty = start_cell_y * 2 + 1;
		// std::cout << "starty: " << starty << "|rows: " << this->rows << "|path rows: " << path_rows << std::endl;

		this->set(0, starty, MazeData::PATH);
		this->set(1, starty, MazeData::PATH);

		// Choose an exit
		// Always pick cell on right side.
		int end_cell_y = dist(generator);
		int endy = end_cell_y * 2 + 1;
		this->set(this->columns - 1, endy, MazeData::PATH);

		std::vector<maze_cell> cells;
		std::vector<maze_cell> dirs;
		maze_cell c;
		cells.push_back({0, start_cell_y});

		clock_t start_time = clock();
		while (cells.size() > 0 && double(clock() - start_time) / double(CLOCKS_PER_SEC) < 5.0) {
			dirs.clear();

			c = cells.front();
			cells.erase(cells.begin());

			if (c.x - 1 >= 0 && this->get((c.x - 1) * 2 + 1, c.y * 2 + 1) == MazeData::UNDEFINED) {
				dirs.push_back({c.x - 1, c.y});
			}
			if (c.x + 1 <= path_cols && this->get((c.x + 1) * 2 + 1, c.y * 2 + 1) == MazeData::UNDEFINED) {
				dirs.push_back({c.x + 1, c.y});
			}
			if (c.y - 1 >= 0 && this->get(c.x * 2 + 1, (c.y - 1) * 2 + 1) == MazeData::UNDEFINED) {
				dirs.push_back({c.x, c.y - 1});
			}
			if (c.y + 1 <= path_rows && this->get(c.x * 2 + 1, (c.y + 1) * 2 + 1) == MazeData::UNDEFINED) {
				dirs.push_back({c.x, c.y + 1});
			}

			if (dirs.size() == 0) {
				continue;
			}
			std::uniform_int_distribution<int> dir_dist(0, dirs.size()-1);
			int picked = dir_dist(generator);
			maze_cell n_cell = dirs[picked];
			// std::cout << "cur cell: (" << c.x << ", " << c.y << ")" << std::endl;
			// std::cout << "new cell: (" << n_cell.x << ", " << n_cell.y << ")" << std::endl;
			dirs.erase(dirs.begin() + picked);

			int wall_x, wall_y;
			if (n_cell.x != c.x) {
				if (n_cell.x - c.x > 0) {
					// c.x → n_cell.x
					wall_x = c.x * 2 + 2;
					wall_y = c.y * 2 + 1;
				} else {
					// n_cell.x → c.x
					wall_x = c.x * 2;
					wall_y = c.y * 2 + 1;
				}
			} else {
				if (n_cell.y - c.y > 0) {
					//  c.y
					//   ↓
					// n_cell.y
					wall_x = c.x * 2 + 1;
					wall_y = c.y * 2 + 2;
				} else {
					// n_cell.y
					//   ↓
					//  c.y
					wall_x = c.x * 2 + 1;
					wall_y = c.y * 2;
				}
			}
			this->set(wall_x, wall_y, MazeData::PATH);
			this->set(n_cell.x * 2 + 1, n_cell.y * 2 + 1, MazeData::PATH);
			// std::cout << "broke wall: (" << wall_x << ", " << wall_y << ")" << std::endl;

			cells.push_back(c);
			cells.insert(cells.begin(), n_cell);
		}
	}
};
