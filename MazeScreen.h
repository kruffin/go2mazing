#include <cstring>
#include <string>
#include <go2/input.h>
#include "lib/dr_libs/dr_wav.h"
#include "lib/ugui/ugui.h"
#include "Scene.h"

class MazeScreen : public Scene {
public:
	MazeScreen(int screen_width, int screen_height);
	~MazeScreen();
	void update(double dt, double totalTime);
	void draw();
	bool load(std::string programPath);
	bool handleInput(go2_gamepad_state_t *gamepad);
	void setSceneData(SceneData *data);
private:
	int screen_height;
	int screen_width;
	MazeData *maze_data;
	std::string no_maze_str;
	short point_size;
	int offset_y;
	int max_cols;
	int max_rows;
};