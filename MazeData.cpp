#include <random>
#include <chrono>

#include <iostream>
#include <ostream>

#include "MazeData.h"


MazeData::MazeData(int cols, int rows, int level) {
	this->columns = cols;
	this->rows = rows;
	this->level = level;
	this->maze = new char[columns*rows];
	this->complexity = new int[columns*rows];
	this->graph = NULL;
};

MazeData::~MazeData() {
	delete this->maze;
	delete this->complexity;
	freeGraph(this->graph);
	this->graph = NULL;
};

void MazeData::freeGraph(MazeData::complexity_cell *graph) {
	if (NULL != graph) {
		std::deque<complexity_cell *> todel;
		std::vector<complexity_cell *> tovisit;

		tovisit.push_back(graph);
		while (tovisit.size() > 0) {
			complexity_cell *cur = tovisit.front();
			tovisit.erase(tovisit.begin());

			todel.push_front(cur);

			for (int i = 0; i < cur->children.size(); ++i) {
				tovisit.push_back(cur->children[i]);
			}
		}

		for (int i = 0; i < todel.size(); ++i) {
			delete todel[i];
		}
	}
};


int MazeData::getType() {
	return SceneData::TYPE_SCENE_MAZE;
};
char MazeData::get(int x, int y) {
	return this->maze[x + y*columns];
};
void MazeData::set(int x, int y, char val) {
	this->maze[x + y*columns] = val;
};

int MazeData::getComplexity(int x, int y) {
	return this->complexity[x + y*columns];
};
void MazeData::setComplexity(int x, int y, int val) {
	this->complexity[x + y*columns] = val;
};

void MazeData::replaceAllTiles(char toReplace, char replaceWith) {
	for (int idx = 0; idx < rows*columns; ++idx) {
		if (this->maze[idx] == toReplace) {
			this->maze[idx] = replaceWith;
		}
	}
};

// Expects a two element array input.
void MazeData::generate(maze_cell *entrances) {
	bool finished = false;

	while (!finished) {
		freeGraph(this->graph);
		this->graph = NULL;
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

		for (int i = 0; i < columns*rows; ++i) {
			this->complexity[i] = -1;
		}

		floodFill(entrances);
		// Times 2 here since I halved the rows/columns when marking entrances.
		// Plus 1 to y since the top row wasn't used when calculating entrances.
		calculateComplexity(entrances[0].x * 2, entrances[0].y * 2 + 1);
		//printComplexity();
		
		//printGraph(this->graph, 0);
		std::deque<maze_cell> path = findPath(entrances[1].x * 2 + 2, entrances[1].y * 2 + 1);
		std::cout << "Path: ";
		for (int i = 0; i < path.size(); ++i) {
			std::cout << "->(" << path[i].x << "," << path[i].y << ")";
		}
		std::cout << std::endl;

		finished = placeDoorAndKey(path);
		printMaze();
	}
};

void MazeData::floodFill(maze_cell *entrances) {
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
	entrances[0] = {0, start_cell_y};
	entrances[1] = {path_cols, end_cell_y};
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
};

void MazeData::calculateComplexity(int entrance_x, int entrance_y) {
	std::vector<complexity_cell *> cells;
	complexity_cell *c = new complexity_cell{entrance_x, entrance_y, 0, std::vector<complexity_cell*>(), NULL};
	this->graph = c;

	cells.push_back(c);
	this->setComplexity(entrance_x, entrance_y, 0);
	clock_t start_time = clock();
	std::cout << "start cell: (" << entrance_x << ", " << entrance_y << ")" << std::endl;

	while (cells.size() > 0 && double(clock() - start_time) / double(CLOCKS_PER_SEC) < 5.0) {
		c = cells.front();
		cells.erase(cells.begin());

		int x = c->x - 1;
		int y = c->y;
		int comp;
		char tile;
		if (x >= 0) {
			comp = this->getComplexity(x, y);
			tile = this->get(x, y);
			if ((-1 == comp || comp > c->complexity + 1) && tile == MazeData::PATH) {
				complexity_cell *new_cell = new complexity_cell{x, y, c->complexity + 1, std::vector<complexity_cell*>(), c};
				c->children.push_back(new_cell);
				cells.push_back(new_cell);
				this->setComplexity(x, y, c->complexity + 1);
			}
		}

		x = c->x + 1;
		y = c->y;
		if (x < this->columns) {
			comp = this->getComplexity(x, y);
			tile = this->get(x, y);
			if ((-1 == comp || comp > c->complexity + 1) && tile == MazeData::PATH) {
				complexity_cell *new_cell = new complexity_cell{x, y, c->complexity + 1, std::vector<complexity_cell*>(), c};
				c->children.push_back(new_cell);
				cells.push_back(new_cell);
				this->setComplexity(x, y, c->complexity + 1);
			}
		}

		x = c->x;
		y = c->y - 1;
		if (y >= 0) {
			comp = this->getComplexity(x, y);
			tile = this->get(x, y);
			if ((-1 == comp || comp > c->complexity + 1) && tile == MazeData::PATH) {
				complexity_cell *new_cell = new complexity_cell{x, y, c->complexity + 1, std::vector<complexity_cell*>(), c};
				c->children.push_back(new_cell);
				cells.push_back(new_cell);
				this->setComplexity(x, y, c->complexity + 1);
			}
		}

		x = c->x;
		y = c->y + 1;
		if (y < this->rows) {
			comp = this->getComplexity(x, y);
			tile = this->get(x, y);
			if ((-1 == comp || comp > c->complexity + 1) && tile == MazeData::PATH) {
				complexity_cell *new_cell = new complexity_cell{x, y, c->complexity + 1, std::vector<complexity_cell*>(), c};
				c->children.push_back(new_cell);
				cells.push_back(new_cell);
				this->setComplexity(x, y, c->complexity + 1);
			}
		}
	}
};

