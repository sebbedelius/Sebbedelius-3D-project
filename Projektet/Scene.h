#ifndef SCENE_H
#define SCENE_H
#include "TableObject.h"
#include "Terrain.h"
#include "CameraClass.h"
#include "Box.h"
#include "BoxNode.h"
#include "StaticCamera.h"
#include "Waves.h"

enum RenderType {ForwardRendering, DeferredRendering};

class Scene
{
private:
	std::chrono::high_resolution_clock::time_point start;

	XMMATRIX waterTexTransform;

	TableObject* table;
	Terrain* terrain;
	Box* box;
	Box* box2;
	
	RenderType renderType;
	
	CameraClass* mCamera;
	StaticCamera* mStaticCamera;
	BoxNode* rootNode;

	Sun* mSun;
	ShadowMap* mShadowMap;
	Waves* mWaves;

	Shader* shader;
	Shader* debugShader;
	
	int idCount;
	int nodesCount;	
	int nrOfChildren;
	int nrOfWavesVertices;
	int nrOfWavesIndices;
	float waterTexOffsetX = 0.0f;
	float waterTexOffsetY = 0.0f;

	int currentLevel;

	std::vector<Vertex> boundingBoxVertices;
	std::vector<TriangleVertexTexXM> wavesVertices;

	SHADOW_GS_CONSTANT_BUFFER* gWavesGSConstData;
	VS_WAVES_BUFFER* gWavesVSConstData;
	CAMERA_BUFFER* gCameraData;

	ID3D11Buffer* gBoundingVertexBuffer;	
	ID3D11Buffer* gWavesVertexBuffer;
	ID3D11Buffer* gWavesIndexBuffer;
	ID3D11Buffer* gWavesGSConstBuffer;
	ID3D11Buffer* gWavesVSConstBuffer;
	ID3D11Buffer* gCameraBuffer;

	ID3D11ShaderResourceView* waterTex;

	ID3D11RasterizerState* rsState;	
	ID3D11BlendState* gBlendState;

	D3D11_MAPPED_SUBRESOURCE dataPtr;	

	void checkKeys(float dt);

	BoundingBox createBoundingBox(float widthLeft, float widthRight, float depthClose, float depthFar);
	void createChildren(BoxNode* parent, float middleWidth, float middleDepth, float parentWidth, float parentDepth);	
	void sortGeometry(BoxNode* rootNode, int objectId);

	void clearQuadTree(BoxNode* node);
	void createFrustumAndTest();
	void testFrustumAgainstTree(BoundingFrustum bf, BoxNode* parent);
	void checkPolygonsInNode(BoundingFrustum bf, BoxNode* node);

	void createCameraBuffer(ID3D11Device* inDevice);
	void createWavesConstantBuffer(ID3D11Device* inDevice);	
	void updateWavesMatrices(ID3D11DeviceContext* inDeviceContext);
	void updateWavesMatricesForStaticCamera(ID3D11DeviceContext* inDeviceContext);
	void createWavesTriangleData(ID3D11Device* inDevice);
	void updateWaves(ID3D11DeviceContext* inDeviceContext, float dt);

	void createRasterizerState(ID3D11Device* inDevice);
	void createBlendState(ID3D11Device* inDevice);

	void setPipelineWaves(ID3D11DeviceContext* inDeviceContext,
		ID3D11RenderTargetView * inRTV, ID3D11DepthStencilView * inDSV);
	void setPipelineDebug(ID3D11DeviceContext* inDeviceContext,
		ID3D11RenderTargetView * inRTV, ID3D11DepthStencilView * inDSV);
	void clearPipeline(ID3D11DeviceContext* inDeviceContext);
public:
	Scene(ID3D11Device* inDevice, ID3D11DeviceContext* inDeviceContext,
		CameraClass* inCamera, RenderType renderType);
	~Scene();

	void buildQuadTree(ID3D11Device* inDevice);

	void update(ID3D11Device* inDevice, ID3D11DeviceContext* inDeviceContext, float dt);
	void render(ID3D11DeviceContext* inDeviceContext, ID3D11RenderTargetView* inRTV, ID3D11DepthStencilView* inDSV,
		float inDt);
	void render(ID3D11RenderTargetView* inRTV[], int nrOfRenderTargets, ID3D11DepthStencilView* inDSV,
		float inDt);
};


#endif