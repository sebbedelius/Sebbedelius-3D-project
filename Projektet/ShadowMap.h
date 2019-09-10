#ifndef SHADOWMAP_H
#define SHADOWMAP_H
#include <d3d11.h>
#include "DirectXMath.h"
using namespace DirectX;

class ShadowMap
{
private:
	UINT width, height;

	ID3D11ShaderResourceView* mDepthMapSRV;
	ID3D11DepthStencilView* mDepthMapDSV;
	ID3D11RasterizerState* gRasterizerState;
	ID3D11SamplerState* gSamplerState;

	D3D11_VIEWPORT mViewport;

	void setViewport();
	HRESULT buildShadowmap(ID3D11Device* inDevice);
	HRESULT createRasterizerState(ID3D11Device* inDevice);
	HRESULT createSamplerState(ID3D11Device* inDevice);
public:
	ShadowMap(ID3D11Device* inDevice, UINT inWidth, UINT inHeight);
	~ShadowMap();

	void setPipelineAndViewPort(ID3D11DeviceContext* inDeviceContext, ID3D11RenderTargetView* inRTV);

	ID3D11ShaderResourceView* getDepthMapSRV();
	ID3D11RasterizerState* getRasterizerState();
	ID3D11SamplerState* getSamplerState();
};

#endif 
