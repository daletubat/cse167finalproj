#include "OBJObject.h"
#include "Window.h"

using namespace std;

int OBJObject::imageHeight = 0;
int OBJObject::imageWidth = 0;

OBJObject::OBJObject()
{
}

OBJObject::OBJObject(const char *filepath, const char *texture)
{
	scale_size = 1;
	toWorld = glm::mat4(1.0f);
	parse(filepath);
	loadTexture(texture);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &NBO);
	glGenBuffers(1, &TBO);

	glBindVertexArray(VAO); //Bind Container

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, NBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), &normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, TBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * textures.size(), &textures[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0); //Unbind Buffer
	glBindVertexArray(0); //Unbind Container
}

OBJObject::~OBJObject() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &NBO);
	glDeleteBuffers(1, &TBO);
}

/** Load a ppm file from disk.
@input filename The location of the PPM file.  If the file is not found, an error message
will be printed and this function will return 0
@input width This will be modified to contain the width of the loaded image, or 0 if file not found
@input height This willbe modified to contain the height of the loaded image, or 0 if file not found
@return Returns the RGB pixel data as interleaved unsigned chars (R0 G0 B0 R1 G1 B1 R2 G2 B2 .... etc) or 0 if an error ocured
**/
unsigned char* OBJObject::loadPPM(const char* filename, int& width, int& height)
{
	const int BUFSIZE = 128;
	FILE* fp;
	unsigned int read;
	unsigned char* rawData;
	char buf[3][BUFSIZE];
	char* retval_fgets;
	size_t retval_sscanf;

	if ((fp = fopen(filename, "rb")) == NULL)
	{
		//std::cerr << "error reading ppm file, could not locate " << filename << std::endl;
		width = 0;
		height = 0;
		return NULL;
	}

	// Read magic number:
	retval_fgets = fgets(buf[0], BUFSIZE, fp);

	// Read width and height:
	do
	{
		retval_fgets = fgets(buf[0], BUFSIZE, fp);
	} while (buf[0][0] == '#');
	retval_sscanf = sscanf(buf[0], "%s %s", buf[1], buf[2]);
	width = atoi(buf[1]);
	height = atoi(buf[2]);

	// Read maxval:
	do
	{
		retval_fgets = fgets(buf[0], BUFSIZE, fp);
	} while (buf[0][0] == '#');

	// Read image data:
	rawData = new unsigned char[width * height * 3];
	read = fread(rawData, width * height * 3, 1, fp);
	fclose(fp);
	if (read != 1)
	{
		std::cerr << "error parsing ppm file, incomplete data" << std::endl;
		delete[] rawData;
		width = 0;
		height = 0;
		return NULL;
	}

	return rawData;
}

// load image file into texture object
void OBJObject::loadTexture(const char * filename)
{
	unsigned char* tdata;  // texture pixel data

	// Create ID for texture
	glGenTextures(1, &texture);

	// Set this texture to be the one we are working with
	glBindTexture(GL_TEXTURE_2D, texture);

	// Use bilinear interpolation:
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Load image file
	tdata = loadPPM(filename, imageWidth, imageHeight);
	if (tdata == NULL)
		return;
	// Generate the texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, tdata);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void OBJObject::parse(const char *filepath)
{
	GLfloat x = 0, y = 0, z = 0;  // vertex coordinates
	GLfloat xn = 0, yn = 0, zn = 0; // vertex normals
	GLfloat xt = 0, yt = 0, zt = 0; //texture coordinates
	GLuint xf1 = 0, xf2 = 0, xf3 = 0, yf1 = 0, yf2 = 0, yf3 = 0, zf1 = 0, zf2 = 0, zf3 = 0; // faces
	GLint c1 = 0, c2 = 0;    // characters read from file
	char read[10000];

	FILE* fp = fopen(filepath, "rb");  // make the file name configurable so you can load other files
	if (fp == NULL) {
		exit(-1);
	}  // just in case the file can't be found or is corrupt


	while (fgets(read, sizeof(read), fp)) {
		if (feof(fp)) {
			break;
		}

		if ((read[0] == 'v') && (read[1] == ' ')) {
			sscanf_s(read, "v %f %f %f", &x, &y, &z);
			listVertices.push_back(glm::vec3(x, y, z));
		}
		else if ((read[0] == 'v') && (read[1] == 'n')) {
			sscanf_s(read, "vn %f %f %f", &xn, &yn, &zn);
			listNormals.push_back(glm::vec3(xn, yn, zn));
		}
		else if ((read[0] == 'v') && (read[1] == 't')) {
			sscanf_s(read, "vt %f %f %f", &xt, &yt, &zt);
			listTextures.push_back(glm::vec3(xt, yt, zt));
		}
		else if ((read[0] == 'f')) {
			sscanf_s(read, "f %u/%u/%u %u/%u/%u %u/%u/%u", &xf1, &xf2, &xf3, &yf1, &yf2, &yf3, &zf1, &zf2, &zf3);
			faceVertices.push_back(xf1 - 1);
			faceVertices.push_back(yf1 - 1);
			faceVertices.push_back(zf1 - 1);
			
			faceTexels.push_back(xf2 - 1);
			faceTexels.push_back(yf2 - 1);
			faceTexels.push_back(zf2 - 1);

			faceNormals.push_back(xf3 - 1);
			faceNormals.push_back(yf3 - 1);
			faceNormals.push_back(zf3 - 1);
		}
		//std::cout << xf1 << " " << xf2 << " " << xf3 << std::endl;
	}

	for (unsigned int i = 0; i < faceVertices.size(); i++) {
		vertices.push_back(listVertices[faceVertices[i]]);
		normals.push_back(listNormals[faceNormals[i]]);
		textures.push_back(listTextures[faceTexels[i]]);
	}
	
	/*vertices.clear();
	normals.clear();
	textures.clear();

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
	if (!scene) {
		std::cout << "didn't work crap" << std::endl;
		exit(-1);
	}
	const int totalSize = sizeof(aiVector3D) * 2 + sizeof(aiVector2D);
	int totalVert = 0;

	for (int i = 0; i < scene->mNumMeshes; i++) {
		aiMesh * mesh = scene->mMeshes[i];
		int faces = mesh->mNumFaces;
		listMaterial.push_back(mesh->mMaterialIndex);	
	}*/

	fclose(fp); 
}

void OBJObject::draw(GLuint shaderProgram)
{
	glm::mat4 model = toWorld;
	glm::mat4 view = Window::V;
	glm::mat4 modelview = Window::V * toWorld;
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	
	uProjection = glGetUniformLocation(shaderProgram, "projection");
	uModel = glGetUniformLocation(shaderProgram, "model");
	uView = glGetUniformLocation(shaderProgram, "view");
	uViewPos = glGetUniformLocation(shaderProgram, "viewPos");

	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
	glUniformMatrix4fv(uModel, 1, GL_FALSE, &model[0][0]);
	glUniformMatrix4fv(uView, 1, GL_FALSE, &view[0][0]);
	glUniform3f(uViewPos, Window::cam_pos.x, Window::cam_pos.y, Window::cam_pos.z);

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(shaderProgram, "tex"), 0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}




void OBJObject::scale() {
	this->toWorld = glm::scale(this->toWorld, glm::vec3(scale_size, scale_size, scale_size));
}

void OBJObject::reset() {
	this->toWorld = glm::mat4(1.0f);
}