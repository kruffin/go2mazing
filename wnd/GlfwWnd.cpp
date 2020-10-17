#ifdef USE_GLFW

#include <iostream>
#include <ostream>
#include <cstring>

#include "../lib/glad/glad.h"
#include "GlfwWnd.h"
#include "../Sound.h"

const char *GlfwWnd::vertex_shader_text = 
	"#version 130\n"
	"precision highp float;\n"
	"in vec2 pos;\n"
	"in vec2 intexcoord;\n"
	"out vec2 texcoord;\n"
	"void main() {\n"
	"	gl_Position = vec4(pos, 0.0, 1.0);\n"
	"	texcoord = intexcoord;\n"
	"	// rotate the texture coords.\n"
	"	//texcoord.x = pos.y * 0.5 + 0.5; //change to [0,1]\n"
	"	//texcoord.y = pos.x * 0.5 + 0.5;\n"
	"}\n";

const char *GlfwWnd::frag_shader_text = 
	"#version 130\n"
	"precision highp float;\n"
	"in vec2 texcoord;\n"
	"uniform sampler2D tex;\n"
	"out vec4 fragColor;\n"
	"void main() {\n"
	"	fragColor = texture(tex, texcoord);//vec4(0.9, 0.5, 0.5, 1.0);\n"
	"}\n";

GlfwWnd::GlfwWnd() {

};

GlfwWnd::~GlfwWnd() {
	this->destroy();
};

void GlfwWnd::initImageTexture() {
	this->imageFormat = GL_RGB;
	this->imageType = GL_UNSIGNED_SHORT_5_6_5;

	// Times 2 because a RGB565 uses 2 bytes (16 bits).
	this->imageData = std::vector<GLubyte>(this->width*this->height*2);

	glGenTextures(1, &this->imageTexture);
	glBindTexture(GL_TEXTURE_2D, this->imageTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->width, this->height, 0, this->imageFormat, this->imageType, &this->imageData[0]);
	glBindTexture(GL_TEXTURE_2D, 0);
};

void GlfwWnd::init() {
	this->width = 320;
	this->height = 480;

	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW." << std::endl;
		return;
	}

	this->audio = go2_audio_create(44100); // 44.1 kHz

	this->window = glfwCreateWindow(this->height, this->width, "Go2Mazing", NULL, NULL);
	glfwMakeContextCurrent(this->window);
	gladLoadGL();
	glfwSetFramebufferSizeCallback(this->window, frame_buffer_size_changed);
	glfwGetFramebufferSize(this->window, &this->windowWidth, &this->windowHeight);
	glViewport(0, 0, this->windowWidth, this->windowHeight);

	this->vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(this->vertexShader, 1, &GlfwWnd::vertex_shader_text, NULL);
	glCompileShader(this->vertexShader);

	this->fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(this->fragShader, 1, &GlfwWnd::frag_shader_text, NULL);
	glCompileShader(this->fragShader);

	this->program = glCreateProgram();
	glAttachShader(this->program, this->vertexShader);
	glAttachShader(this->program, this->fragShader);
	glLinkProgram(this->program);
	GLint isLinked = GL_FALSE;
	glGetProgramiv(this->program, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE) {
		GLsizei logSize = 0;
		glGetProgramiv(this->program, GL_INFO_LOG_LENGTH, &logSize);
		char log[logSize];
		glGetProgramInfoLog(this->program, logSize, &logSize, &log[0]);
		std::cerr << "Failed to compile shader program." << std::endl << log;
	}

	float quadPoints[] = {
		// -1.0f, 1.0f,
		// 1.0f, 1.0f,
		// -1.0f, -1.0f,
		// 1.0f, -1.0f

		-1.0f,	1.0f,	1.0f,	1.0f, 
		-1.0f,	-1.0f,	0.0f,	1.0f,
		1.0f,	1.0f,	1.0f,	0.0f,
		1.0f,	-1.0f,	0.0f,	0.0f
	};
	glGenVertexArrays(1, &this->vao);
	glBindVertexArray(this->vao);
	glGenBuffers(1, &this->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadPoints), quadPoints, GL_STATIC_DRAW);

	this->posLocation = glGetAttribLocation(this->program, "pos");
	glEnableVertexAttribArray(this->posLocation);
	glVertexAttribPointer(this->posLocation, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void *)0);

	this->texLocation = glGetAttribLocation(this->program, "intexcoord");
	glEnableVertexAttribArray(this->texLocation);
	glVertexAttribPointer(this->texLocation, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void *)(2*sizeof(float)));

	glBindVertexArray(0);

	this->initImageTexture();
};	

