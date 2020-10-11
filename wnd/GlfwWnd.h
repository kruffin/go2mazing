#pragma once
#ifdef USE_GLFW

#include <GLFW/glfw3.h>
#include <vector>

#include "BaseWnd.h"
#include "../audio.h"

class GlfwWnd : public BaseWnd {
public:
	GlfwWnd();
	~GlfwWnd();

	void init();
	void destroy();
	void blit(int x, int y, Sprite *s, int frame);
	void setPixel(int x, int y, unsigned char *color, size_t color_size);
	void swapBuffer();
	void getInput(BaseInput *input);
	void playSound(const short* data, int frames);
	void stopSounds();

	int getWidth();
	int getHeight();
	void frameBufferSizeChanged(GLFWwindow *window, int width, int height);

private:
	void initImageTexture();

	int windowHeight;
	int windowWidth;

	int height;
	int width;
	std::vector<GLubyte> imageData;
	GLuint imageTexture;
	GLenum imageFormat;
	GLenum imageType;

	GLFWwindow *window;
	GLuint vao;
	GLuint vbo;
	GLuint vertexShader;
	GLuint fragShader;
	GLuint program;

	GLint posLocation;
	GLint texLocation;

	static const char *vertex_shader_text;
	static const char *frag_shader_text;

	go2_audio_t* audio;
};


void frame_buffer_size_changed(GLFWwindow *window, int width, int height);
#endif