#ifndef FULLSCREENQUAD_H
#define FULLSCREENQUAD_H
#include "Shader.h"
#include "CameraClass.h"

class FullScreenQuad
{
private:
	
	XMMATRIX scaling, world, worldViewProj;
	XMVECTOR camPos = XMVectorSet(0.0f, 0.0f, -1.0f, 1.0f);
	XMVECTOR lookAt = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
	float fov = DirectX::XM_PI * 0.45f;
	float aspectRatio = 1280.0f / 720.0f;
	float nearPlane = 0.1f;
	float farPlane = 20.0f;

	D3D11_MAPPED_SUBRESOURCE dataPtr;
	GS_CONSTANT_BUFFER VSLConstData;
	PSL_LIGHT_BUFFER PSLConstData;
	CAMERA_BUFFER* PSLCameraData;
	ID3D11Buffer* gVertexBuffer;
	ID3D11Buffer* gVSLConstantBuffer;
	ID3D11Buffer* gPSLConstantBuffer;
	ID3D11Buffer* gPSLCameraBuffer;
	Shader* shader;	
	ID3D11Device* gDevice;
	ID3D11DeviceContext* gDeviceContext;
	ID3D11ShaderResourceView* gShaderResourceView[NR_OF_TARGET_AND_RESOURCE_VIEWS];
	ID3D11SamplerState* gSamplerState;
	CameraClass* camera;

	int nrOfVertices;

	int createTriangleData();
	void createMatrices();
	void createLights();
	HRESULT createConstantBuffers();
	HRESULT createSamplerState();

	void setPipeline(ID3D11RenderTargetView* inSRV, ID3D11DepthStencilView* inDSV);
	void clearPipeline();
public:
	FullScreenQuad(ID3D11Device* inDevice, ID3D11DeviceContext* inDeviceContext,
		ID3D11ShaderResourceView* inSRV[], CameraClass* camera);
	~FullScreenQuad();

	void update();
	void render(ID3D11RenderTargetView* inRTV, ID3D11DepthStencilView* inDSV,
		 ShaderType shaderType);
};

#endif
