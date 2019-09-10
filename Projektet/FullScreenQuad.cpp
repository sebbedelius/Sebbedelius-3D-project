#include "FullScreenQuad.h"

int FullScreenQuad::createTriangleData()
{
	struct TriangleVertexPos
	{
		float x, y, z;
		float u, v;
	};

	int screenQuadVertexSize = 6;
	TriangleVertexPos triangleScreenQuadVertices[6] =
	{
		-1.0f, 1.0f, 0.0f, 0.0f, 0.0f,	//v0 pos		

		1.0f, -1.0f, 0.0f, 1.0f, 1.0f,	//v1		

		-1.0f, -1.0f, 0.0f, 0.0f, 1.0f,//v2		

		1.0f, 1.0f, 0.0f, 1.0f, 0.0f, //v3		

		1.0f, -1.0f, 0.0f, 1.0f, 1.0f, //v1

		-1.0f, 1.0f, 0.0f, 0.0f, 0.0f //v0
	};

	// Describe the Vertex Buffer
	D3D11_BUFFER_DESC lightBufferDesc;
	memset(&lightBufferDesc, 0, sizeof(lightBufferDesc));
	// what type of buffer will this be?
	lightBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	// what type of usage (press F1, read the docs)
	lightBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	// how big in bytes each element in the buffer is.
	lightBufferDesc.ByteWidth = sizeof(triangleScreenQuadVertices);
	lightBufferDesc.CPUAccessFlags = 0;
	lightBufferDesc.MiscFlags = 0;

	// this struct is created just to set a pointer to the
	// data containing the vertices.
	D3D11_SUBRESOURCE_DATA lightData;
	lightData.pSysMem = triangleScreenQuadVertices;
	lightData.SysMemPitch = 0;
	lightData.SysMemSlicePitch = 0;

	// create a Vertex Buffer
	this->gDevice->CreateBuffer(&lightBufferDesc, &lightData, &this->gVertexBuffer);

	return screenQuadVertexSize;
}

void FullScreenQuad::createMatrices()
{
	this->scaling = XMMatrixScaling(1, 1, 1);
	this->world = XMMatrixRotationY(0);
	XMMATRIX viewSpace = XMMatrixLookAtLH(this->camPos, this->lookAt, this->up);
	XMMATRIX projection = XMMatrixPerspectiveFovLH(this->fov, this->aspectRatio, this->nearPlane, this->farPlane);
	XMMATRIX worldViewProj1 = XMMatrixMultiply(this->world, viewSpace);
	XMMATRIX worldViewProj = XMMatrixMultiplyTranspose(worldViewProj1, projection);

	this->VSLConstData = { XMMatrixTranspose(this->world), worldViewProj };
}

void FullScreenQuad::createLights()
{	
	XMVECTOR lightPos = XMVectorSet(10.0f, 20.0f, 1.0f, 1.0f);
	XMVECTOR lightColor = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	XMVECTOR lightAmbient = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);

	this->PSLConstData = { lightPos, lightColor, lightAmbient };	
}

HRESULT FullScreenQuad::createConstantBuffers()
{
	HRESULT hr = 0;

	D3D11_BUFFER_DESC vslCbDesc;
	vslCbDesc.ByteWidth = sizeof(GS_CONSTANT_BUFFER);
	vslCbDesc.Usage = D3D11_USAGE_DYNAMIC;
	vslCbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	vslCbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vslCbDesc.MiscFlags = 0;
	vslCbDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vslData;
	vslData.pSysMem = &VSLConstData;
	vslData.SysMemPitch = 0;
	vslData.SysMemSlicePitch = 0;

	hr = this->gDevice->CreateBuffer(&vslCbDesc, &vslData,
		&this->gVSLConstantBuffer);
	if (FAILED(hr))
	{
		return hr;
	}

	D3D11_BUFFER_DESC lbDesc;	
	lbDesc.ByteWidth = sizeof(PSL_LIGHT_BUFFER);
	lbDesc.Usage = D3D11_USAGE_DYNAMIC;
	lbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lbDesc.MiscFlags = 0;
	lbDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA lbData;
	lbData.pSysMem = &PSLConstData;
	lbData.SysMemPitch = 0;
	lbData.SysMemSlicePitch = 0;

	hr = this->gDevice->CreateBuffer(&lbDesc, &lbData,
		&this->gPSLConstantBuffer);
	if (FAILED(hr))
	{
		return hr;
	}

	D3D11_BUFFER_DESC cDesc;
	cDesc.ByteWidth = sizeof(CAMERA_BUFFER);
	cDesc.Usage = D3D11_USAGE_DYNAMIC;
	cDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cDesc.MiscFlags = 0;
	cDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA cData;
	cData.pSysMem = &PSLConstData;
	cData.SysMemPitch = 0;
	cData.SysMemSlicePitch = 0;

	hr = this->gDevice->CreateBuffer(&cDesc, &cData,
		&this->gPSLCameraBuffer);
	if (FAILED(hr))
	{
		return hr;
	}

	return S_OK;
}

