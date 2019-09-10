#ifndef GEOMETRYPASS_H
#define GEOMETRYPASS_H
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "ObjLoader.h"
#include <vector>
#include "TextureLoader.h"
#include "Scene.h"

class GeometryPass
{
private:
	PS_CONSTANT_BUFFER PsConstData;
	GS_CONSTANT_BUFFER GsConstData;	
	
	ID3D11RenderTargetView* gRenderTargetView[NR_OF_TARGET_AND_RESOURCE_VIEWS];
	ID3D11DepthStencilView* gDepthStencilView;	
	
	Scene* scene;		

public:
	GeometryPass(ID3D11Device* inGDevice, ID3D11DeviceContext* inGDeviceContext,
		ID3D11RenderTargetView *inGRTV[], ID3D11DepthStencilView *inGDSV,
	    CameraClass *camera);	
	~GeometryPass();

	void initialize();

	void update(ID3D11Device* inDevice, ID3D11DeviceContext* inDeviceContext, float dt);
	void render(float inDt);
};


#endif
