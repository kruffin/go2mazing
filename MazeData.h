#pragma once

#include <vector>
#include <deque>
#include "Scene.h"

class MazeData : public SceneData {
public:
	static const char WALL = 'w';
	static const char PATH = ' ';
	static const char WALKED = 'x';
	static const char REWALKED = 'r';
	static const char KEY_GREEN = 'g';
	static const char DOOR_GREEN = 'G';
	static const char UNDEFINED = 'u';
	int columns;
	int rows;
	int level;
	char *maze;
	int *complexity;
	MazeData(int cols, int rows, int level);
	~MazeData();
	int getType();
	char get(int x, int y);
	void set(int x, int y, char val);
	int getComplexity(int x, int y);
	void setComplexity(int x, int y, int val);

	struct maze_cell {
		int x;
		int y;
	};

	// Expects a two element array input.
	void generate(maze_cell *entrances);
	void floodFill(maze_cell *entrances);

	struct complexity_cell {
		int x;
		int y;
		int complexity;
		std::vector<complexity_cell *> children;
		//struct complexity_cell *children;
		//unsigned char childCount;
		struct complexity_cell *parent;
	};

	complexity_cell *graph;
	void freeGraph(MazeData::complexity_cell *graph);

	void calculateComplexity(int entrance_x, int entrance_y);
	void printComplexity();
	void printMaze();
	void printGraph(MazeData::complexity_cell *cur, int indent);

	std::deque<maze_cell> findPath(int x, int y);
	std::deque<maze_cell> findPath(int x, int y, MazeData::complexity_cell *cur);

	MazeData::complexity_cell * findGraphNode(int x, int y);

	bool placeDoorAndKey(std::deque<MazeData::maze_cell> path);
	int countNodes(MazeData::complexity_cell *toCompute, MazeData::complexity_cell *toIgnore);
};