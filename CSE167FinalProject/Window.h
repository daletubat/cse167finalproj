#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <iostream>

#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
#include "Shader.h"
#include "OBJObject.h"

class Window
{
public:
	static int width;
	static int height;
	static glm::vec3 lastPoint;
	static glm::vec3 curr_pos;
	static glm::vec3 scale;
	static glm::vec3 cam_pos;
	static glm::mat4 P; // P for projection
	static glm::mat4 V; // V for view
	static glm::mat4 transform;
	static bool rotate;
	static bool translate;
	static void initialize_objects();
	static void clean_up();
	static GLFWwindow* create_window(int width, int height);
	static void resize_callback(GLFWwindow* window, int width, int height);
	static void idle_callback();
	static void display_callback(GLFWwindow*);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void GLFWmousebuttonfun(GLFWwindow* window, int key, int scancode, int mods);
	static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static glm::vec3 trackBallMapping(glm::vec3 point);
};

#endif
