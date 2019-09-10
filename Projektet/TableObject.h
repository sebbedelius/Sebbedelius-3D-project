#ifndef TABLEOBJECT_H
#define TABLEOBJECT_H
#include "Shader.h"
#include "CameraClass.h"
#include "StaticCamera.h"
#include "ShadowMap.h"
#include "Sun.h"
#include "ShadowMap.h"
#include <vector>
#include "directxcollision.h"
#include "ObjLoader.h"
#include "TextureLoader.h"
using namespace DirectX;

class TableObject
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
	BoundingBox mOBB;
	int id;
	bool draw;

	D3D11_MAPPED_SUBRESOURCE dataPtr;	
	PS_CONSTANT_BUFFER psConstData;
	SHADOW_GS_CONSTANT_BUFFER* sConstData;
	SHADOW_VS_CONSTANT_BUFFER* vssConstData;
	GS_CONSTANT_BUFFER* gDbgConstData;
	CAMERA_BUFFER* gCamData;

	std::vector<TriangleVertexTex> triangleVertices;	
	std::vector<Vertex> vertex;
	std::vector<Vertex> boundingBoxVertices;

	Shader* shader;	
	Shader* shadowShader;
	Shader* debugShader;

	ID3D11Buffer* gShadowConstantBuffer;	
	ID3D11Buffer* gVSShadowConstantBuffer;
	ID3D11Buffer* gPSConstantBuffer;
	ID3D11Buffer* gCameraBuffer;
	ID3D11Buffer* gBoundingVertexBuffer;
	ID3D11Buffer* gDbgConstantBuffer;
	ID3D11Buffer* gVertexBuffer;
	ID3D11ShaderResourceView** textureRV;
	ID3D11SamplerState** samplerStates;	
	ID3D11Device* gDevice;
	ID3D11DeviceContext* gDeviceContext;

	int nrOfVertices;

	int createTriangleData();
	void createMatrices();
	void createMatricesForStaticCamera();
	void updateMatrices();
	void updateMatricesForStaticCamera();

	HRESULT createConstantBuffers();	
	HRESULT createSamplerStates();		

	void setPipelineShadowPass();
	void setPipeline(ID3D11RenderTargetView* inRTV, ID3D11DepthStencilView* inDSV);
	void setPipeline(ID3D11RenderTargetView* inRTV[], ID3D11DepthStencilView* inDSV);
	void setPipelineDebug(ID3D11RenderTargetView* inRTV, ID3D11DepthStencilView* inDSV);
	void clearPipeline(ShaderType shaderType);	
		
public:	
	TableObject(ID3D11Device* inDevice, ID3D11DeviceContext* inDeviceContext,
		CameraClass* inCamera, StaticCamera* inStaticCamera, Sun* inSun, ShadowMap* inShadowMap, ShaderType shaderType);
	~TableObject();

	void createBoundingBox();

	XMMATRIX* getWorld();
	XMMATRIX* getWorldViewProj();
	XMMATRIX* getSunWorldViewProj();
	int getId() const;
	BoundingBox getBoundingBox() const;
	bool getDraw() const;

	void setId(int id);
	void setDraw(bool value);

	void update();

	void renderShadowPass();
	void render(ID3D11RenderTargetView* inRTV, ID3D11DepthStencilView* inDSV,
		float inDt, ShaderType shadertype);
	void render(ID3D11RenderTargetView* inRTV[], ID3D11DepthStencilView* inDSV,
		float inDt, ShaderType shadertype);

	
};

#endif