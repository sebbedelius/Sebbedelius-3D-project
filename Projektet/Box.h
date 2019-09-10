#ifndef BOX_H
#define BOX_H
#include "Shader.h"
#include "CameraClass.h"
#include "TextureLoader.h"
#include "Sun.h"
#include "ShadowMap.h"
#include "StaticCamera.h"
#include <vector>
#include <DirectXCollision.h>
class Box
{
private:
	XMMATRIX world;
	XMMATRIX worldViewProj;	
	XMMATRIX sunWorldViewProj;
	XMFLOAT3 mPostion;

	ShaderType shaderType;
	int id;
	bool draw;

	std::vector<Vertex> vertex;

	D3D11_MAPPED_SUBRESOURCE dataPtr;
	GS_CONSTANT_BUFFER* gConstData;
	SHADOW_GS_CONSTANT_BUFFER* sConstData;
	SHADOW_VS_CONSTANT_BUFFER* vssConstData;
	CAMERA_BUFFER* gCamData;

	ID3D11Buffer* gShadowConstantBuffer;
	ID3D11Buffer* gVSShadowConstantBuffer;
	ID3D11Buffer* gVertexBuffer;	
	ID3D11Buffer* gCameraBuffer;
	ID3D11ShaderResourceView* textureRV;
	ID3D11SamplerState* gSamplerState;
	ID3D11BlendState* gBlendState;	
	ID3D11Device* gDevice;
	ID3D11DeviceContext* gDeviceContext;

	CameraClass* mCamera;
	StaticCamera* mStaticCamera;
	Sun* mSun;
	ShadowMap* mShadowMap;
	Shader* shader;
	Shader* shadowShader;
	BoundingBox mOBB;

	int nrOfVertices;

	HRESULT createBox();
	HRESULT createConstantBuffers();
	HRESULT createSamplerState();
	HRESULT createBlendState();

	void createMatrices();
	void createMatricesForStaticCamera();
	void updateMatrices();	
	void updateMatricesForStaticCamera();

	void setPipelineShadowPass();
	void setPipeline(ID3D11RenderTargetView* inRTV, ID3D11DepthStencilView* inDSV);
	void clearPipeline();

public:
	Box(ID3D11Device* inDevice, ID3D11DeviceContext* inDeviceContext,
		CameraClass* inCamera, StaticCamera* inStaticCamera, Sun* inSun, ShadowMap* inShadowMap, ShaderType shaderType);
	~Box();

	void createBoundingBox();

	int getId() const;
	BoundingBox getBoundingBox() const;
	bool getDraw() const;

	void setId(int id);
	void setPosition(XMFLOAT3 pos);
	void setDraw(bool value);

	void update();

	void renderShadowPass();
	void render(ID3D11RenderTargetView* inRTV, ID3D11DepthStencilView* inDSV);

};
#endif