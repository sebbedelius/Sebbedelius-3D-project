#include "TestPass.h"

TestPass::TestPass(ID3D11Device * inGDevice, ID3D11DeviceContext * inGDeviceContext,
	ID3D11RenderTargetView * inGRTV, ID3D11DepthStencilView * inGDSV,
	CameraClass * camera)
{		
	this->gRenderTargetView = inGRTV;	
	this->gDepthStencilView = inGDSV;		

	this->scene = new Scene(inGDevice,
		inGDeviceContext,
		camera, RenderType::ForwardRendering);	
}

TestPass::~TestPass()
{
	delete this->scene;
}

void TestPass::update(ID3D11Device* inDevice, ID3D11DeviceContext* inDeviceContext, float dt)
{
	this->scene->update(inDevice, inDeviceContext, dt);
}

void TestPass::render(ID3D11DeviceContext* inDevice, float dt)
{	
	this->scene->render(inDevice, this->gRenderTargetView, 
		this->gDepthStencilView,
		dt);
}
