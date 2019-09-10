#ifndef TERRAIN_H
#define TERRAIN_H
#include <d3d11.h>
#include "Shader.h"
#include "CameraClass.h"
#include "Sun.h"
#include "ShadowMap.h"
#include "StaticCamera.h"
#include <vector>
#include "directxcollision.h"
using namespace DirectX;

enum OutOfBouns { North = -1, West = -2, East = -3, South = -4 };

//With help from the book "Introduction to 3D programming with DirectX 11" by Franc Luna
class Terrain
{
private:
	XMMATRIX world;
	XMMATRIX worldViewProj;
	XMMATRIX scaling;
	XMMATRIX sunWorldViewProj;
	
	CameraClass* camera;	
	StaticCamera* staticCamera;
	Sun* mSun;
	ShadowMap* mShadowMap;
	BoundingBox mBB;
	int id;
	bool draw;

	Shader* shader;
	Shader* shadowShader;

	std::vector<TriangleVertexColor> triangleVertices;
	std::vector<unsigned int> indices;
	std::vector<Vertex> vertex;

	std::vector<float> heights;	
	int terrainWidth, terrainHeight;
	float gridWidth, gridDepth;
	float scale;
	float cellSpaceX, cellSpaceZ;

	D3D11_MAPPED_SUBRESOURCE dataPtr;	
	PS_CONSTANT_BUFFER psConstData;
	SHADOW_GS_CONSTANT_BUFFER* sConstData;
	SHADOW_VS_CONSTANT_BUFFER* vssConstData;
	GS_CONSTANT_BUFFER* gDbgConstData;
	
	ID3D11Buffer* gShadowConstantBuffer;
	ID3D11Buffer* gVSShadowConstantBuffer;	
	ID3D11Buffer* gPSConstantBuffer;
	ID3D11Buffer* gVertexBuffer;
	ID3D11Buffer* gIndexBuffer;	
	ID3D11Buffer* gDbgConstantBuffer;

	ID3D11SamplerState* gSamplerState;	
	ID3D11Device* gDevice;
	ID3D11DeviceContext* gDeviceContext;

	int nrOfVertices;
	int nrOfIndices;
	int nrOfFaces;

	void createGrid(float width, float depth);
	void loadHeightmap();	
	void createVertexAndIndexBuffers();	
	void smooth();
	float average(int i, int j);
	bool inBounds(int i, int j);
	void colorTerrain();

	void createMatrices();
	void createMatricesForStaticCamera();
	void updateMatrices();
	void updateMatricesForStaticCamera();

	HRESULT createConstantBuffers();		
	HRESULT createSamplerState();	

	void setPipelineShadowPass();
	void setPipeline(ID3D11RenderTargetView* inRTV, ID3D11DepthStencilView* inDSV);
	void setPipeline(ID3D11RenderTargetView* inRTV[], ID3D11DepthStencilView* inDSV);	
	void clearPipeline();
public:
	Terrain(ID3D11Device* inDevice, ID3D11DeviceContext* inDeviceContext,
		CameraClass* inCamera, StaticCamera* inStaticCamera, Sun* inSun, ShadowMap* inShadowMap, ShaderType shaderType);
	~Terrain();
	
	XMMATRIX getWorld() const;
	XMMATRIX getWorldViewProj() const;
	XMMATRIX getSunWorldViewProj() const;

	int getId() const;
	float getHeight(float x, float z) const;
	BoundingBox getBoundingBox() const;
	int getTerrainWidth() const;
	int getTerrainHeight() const;
	float getGridWidth() const;
	float getGridDepth() const;	
	bool getDraw() const;

	void setPipelineDebug();
	void setId(int id);
	void setBoundingBox(BoundingBox obb);
	void setDraw(bool value);
	
	void update();
	void renderShadowPass();
	void render(ID3D11RenderTargetView* inRTV, ID3D11DepthStencilView* inDSV);
	void render(ID3D11RenderTargetView* inRTV[], ID3D11DepthStencilView* inDSV);
	void dbgRender();
};

#endif