void MazeData::printComplexity() {
	for (int y = 0; y < this->rows; ++y) {
		for (int x = 0; x < this->columns; ++x) {
			std::cout << this->getComplexity(x, y) << ", ";
		}
		std::cout << std::endl;
	}
};

void MazeData::printMaze() {
	for (int y = 0; y < this->rows; ++y) {
		for (int x = 0; x < this->columns; ++x) {
			std::cout << this->get(x, y);
		}
		std::cout << std::endl;
	}
};

void MazeData::printGraph(MazeData::complexity_cell *cur, int indent) {
	for (int i = 0; i < indent; ++i) {
		std::cout << '\t';
	}
	std::cout << "(" << cur->x << ", " << cur->y << ")" << std::endl;

	for (int c = 0; c < cur->children.size(); ++c) {
		printGraph(cur->children[c], indent + 1);
	}
};

std::deque<MazeData::maze_cell> MazeData::findPath(int x, int y) {
	return this->findPath(x, y, this->graph);
};

std::deque<MazeData::maze_cell> MazeData::findPath(int x, int y, MazeData::complexity_cell *cur) {
	if (cur->x == x && cur->y == y) {
		// std::cout << "found cell (" << x << ", " << y << ")" << std::endl;
		std::deque<MazeData::maze_cell> path;
		path.push_front({cur->x, cur->y});
		return path;
	}

	for (int i = 0; i < cur->children.size(); ++i) {
		std::deque<MazeData::maze_cell> path = findPath(x, y, cur->children[i]);
		if (path.size() != 0) {
			path.push_front({cur->x, cur->y});
			return path;
		}
	}

	return std::deque<MazeData::maze_cell>();
};

bool MazeData::placeDoorAndKey(std::deque<MazeData::maze_cell> path) {

	// Find the spot in the middle to add the door.
	maze_cell door_pos = path[path.size() / 2 - 1];
	this->set(door_pos.x, door_pos.y, MazeData::DOOR_GREEN);

	// Walk backwards and find a higher complexity path
	maze_cell search_from = path[path.size() / 2 - 2];
	MazeData::complexity_cell *door_node = this->findGraphNode(door_pos.x, door_pos.y);
	MazeData::complexity_cell *node = this->findGraphNode(search_from.x, search_from.y);
	MazeData::complexity_cell *old_node = NULL;

	std::vector<MazeData::complexity_cell *>junctions;

	if (node == NULL) {
		std::cerr << "Failed to find node (" << search_from.x << ", " << search_from.y << ")." << std::endl;
		return false;
	}

	while (NULL != node) {
		if (node->children.size() > 1) {
			// go to the end
			for (int i = 0; i < node->children.size(); ++i) {
				if (old_node != node->children[i] && door_node != node->children[i]) {
					junctions.push_back(node->children[i]);
					//this->set(node->children[i]->x, node->children[i]->y, MazeData::KEY_GREEN);
				}
			}
		}

		old_node = node;
		node = node->parent;
	}

	int largest = -1;
	MazeData::complexity_cell *longestJunction = NULL;
	for (int i = 0; i < junctions.size(); ++i) {
		int dist = countNodes(junctions[i], NULL);
		if (dist > largest) {
			longestJunction = junctions[i];
			largest = dist;
		}
	}

	if (NULL == longestJunction) {
		// There were no junctions after the door...
		// Return false here and just generate another maze.
		std::cout << "Failed to find any junctions." << std::endl;
		return false;
	}

	// Breadth first until last node; this should be the longest path
	std::vector<MazeData::complexity_cell *> tovisit;
	complexity_cell *cur = NULL;
	tovisit.push_back(longestJunction);
	while(tovisit.size() > 0) {
		cur = tovisit.front();
		tovisit.erase(tovisit.begin());

		for (int i = 0; i < cur->children.size(); ++i) {
			tovisit.push_back(cur->children[i]);
		}
	}

	// cur has the last node
	this->set(cur->x, cur->y, MazeData::KEY_GREEN);
	return true;
};

// Finds the graph node at this position.
MazeData::complexity_cell * MazeData::findGraphNode(int x, int y) {

	std::vector<complexity_cell *> tovisit;
	tovisit.push_back(this->graph);

	while (tovisit.size() > 0) {
		complexity_cell *cur = tovisit.front();
		tovisit.erase(tovisit.begin());

		if (x == cur->x && y == cur->y) {
			return cur;
		}

		for (int i = 0; i < cur->children.size(); ++i) {
			tovisit.push_back(cur->children[i]);
		}
	}

	return NULL;
};

int MazeData::countNodes(MazeData::complexity_cell *toCount, MazeData::complexity_cell *toIgnore) {
	int count = 0;
	std::vector<complexity_cell *> tovisit;
	tovisit.push_back(toCount);

	while (tovisit.size() > 0) {
		complexity_cell *cur = tovisit.front();
		tovisit.erase(tovisit.begin());

		++count;

		for (int i = 0; i < cur->children.size(); ++i) {
			if (cur->children[i] != toIgnore) {
				tovisit.push_back(cur->children[i]);
			}
		}
	}

	return count;
};