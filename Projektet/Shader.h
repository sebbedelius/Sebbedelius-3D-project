#ifndef SHADER_H
#define SHADER_H
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "GlobalVaribles.h"
using namespace DirectX;

class Shader
{
private:	
	ID3D11VertexShader * gVertexShader;
	ID3D11GeometryShader* gGeometryShader;
	ID3D11PixelShader* gPixelShader;
	ID3D11InputLayout* gVertexLayout;	

	ID3DBlob* pVS;
	ID3DBlob* pGS;
	ID3DBlob* pPS;

	bool usingGeometryShader;
	bool usingPixelShader = true;

	HRESULT createPositionShader(ID3D11Device* inDevice);
	HRESULT createPositionColorShader(ID3D11Device* inDevice);
	HRESULT createPositionTextureShader(ID3D11Device* inDevice);
	HRESULT createPositionTextureForWavesShader(ID3D11Device* inDevice);
	HRESULT createPositionColorForShadowShader(ID3D11Device* inDevice);
	HRESULT createPositionTextureForShadowShader(ID3D11Device* inDevice);
	HRESULT createPositionColorForDeferredShader(ID3D11Device* inDevice);
	HRESULT createPositionTextureForDeferredShader(ID3D11Device* inDevice);	
	HRESULT createPositionTextureForLightPassShader(ID3D11Device* inDevice);

	void createInputLayoutPositionTexture(ID3D11Device* inDevice);
	void createInputLayoutPositionColor(ID3D11Device* inDevice);	
	void createInputLayoutPosition(ID3D11Device* inDevice);	
	
public:
	Shader();
	~Shader();

	HRESULT createShader(ID3D11Device* inDevice, ShaderType shaderType);

	ID3D11VertexShader* getVertexShader() const;
	ID3D11GeometryShader* getGeometryShader() const;
	ID3D11PixelShader* getPixelShader() const;
	ID3D11InputLayout* getVertexLayout() const;	
};

#endif
