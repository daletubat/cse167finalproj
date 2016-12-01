#include "Window.h"

// On some systems you need to change this to the absolute path
#define VERTEX_SHADER_PATH "shader.vert"
#define FRAGMENT_SHADER_PATH "shader.frag"

const char* window_title = "GLFW Starter Project";
OBJObject * harambe;
OBJObject * rifle;
GLint shaderProgram;
// Default camera parameters
glm::vec3 Window::cam_pos(0.0f, 0.0f, 20.0f);		// e  | Position of camera
glm::vec3 cam_look_at(0.0f, 0.0f, 0.0f);	// d  | This is where the camera looks at
glm::vec3 cam_up(0.0f, 1.0f, 0.0f);			// up | What orientation "up" is
int Window::width;
int Window::height;
glm::mat4 Window::P;
glm::mat4 Window::V;


glm::vec3 Window::lastPoint;
glm::vec3 Window::curr_pos;
bool Window::rotate = false;
bool Window::translate = false;

void Window::initialize_objects()
{
	harambe = new OBJObject("C:\\Users\\madhugrandhi\\Desktop\\Gorilla\\Gorilla.obj", "C:\\Users\\madhugrandhi\\Desktop\\Gorilla\\Gorilla_S.ppm");
	rifle = new OBJObject("C:\\Users\\madhugrandhi\\Desktop\\Henry_Rifle\\lowpoly.obj", "C:\\Users\\madhugrandhi\\Desktop\\Henry_Rifle\\texture.ppm");
	
	// Load the shader program. Make sure you have the correct filepath up top
	shaderProgram = LoadShaders(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);
}

// Treat this as a destructor function. Delete dynamically allocated memory here.
void Window::clean_up()
{
	delete(harambe);
	delete(rifle);
	glDeleteProgram(shaderProgram);
}

GLFWwindow* Window::create_window(int width, int height)
{
	// Initialize GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return NULL;
	}

	// 4x antialiasing
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__ // Because Apple hates comforming to standards
	// Ensure that minimum OpenGL version is 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Enable forward compatibility and allow a modern OpenGL context
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create the GLFW window
	GLFWwindow* window = glfwCreateWindow(width, height, window_title, NULL, NULL);

	// Check if the window could not be created
	if (!window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		fprintf(stderr, "Either GLFW is not installed or your graphics card does not support modern OpenGL.\n");
		glfwTerminate();
		return NULL;
	}

	// Make the context of the window
	glfwMakeContextCurrent(window);

	// Set swap interval to 1
	glfwSwapInterval(1);

	// Get the width and height of the framebuffer to properly resize the window
	glfwGetFramebufferSize(window, &width, &height);
	// Call the resize callback to make sure things get drawn immediately
	Window::resize_callback(window, width, height);

	return window;
}

void Window::resize_callback(GLFWwindow* window, int width, int height)
{
	Window::width = width;
	Window::height = height;
	// Set the viewport size. This is the only matrix that OpenGL maintains for us in modern OpenGL!
	glViewport(0, 0, width, height);

	if (height > 0)
	{
		P = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
		V = glm::lookAt(cam_pos, cam_look_at, cam_up);
	}
}

void Window::idle_callback()
{

}

void Window::display_callback(GLFWwindow* window)
{
	// Clear the and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use the shader of programID
	glUseProgram(shaderProgram) ;
	// Render the cube
	//rifle->draw(shaderProgram);
	harambe->draw(shaderProgram);

	// Gets events, including input such as keyboard and mouse or window resizing
	glfwPollEvents();
	// Swap buffers
	glfwSwapBuffers(window);
}

void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Check for a key press
	if (action == GLFW_PRESS)
	{
		// Check if escape was pressed
		if (key == GLFW_KEY_ESCAPE)
		{
			// Close the window. This causes the program to also terminate.
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		else if (mods == GLFW_MOD_SHIFT && key == GLFW_KEY_S) {
			harambe->scale_size = 2;
			harambe->scale();
			harambe->scale_size = 1;
			harambe->draw(shaderProgram);
		}
		else if (mods == GLFW_MOD_SHIFT && key == GLFW_KEY_R) {
			rifle->scale_size = 2;
			rifle->scale();
			rifle->scale_size = 1;
			rifle->draw(shaderProgram);
		}
		else if (key == GLFW_KEY_S) {
			harambe->scale_size = .5;
			harambe->scale();
			harambe->scale_size = 1;
			harambe->draw(shaderProgram);
		}
		else if (key == GLFW_KEY_R) {
			rifle->scale_size = .5;
			rifle->scale();
			rifle->scale_size = 1;
			rifle->draw(shaderProgram);
		}
		else if (key == GLFW_KEY_R) {
			glPointSize(1.0f);
			harambe->reset();
		}
	}
}

void Window::GLFWmousebuttonfun(GLFWwindow* window, int button, int action, int mods) {
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	glm::vec3 pos = { xpos, ypos, 0 };
	lastPoint = trackBallMapping(pos);

	if (action == GLFW_PRESS) {
		if (button == GLFW_MOUSE_BUTTON_RIGHT) {
			translate = true;
		}
		if (button == GLFW_MOUSE_BUTTON_LEFT) {
			rotate = true;
		}
	}
	else {
		translate = false;
		rotate = false;
	}
}

void Window::cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
	curr_pos = { xpos, ypos, 0 };
	glm::vec3 direction;
	GLfloat velocity;
	GLfloat rot_angle = 0;
	glm::vec3 rotAxis = { 0, 0, 0 };

	curr_pos = Window::trackBallMapping(curr_pos);

	if (rotate) {
		direction = curr_pos - lastPoint;
		velocity = glm::length(direction);

		if (velocity > 0.0001) {
			std::cout << lastPoint.z << " " << curr_pos.z << std::endl;
			rotAxis = glm::cross(lastPoint, curr_pos);
			rot_angle = velocity * 20.0f;
			rotAxis = glm::normalize(rotAxis);
			rifle->toWorld *= glm::rotate(glm::mat4(1.0f), (rot_angle * glm::pi<float>() / 180.0f), rotAxis);// *object->toWorld;
		}
		lastPoint = curr_pos;
	}
	else if (translate) {
		direction = curr_pos - lastPoint;
		direction /= 6;
		if (glm::length(direction) > 10.0f)
			return;
		rifle->toWorld = glm::translate(glm::mat4(1.0f), glm::vec3(direction)) * rifle->toWorld;
	}
	else
		return;
}

void Window::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	GLfloat pixel_diff;
	GLfloat zoom_factor;
	glm::vec3 point = { xoffset, yoffset, 0 };
	pixel_diff = (float) yoffset - curr_pos.y;
	zoom_factor = 1.0f + pixel_diff / 4.0f;
	glm::vec3 scale = glm::vec3(zoom_factor, zoom_factor, zoom_factor);
	harambe->toWorld = glm::scale(glm::mat4(1.0f), scale) * harambe->toWorld;
}

glm::vec3 Window::trackBallMapping(glm::vec3 point) {
	glm::vec3 pos;
	float d;
	pos.x = (2.0f*point.x - width) / width;
	pos.y = (height - 2.0f*point.y) / height;
	pos.z = 0.0f;
	d = glm::length(pos);
	d = (d < 1.0f) ? d : 1.0f;
	pos.z = sqrtf(1.001f - d*d);
	pos = glm::normalize(pos);
	return pos;
}