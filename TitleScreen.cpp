#include <iostream>

#include "TitleScreen.h"
#include "Sound.h"
#include "Drawing.h"

TitleScreen::TitleScreen(int screen_width, int screen_height) {
	this->x = 20.0;
	this->y = 20.0;
	this->speed = 40.0;
	this->screen_width = screen_width;
	this->screen_height = screen_height;
	this->title = std::string("Go a Mazing!!!");
	this->v_x = 0.5;
	this->v_y = 0.5;
	this->press_text = std::string("press any key to start");
	this->press_text_cycles = new UG_COLOR[5] {
		C_GREEN, C_GREEN_YELLOW, C_LIME_GREEN, C_YELLOW_GREEN, C_OLIVE 
	};
	this->press_text_color = this->press_text_cycles[0];

	this->author_text = std::string("a kruffin production");
	this->author_text_color = C_GRAY;
	this->play_time = -1;

	this->version = std::string("v0.2.0");
	this->prill = Player();
	this->cake = Goal();

	this->prill.world_x = 60.0;
	this->prill.world_y = 5.0;

	this->cake.world_x = 70.0;
	this->cake.world_y = 5.0;
}

TitleScreen::~TitleScreen() {
	delete this->press_text_cycles;
	if (this->background_audio != NULL) {
		free(this->background_audio);
	}
}

bool TitleScreen::load(std::string programPath) {
	drwav wav;
	if (!drwav_init_file(&wav, (programPath + "sounds/dootdoot.wav").c_str(), NULL)) {
		std::cout << "Failed to load sound." << std::endl;
		return false;
	} else {
		std::cout << "Channels: " << wav.channels << std::endl;
		this->background_audio = (drwav_int16*)malloc(wav.totalPCMFrameCount * wav.channels * sizeof(drwav_int16));
		this->background_samples = drwav_read_pcm_frames_s16(&wav, wav.totalPCMFrameCount, this->background_audio);
		this->background_frame_count = wav.totalPCMFrameCount;
		std::cout << "Samples: " << this->background_samples << " Frames: " << this->background_frame_count << std::endl;
		drwav_uninit(&wav);
	}

	if (!this->prill.load(programPath + "images/prill.png", 4, 4, 0)) {
		std::cout << "Failed to load player." << std::endl;
		return false;
	}
	if (!this->cake.load(programPath + "images/cake.png", 4, 1, 0)) {
		std::cout << "Failed to load cake." << std::endl;
		return false;
	}

	return true;
}

void TitleScreen::update(double dt, double totalTime) {
	this->x += dt * this->speed * this->v_x;
	this->y += dt * this->speed * this->v_y;
	if (this->x > this->screen_width - 24*this->title.length()) {
		this->v_x *= -1.0;
	}
	if (this->y > this->screen_height - 36) {
		this->v_y *= -1.0;
	}
	if (this->x < 0.0) {
		this->v_x *= -1.0;
		this->x = 0.0;
	}
	if (this->y < 0.0) {
		this->v_y *= -1.0;
		this->y = 0.0;
	}
	this->press_text_color = this->press_text_cycles[int(totalTime) % 5];
	if (-1 == this->play_time || double(clock() - this->play_time) / double(CLOCKS_PER_SEC) > 10.0) {//if (int(totalTime) % 10 == 0) {
		std::cout << "playing" << std::endl;
		play_sound(const_cast<short *>(this->background_audio), int(this->background_frame_count));
		this->play_time = clock();
	}

	this->prill.update(dt, totalTime);
	this->cake.update(dt, totalTime);
}

void TitleScreen::draw() {
	UG_FontSelect(&FONT_22X36);
	UG_FillScreen(C_PALE_GOLDEN_ROD);

	UG_SetForecolor(C_BLACK);
	UG_SetBackcolor(C_PALE_GOLDEN_ROD);

	//std::cout << "x: " << g_x << " y: " << g_y << std::endl;
	UG_PutString(int(this->x), int(this->y), const_cast<char*>(this->title.c_str()));

	UG_FillRoundFrame(60, this->screen_height - 110,
					  this->screen_width - 60, this->screen_height - 70,
					  5, C_DARK_ORCHID);
	UG_FontSelect(&FONT_8X14);
	UG_SetForecolor(this->press_text_color);
	UG_SetBackcolor(C_DARK_ORCHID);
	UG_PutString(this->screen_width/2 - 9*this->press_text.length()/2, 
				 this->screen_height - 97, 
				 const_cast<char*>(this->press_text.c_str()));

	UG_FontSelect(&FONT_8X8);
	UG_SetForecolor(this->author_text_color);
	UG_SetBackcolor(C_INDIGO);
	UG_FillRoundFrame(this->screen_width - 9*this->author_text.length() - 1, this->screen_height - 11,
					  this->screen_width-1, this->screen_height-1,
					  5, C_INDIGO);
	UG_PutString(this->screen_width - 9*this->author_text.length(), 
				 this->screen_height - 10, 
				 const_cast<char*>(this->author_text.c_str()));

	UG_FillRoundFrame(0, 0,
					  9 * this->version.length(), 10,
					  2, C_INDIGO);
	UG_PutString(1, 1, const_cast<char*>(this->version.c_str()));

	this->prill.draw();
	this->cake.draw();
	// KR_blit(20, 20, &this->prill, 0);
}

bool TitleScreen::handleInput(go2_gamepad_state_t *gamepad) {
	if (gamepad->dpad.left || gamepad->dpad.right ||
		gamepad->dpad.up || gamepad->dpad.down ||
		gamepad->buttons.a || gamepad->buttons.b ||
		gamepad->buttons.x || gamepad->buttons.y ||
		gamepad->buttons.top_left || gamepad->buttons.top_right) {

		MazeData *data = new MazeData(11,11,1);
		change_scene(SCREEN_MAZE, data);
		return true;
	}
	return false;
}

void TitleScreen::setSceneData(SceneData *data) {
	if (data != NULL) {
		delete data;
	}
}