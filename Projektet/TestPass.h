#ifndef TESTPASS_H
#define TESTPASS_H
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "ObjLoader.h"
#include <vector>
#include <wrl.h>
#include "TextureLoader.h"
#include "Shader.h"
#include "Scene.h"
using namespace Microsoft::WRL;

class TestPass
{
private:
	GS_CONSTANT_BUFFER GsConstData;
	
	ID3D11DepthStencilView* gDepthStencilView;	
	ID3D11RenderTargetView* gRenderTargetView;	

	Scene* scene;		

public:
	TestPass(ID3D11Device* inGDevice, ID3D11DeviceContext* inGDeviceContext,
		ID3D11RenderTargetView* inGRTV ,ID3D11DepthStencilView *inGDSV,
		CameraClass *camera);
	~TestPass();

	void update(ID3D11Device* inDevice, ID3D11DeviceContext* inDeviceContext, float dt);
	void render(ID3D11DeviceContext* inDeviceContext, float dt);
};


#endif