void GlfwWnd::destroy() {
	if (NULL != this->window) {
		glfwDestroyWindow(this->window);
		this->window = NULL;
	}
	glfwTerminate();
	go2_audio_destroy(this->audio);
};

void GlfwWnd::frameBufferSizeChanged(GLFWwindow *window, int width, int height) {
	if (window == this->window) {
		this->windowWidth = width;
		this->windowHeight = height;
		glViewport(0, 0, this->windowWidth, this->windowHeight);
	}
};

void GlfwWnd::blit(int x, int y, Sprite *s, int frame) {
	uint8_t* dst = static_cast<uint8_t*>(&this->imageData[0]);
	int bytes_per_pixel = 2;
	int stride = bytes_per_pixel * this->width;
	// std::cout << "bpp: " << bits_per_pixel << "|stride: " << stride;
	// std::cout << "sprite at (" << x << ", " << y << ")" << std::endl;
	// std::flush(std::cout);
	// s->blit(frame, x, y, dst, stride, this->width, this->height, bits_per_pixel);

	y = y + s->frameHeight;
	x = x + s->frameWidth;

	for (int sy = 0; sy < s->frameHeight; ++sy) {
		for (int sx = 0; sx < s->frameWidth; ++sx) {
			Sprite::Pixel cur = s->getPixel(frame, sx, sy);
			if (cur.alpha == 0) { continue; }
			// Takes a picture like this
			// [c][d][e]
			// [ ][ ][ ]
			// [ ][ ][*]
			// then rotates it since the go2 screen is long ways; keep this in since this flipping is needed
			// somewhere in the common code. TODO: investigate so this gets simpler.
			// [ ][ ][c]
			// [ ][ ][d]
			// [*][ ][e]
			// and flips it since onpengl textures start at the bottom-left
			// [*][ ][e]
			// [ ][ ][d]
			// [ ][ ][c]
			
			int fy = (this->height-1 -x+ (s->frameWidth -sx));
			int fx = (this->width-1 -y+ (s->frameHeight -sy));
			if (fy < 0 || fx < 0 || fy >= this->height || fx >= this->width) {
				continue;
			}
			memcpy(dst + (fy * stride + fx * bytes_per_pixel), (unsigned char*)&cur.color, sizeof(cur.color));
		}
	}
};

void GlfwWnd::setPixel(int x, int y, unsigned char *color, size_t color_size) {
	uint8_t* dst = static_cast<uint8_t*>(&this->imageData[0]);
	int bytes_per_pixel = 2;
	int stride = bytes_per_pixel * this->width;

	int yfinal = this->height - 1 - x;
	int xfinal = this->width - 1 - y;
	memcpy(dst + (yfinal * stride + xfinal * bytes_per_pixel), color, color_size);
};

void GlfwWnd::swapBuffer() {
	glUseProgram(this->program);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->imageTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 
		this->width, this->height, 0, this->imageFormat, 
		this->imageType, static_cast<const void*>(&this->imageData[0]));

	glBindVertexArray(this->vao);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
	glfwSwapBuffers(this->window);
	//glClear(GL_COLOR_BUFFER_BIT);
};

void GlfwWnd::getInput(BaseInput *input) {
	glfwPollEvents();
	input->f1 = glfwWindowShouldClose(this->window);

	input->left = glfwGetKey(this->window, GLFW_KEY_LEFT) == GLFW_PRESS;
	input->right = glfwGetKey(this->window, GLFW_KEY_RIGHT) == GLFW_PRESS;
	input->up = glfwGetKey(this->window, GLFW_KEY_UP) == GLFW_PRESS;
	input->down = glfwGetKey(this->window, GLFW_KEY_DOWN) == GLFW_PRESS;

	input->lt1 = glfwGetKey(this->window, GLFW_KEY_L) == GLFW_PRESS;
	input->rt1 = glfwGetKey(this->window, GLFW_KEY_R) == GLFW_PRESS;
};

void GlfwWnd::playSound(const short* data, int frames) {
	// std::cout << "submitting sound with " << frames << " frames." << std::endl;
	go2_audio_submit_fix(this->audio, data, frames);
};

void GlfwWnd::stopSounds() {
	if (NULL != this->audio) {
		go2_audio_destroy(this->audio);
	}
	this->audio = go2_audio_create(44100); // 44.1 kHz
};

int GlfwWnd::getWidth() {
	return this->width;
};

int GlfwWnd::getHeight() {
	return this->height;
};

#endif