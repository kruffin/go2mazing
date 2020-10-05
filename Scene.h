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
