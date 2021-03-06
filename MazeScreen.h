#include <cstring>
#include <string>
#include <random>
#include "lib/dr_libs/dr_wav.h"
#include "lib/ugui/ugui.h"
#include "wnd/BaseWnd.h"
#include "Scene.h"
#include "Player.h"
#include "Goal.h"
#include "Key.h"
#include "MazeData.h"

class MazeScreen : public Scene {
public:
	MazeScreen(int screen_width, int screen_height);
	~MazeScreen();
	void update(double dt, double totalTime);
	void draw();
	bool load(std::string programPath);
	bool handleInput(BaseInput *gamepad);
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
	Player player;
	Goal goal;
	Sprite walls;
	Sprite paths;
	Goal exit;
	Goal tree;
	Goal entrance;
	Key green_key;
	Goal green_door;

	drwav_int16 *bonk_audio;
	size_t bonk_samples;
	drwav_uint64 bonk_frame_count;

	double maze_complete;
	std::default_random_engine complete_generator;
	std::uniform_int_distribution<int> complete_dist;

	short begin_x;
	short begin_y;

	void movePLayer(int new_x, int new_y, int direction);
	MazeData::maze_cell rideRoad(int start_x, int start_y, int end_x, int end_y);
	MazeData::complexity_cell * nextOnRoad(MazeData::complexity_cell *target, MazeData::complexity_cell *excluded);
};