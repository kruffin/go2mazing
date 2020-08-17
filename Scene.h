#pragma once

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
	int columns;
	int rows;
	int level;
	char *maze;
	MazeData(int cols, int rows, int level) {
		this->columns = cols;
		this->rows = rows;
		this->level = level;
		this->maze = new char[columns*rows];
		// Put walls all around it
		for (int x = 0; x < columns; ++x) {
			for (int y = 0; y < rows; ++y) {
				if (0 == y || 0 == x || rows-1 == y || columns-1 == x ||
					y % 2 == 0 || x % 2 == 0) {
					this->set(x, y, MazeData::WALL);
				} else {
					this->set(x, y, MazeData::PATH);
				}
			}
		}
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
};