HRESULT FullScreenQuad::createSamplerState()
{
	HRESULT hr = 0;

	D3D11_SAMPLER_DESC sampAni;
	sampAni.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampAni.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampAni.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampAni.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampAni.MaxAnisotropy = 4;
	sampAni.MipLODBias = 0;	

	hr = this->gDevice->CreateSamplerState(&sampAni, &this->gSamplerState);
	if (FAILED(hr))
	{
		return hr;
	}	

	return S_OK;
}

void FullScreenQuad::setPipeline(ID3D11RenderTargetView* inRTV, ID3D11DepthStencilView* inDSV)
{
	this->gDeviceContext->VSSetShader(this->shader->getVertexShader(), nullptr, 0);
	this->gDeviceContext->HSSetShader(nullptr, nullptr, 0);
	this->gDeviceContext->DSSetShader(nullptr, nullptr, 0);
	this->gDeviceContext->GSSetShader(nullptr, nullptr, 0);
	this->gDeviceContext->PSSetShader(this->shader->getPixelShader(), nullptr, 0);

	UINT32 vertexSizeLight = sizeof(TriangleVertexTex);
	UINT32 offsetLight = 0;

	// specify which vertex buffer to use next.
	this->gDeviceContext->IASetVertexBuffers(0, 1, &this->gVertexBuffer, &vertexSizeLight, &offsetLight);

	this->gDeviceContext->OMSetRenderTargets(1,
		&inRTV,
		inDSV);	

	//Bind constant buffer to the vertex shader
	this->gDeviceContext->VSSetConstantBuffers(0, 1, &this->gVSLConstantBuffer);
	//Bind constant buffer to the light pixel shader
	this->gDeviceContext->PSSetConstantBuffers(0, 1, &this->gPSLConstantBuffer);
	this->gDeviceContext->PSSetConstantBuffers(1, 1, &this->gPSLCameraBuffer);
	// specify the topology to use when drawing
	this->gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//Bind textures to light pixel shader
	this->gDeviceContext->PSSetShaderResources(0, NR_OF_TARGET_AND_RESOURCE_VIEWS,
		this->gShaderResourceView);
	//Set sampler state	
	this->gDeviceContext->PSSetSamplers(0, 1, &this->gSamplerState);
	this->gDeviceContext->IASetInputLayout(this->shader->getVertexLayout());
}

void FullScreenQuad::clearPipeline()
{
	ID3D11Buffer* nullBuffer[2] = { nullptr, nullptr };
	this->gDeviceContext->VSSetConstantBuffers(0, 2, nullBuffer);
	this->gDeviceContext->PSSetConstantBuffers(0, 2, nullBuffer);
}

FullScreenQuad::FullScreenQuad(ID3D11Device * inDevice, ID3D11DeviceContext * inDeviceContext,
	ID3D11ShaderResourceView* inSRV[], CameraClass* camera)
{
	this->gDevice = inDevice;
	this->gDeviceContext = inDeviceContext;
	this->camera = camera;	

	for (int i = 0; i < NR_OF_TARGET_AND_RESOURCE_VIEWS; i++)
	{
		this->gShaderResourceView[i] = inSRV[i];
	}	

	this->nrOfVertices = this->createTriangleData();	

	this->createMatrices();
	this->createLights();
	this->createConstantBuffers();

	inDeviceContext->Map(this->gPSLCameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &dataPtr);		
	this->PSLCameraData = (CAMERA_BUFFER*) dataPtr.pData;
	this->PSLCameraData->camPos = this->camera->getPositionXM();	
	inDeviceContext->Unmap(this->gPSLCameraBuffer, 0);	

	this->shader = new Shader();
	this->shader->createShader(this->gDevice, ShaderType::PositionTextureForLightPass);

	this->createSamplerState();
}

FullScreenQuad::~FullScreenQuad()
{
	this->gVertexBuffer->Release();
	this->gVSLConstantBuffer->Release();
	this->gPSLConstantBuffer->Release();	
	this->gPSLCameraBuffer->Release();	
	this->gSamplerState->Release();

	delete this->shader;	
}

void FullScreenQuad::update()
{
	this->gDeviceContext->Map(this->gPSLCameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &dataPtr);
	this->PSLCameraData = (CAMERA_BUFFER*)dataPtr.pData;
	this->PSLCameraData->camPos = this->camera->getPositionXM();
	this->gDeviceContext->Unmap(this->gPSLCameraBuffer, 0);		
}

void FullScreenQuad::render(ID3D11RenderTargetView* inRTV, ID3D11DepthStencilView* inDSV,
	ShaderType shaderType)
{
	this->setPipeline(inRTV, inDSV);

	this->gDeviceContext->Draw(this->nrOfVertices, 0);

	this->clearPipeline();
}
