#include "LightPass.h"



LightPass::LightPass(ID3D11Device* inGDevice, ID3D11DeviceContext* inGDeviceContext,
	ID3D11RenderTargetView* inGRTV, ID3D11DepthStencilView* inGDSV,
	ID3D11ShaderResourceView* inGSRV[], CameraClass* camera)
{	
	this->gRenderTargetView = inGRTV;
	this->gDepthStencilView = inGDSV;			

	this->FSQ = new FullScreenQuad(inGDevice, inGDeviceContext,
		inGSRV, camera);
}

LightPass::~LightPass()
{	
	delete this->FSQ;
}

void LightPass::update()
{
	this->FSQ->update();
}

void LightPass::render()
{
	this->FSQ->render(this->gRenderTargetView,
		this->gDepthStencilView,
		ShaderType::PositionTextureForLightPass);
}


