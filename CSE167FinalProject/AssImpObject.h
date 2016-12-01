#pragma comment(lib, "assimp.lib")

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class AssImpObject
{
public:
	AssImpObject();
	~AssImpObject();

	bool LoadModelFromFile(const char* filename);
};

