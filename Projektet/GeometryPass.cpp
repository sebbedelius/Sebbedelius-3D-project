#include "GeometryPass.h"

GeometryPass::GeometryPass(ID3D11Device* inGDevice, ID3D11DeviceContext* inGDeviceContext,
	ID3D11RenderTargetView *inGRTV[], ID3D11DepthStencilView *inGDSV, 
	CameraClass *camera)
{

	for (int i = 0; i < NR_OF_TARGET_AND_RESOURCE_VIEWS; i++)
	{
		this->gRenderTargetView[i] = inGRTV[i];
	}

	this->gDepthStencilView = inGDSV;		

	this->scene = new Scene(inGDevice,
		inGDeviceContext,
		camera, RenderType::DeferredRendering);	
}

GeometryPass::~GeometryPass()
{	
	delete this->scene;
}

void GeometryPass::initialize()
{
}

void GeometryPass::update(ID3D11Device* inDevice, ID3D11DeviceContext* inDeviceContext, float dt)
{
	this->scene->update(inDevice, inDeviceContext, dt);
}

void GeometryPass::render(float dt)
{
	this->scene->render(this->gRenderTargetView,
		4,
		this->gDepthStencilView,
		dt);
}
