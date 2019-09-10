#ifndef LIGHT_PASS_H
#define LIGHT_PASS_H
#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include "FullScreenQuad.h"
class LightPass
{
private:	
	ID3D11RenderTargetView* gRenderTargetView;
	ID3D11DepthStencilView* gDepthStencilView;	

	FullScreenQuad* FSQ;
	
public:
	LightPass(ID3D11Device* inGDevice, ID3D11DeviceContext* inGDeviceContext, 
		ID3D11RenderTargetView* inGRTV, ID3D11DepthStencilView* inGDSV,
		ID3D11ShaderResourceView* inGSRV[], CameraClass* camera);
	~LightPass();
	
	void update();
	void render();
};


#endif
