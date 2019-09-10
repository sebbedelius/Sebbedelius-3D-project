#ifndef OBJLOADER_H
#define OBJLOADER_H
#include <string>
#include <d3d11.h>
#include <DirectXMath.h>
#include "GlobalVaribles.h"
using namespace std;

class ObjLoader
{
private:

	int nrOfVertices;
	int nrOfTextureCoords;
	int nrOfNormalCoords;
	int nrOfIndices;	
	int capacity;
	int capacityForIndices;
	Vertex * vertex;
	Texture * texture;
	Normal * normal;
	int * vertexIndices, *textureIndices, *normalIndices;

	void expand();
	void expandIndicesArr();

public:
	ObjLoader(int cap = 2000);
	~ObjLoader();
	bool loadObj(string filename);
	void getVertices(Vertex passedVertexArr[]);
	void getTextures(Texture passedTextureArr[]);
	void getNormals(Normal passedNormalArr[]);
	int getNrOfVertices();
	int getNrOfIndices();
};



#endif
