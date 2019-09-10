#include "ShadowMap.h"

void ShadowMap::setViewport()
{
	this->mViewport.TopLeftX = 0.0f;
	this->mViewport.TopLeftY = 0.0f;
	this->mViewport.Height = static_cast<float>(this->height);
	this->mViewport.Width = static_cast<float>(this->width);
	this->mViewport.MinDepth = 0.0f;
	this->mViewport.MaxDepth = 1.0f;
}

HRESULT ShadowMap::buildShadowmap(ID3D11Device* inDevice)
{
	ID3D11Texture2D* depthMap = NULL;
	D3D11_TEXTURE2D_DESC tDesc;
	ZeroMemory(&depthMap, sizeof(depthMap));
	tDesc.Width = this->width;
	tDesc.Height = this->height;
	tDesc.ArraySize = 1;
	tDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	tDesc.CPUAccessFlags = 0;
	tDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	tDesc.MipLevels = 1;
	tDesc.MiscFlags = 0;
	tDesc.SampleDesc.Count = 1;
	tDesc.SampleDesc.Quality = 0;
	tDesc.Usage = D3D11_USAGE_DEFAULT;
	
	HRESULT hr = inDevice->CreateTexture2D(&tDesc, 0, &depthMap);
	if (FAILED(hr))
	{
		return hr;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = 0;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	
	hr = inDevice->CreateDepthStencilView(depthMap, &dsvDesc, &mDepthMapDSV);
	if (FAILED(hr))
	{
		return hr;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = tDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;

	hr = inDevice->CreateShaderResourceView(depthMap, &srvDesc, &mDepthMapSRV);
	if (FAILED(hr))
	{
		return hr;
	}

	depthMap->Release();
	return S_OK;
}

HRESULT ShadowMap::createRasterizerState(ID3D11Device* inDevice)
{
	HRESULT hr = 0;

	D3D11_RASTERIZER_DESC rDesc;
	memset(&rDesc, 0, sizeof(rDesc));
	rDesc.FillMode = D3D11_FILL_SOLID;
	rDesc.CullMode = D3D11_CULL_BACK;
	rDesc.FrontCounterClockwise = false;
	rDesc.DepthClipEnable = false;
	rDesc.DepthBias = 100000;
	rDesc.DepthBiasClamp = 0.0f;
	rDesc.SlopeScaledDepthBias = 1.0f;

	hr = inDevice->CreateRasterizerState(&rDesc, &this->gRasterizerState);
	if (FAILED(hr))
	{
		return hr;
	}

	return S_OK;
}

HRESULT ShadowMap::createSamplerState(ID3D11Device * inDevice)
{
	HRESULT hr = 0;

	D3D11_SAMPLER_DESC samShadow;
	memset(&samShadow, 0, sizeof(samShadow));
	samShadow.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samShadow.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samShadow.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samShadow.BorderColor[0] = 0.0f;
	samShadow.BorderColor[1] = 0.0f;
	samShadow.BorderColor[2] = 0.0f;
	samShadow.BorderColor[3] = 0.0f;
	samShadow.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	samShadow.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	samShadow.MaxAnisotropy = 4;
	samShadow.MipLODBias = 0;

	hr = inDevice->CreateSamplerState(&samShadow, &this->gSamplerState);

	return S_OK;
}

ShadowMap::ShadowMap(ID3D11Device * inDevice, UINT inWidth, UINT inHeight)
{
	this->width = inWidth;
	this->height = inHeight;

	this->setViewport();
	this->buildShadowmap(inDevice);
	this->createRasterizerState(inDevice);
	this->createSamplerState(inDevice);
}

ShadowMap::~ShadowMap()
{
	this->mDepthMapDSV->Release();
	this->gRasterizerState->Release();
	this->mDepthMapSRV->Release();
	this->gSamplerState->Release();
}

void ShadowMap::setPipelineAndViewPort(ID3D11DeviceContext * inDeviceContext, ID3D11RenderTargetView* inRTV)
{
	inDeviceContext->RSSetViewports(1, &mViewport);

	//Null render target because we only want to draw to the depth buffer
	ID3D11RenderTargetView* nullRTV[1] = { nullptr };
	inDeviceContext->OMSetRenderTargets(1, &inRTV, mDepthMapDSV);

	inDeviceContext->ClearDepthStencilView(mDepthMapDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

ID3D11ShaderResourceView * ShadowMap::getDepthMapSRV()
{
	return this->mDepthMapSRV;
}

ID3D11RasterizerState * ShadowMap::getRasterizerState()
{
	return this->gRasterizerState;
}

ID3D11SamplerState * ShadowMap::getSamplerState()
{
	return this->gSamplerState;
}
