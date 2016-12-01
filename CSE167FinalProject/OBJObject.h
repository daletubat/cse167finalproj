#ifndef OBJOBJECT_H
#define OBJOBJECT_H
#define _CRT_SECURE_NO_WARNINGS 

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#include <OpenGL/gl.h> // Remove this line in future projects
#else
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>

class OBJObject
{
public:
	static int imageWidth;
	static int imageHeight;

	std::vector<glm::vec3> listVertices;
	std::vector<glm::vec3> listNormals;
	std::vector<glm::vec3> listTextures;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> textures;
	std::vector<unsigned int> faceVertices;
	std::vector<unsigned int> faceTexels;
	std::vector<unsigned int> faceNormals;
	std::vector<unsigned int> listMaterial;

	GLuint VBO, VAO, NBO, TBO;
	GLuint uProjection, uModel, uView, uViewPos;
	GLuint texture;

	glm::mat4 toWorld;
	float scale_size;

	OBJObject();
	OBJObject(const char *filepath, const char *texture);
	~OBJObject();

	unsigned char* loadPPM(const char* filename, int& width, int& height);
	void loadTexture(const char* filename);
	void parse(const char* filepath);
	void draw(GLuint shaderProgram);
	void scale();
	void reset();
};

#endif