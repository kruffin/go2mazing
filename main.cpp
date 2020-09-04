#include <drm/drm_fourcc.h>

#include <cstring>
#include <iostream>
#include <filesystem>
namespace fs = std::filesystem::__cxx11;

#define DR_WAV_IMPLEMENTATION
#include "lib/dr_libs/dr_wav.h"
#include "lib/ugui/ugui.h"
#include "Scene.h"
#include "Drawing.h"
#include "TitleScreen.h"
#include "MazeScreen.h"
#include "Sound.h"
#include "wnd/Go2LibWnd.h"

void initUgui();
void destroyUgui();
bool drawScreen();
void updateLogic();
void drawErrorScreen(char msg[]);

Go2LibWnd window = Go2LibWnd();
BaseInput outGamepadState;

// ugui stuff
UG_GUI gui;

bool dirty_display = true;

// For handling button inputs
bool btn_pressed = false;
clock_t last_press;
clock_t current_press;
clock_t held_since;
clock_t idle_since;

clock_t run_time;

// Scene management
SceneType current_screen = SCREEN_TITLE;
TitleScreen *title_screen;
MazeScreen * maze_screen;


clock_t last_draw_time;
clock_t last_logic_time;


int main(int argc, char * argv[]) {
	fs::path progPath = fs::path(argv[0]);
	progPath.remove_filename();


	window.init();
	// initGo2();
	initUgui();
	title_screen->load(progPath);
	maze_screen->load(progPath);
	last_press = clock();
	idle_since = clock();
	last_draw_time = clock();
	last_logic_time = clock();
	run_time = clock();

	while(1) {
		window.getInput(&outGamepadState);

		if (outGamepadState.f1) {
			std::cout << "f1";
			window.destroy();
			destroyUgui();
			return 0;
		}
		current_press = clock();
		double diff = double(current_press - last_press) / double(CLOCKS_PER_SEC);
		if (diff > 0.15) {

			switch (current_screen) {
				case SCREEN_TITLE:
					btn_pressed = title_screen->handleInput(&outGamepadState);
					break;
				case SCREEN_MAZE:
					btn_pressed = maze_screen->handleInput(&outGamepadState);
					break;
				default:
					break;
			}
		}

		if (btn_pressed) {
			last_press = current_press;
			idle_since = current_press;
			btn_pressed = false;
		}

		updateLogic(); // Sets display dirty inside this function.
		if (dirty_display) {
			window.swapBuffer();
			dirty_display = false;
		}

	}
};


void play_sound(const short* data, int frames) {
	window.playSound(data, frames);
};

void go2SetPixel(UG_S16 x, UG_S16 y, UG_COLOR c) {
	window.setPixel(x, y, (unsigned char *)&c, sizeof(c));
};

void updateLogic() {
	clock_t current = clock();
	double draw_dt = double(current - last_draw_time) / double(CLOCKS_PER_SEC);
	double logic_dt = double(current - last_logic_time) / double(CLOCKS_PER_SEC);
	double totalTime = double(current - run_time) / double(CLOCKS_PER_SEC);

	last_logic_time = current;
	switch (current_screen) {
		case SCREEN_TITLE:
			title_screen->update(logic_dt, totalTime);
			break;
		case SCREEN_MAZE:
			maze_screen->update(logic_dt, totalTime);
			break;
		default:
			break;
	}

	if (draw_dt >= 1.0/30.0) {
		// Keep a certain fps (30 in this case).
		last_draw_time = current;
		drawScreen();
		dirty_display = true;
	}
	
};

bool drawScreen() {
	switch (current_screen) {
		case SCREEN_TITLE:
			title_screen->draw();
			return true;
		case SCREEN_MAZE:
			maze_screen->draw();
			return true;
		default:
			char err[] = "Bad current screen.";
			drawErrorScreen(err);
			return false;
	}
};

void drawErrorScreen(char msg[]) {
	UG_FontSelect(&FONT_8X14);
	UG_FillScreen(C_DARK_GOLDEN_ROD);
	UG_SetForecolor(C_CRIMSON);
	UG_SetBackcolor(C_DARK_GOLDEN_ROD);
	UG_PutString(20, 20, msg);
};

void initUgui() {
	// std::cout << "screen width: " << width << ", height: " << height << std::endl;
	// swap dimensions so ui surface is correct for rotated screen
	UG_Init(&gui, go2SetPixel, window.getHeight(), window.getWidth());
	title_screen = new TitleScreen(window.getHeight(), window.getWidth());
	maze_screen = new MazeScreen(window.getHeight(), window.getWidth());
};

void destroyUgui() {
	delete title_screen;
	delete maze_screen;
};

Scene *get_current_screen() {
	switch(current_screen) {
		case SCREEN_TITLE:
			return title_screen;
		case SCREEN_MAZE:
			return maze_screen;
		default:
			return (Scene *)NULL;
	}
};

void change_scene(SceneType scene, SceneData *data) {
	// kill audio
	window.stopSounds();

	// set the scene
	current_screen = scene;
	get_current_screen()->setSceneData(data);
	dirty_display = true;
};

void KR_blit(int x, int y, Sprite *s, int frame) {
	window.blit(x, y, s, frame);
};