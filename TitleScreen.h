#pragma once
#include <cstring>
#include <string>
#include "lib/dr_libs/dr_wav.h"
#include "lib/ugui/ugui.h"
#include "wnd/BaseWnd.h"
#include "Scene.h"
#include "Player.h"
#include "Goal.h"

class TitleScreen : public Scene {
public:
	TitleScreen(int screen_width, int screen_height);
	~TitleScreen();
	void update(double dt, double totalTime);
	void draw();
	bool load(std::string programPath);
	bool handleInput(BaseInput *gamepad);
	void setSceneData(SceneData *data);
private:
	double x;
	double y;
	double v_x;
	double v_y;
	double speed;
	int screen_width;
	int screen_height;
	std::string title;
	std::string press_text;
	UG_COLOR press_text_color;
	UG_COLOR *press_text_cycles;
	std::string author_text;
	UG_COLOR author_text_color;
	drwav_int16 *background_audio;
	size_t background_samples;
	drwav_uint64 background_frame_count;
	clock_t play_time;
	std::string version;
	Player prill;
	Goal cake;
};