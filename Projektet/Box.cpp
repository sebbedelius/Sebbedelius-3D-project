#include "Box.h"

HRESULT Box::createBox()
{
	HRESULT hr = 0; 
	TextureLoader* texLoad = new TextureLoader();

	this->nrOfVertices = 6 * 6; // Four sides and four vertices per side

	TriangleVertexTex triangleVertices[36] =
	{
		-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, // Top left
		0.5f, 0.5f, -0.5f, 1.0f, 0.0f, // Top right
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // Bottom left
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // Bottom left
		0.5f, 0.5f, -0.5f, 1.0f, 0.0f, // Top right
		0.5f, -0.5f, -0.5f, 1.0f, 1.0f, // Bottom right

		0.5f, 0.5f, -0.5f, 0.0f, 0.0f, // Top left
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // Top right
		0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // Bottom left
		0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // Bottom left
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // Top right
		0.5f, -0.5f, 0.5f, 1.0f, 1.0f, // Bottom right

		0.5f, 0.5f, 0.5f, 0.0f, 0.0f, // Top left
		-0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // Top right
		0.5f, -0.5f, 0.5f, 0.0f, 1.0f, // Bottom left
		0.5f, -0.5f, 0.5f, 0.0f, 1.0f, // Bottom left
		-0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // Top right
		-0.5f, -0.5f, 0.5f, 1.0f, 1.0f, // Bottom right

		-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, // Top left
		-0.5f, 0.5f, -0.5f, 1.0f, 0.0f, // Top right
		-0.5f, -0.5f, 0.5f, 0.0f, 1.0f, // Bottom left
		-0.5f, -0.5f, 0.5f, 0.0f, 1.0f, // Bottom left
		-0.5f, 0.5f, -0.5f, 1.0f, 0.0f, // Top right
		-0.5f, -0.5f, -0.5, 1.0f, 1.0f, // Bottom right

		0.5f, 0.5f, -0.5, 0.0f, 0.0f, // Top left
		-0.5f, 0.5f, -0.5f, 1.0f, 0.0f, // Top right
		0.5f, 0.5f, 0.5f, 0.0f, 1.0f, // Bottom left
		0.5f, 0.5f, 0.5f, 0.0f, 1.0f, // Bottom left
		-0.5f, 0.5f, -0.5f, 1.0f, 0.0f, // Top right
		-0.5f, 0.5f, 0.5f, 1.0f, 1.0f, // Bottom right

		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // Top left
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, // Top right
		-0.5f, -0.5f, 0.5f, 0.0f, 1.0f, // Bottom left
		-0.5f, -0.5f, 0.5f, 0.0f, 1.0f, // Bottom left
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, // Top right
		0.5f, -0.5f, 0.5f, 1.0f, 1.0f, // Bottom right
	};

	//Store the positions for the bounding box
	this->vertex.resize(36);
	for (int i = 0; i < 36; i++)
	{
		this->vertex[i].pos.x = triangleVertices[i].x;
		this->vertex[i].pos.y = triangleVertices[i].y;
		this->vertex[i].pos.z = triangleVertices[i].z;
	}

	D3D11_BUFFER_DESC bDesc;
	memset(&bDesc, 0, sizeof(bDesc));
	bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bDesc.ByteWidth = sizeof(triangleVertices);
	bDesc.Usage = D3D11_USAGE_DEFAULT;
	bDesc.CPUAccessFlags = 0;
	bDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA bData;
	memset(&bData, 0, sizeof(bData));
	bData.pSysMem = triangleVertices;
	bData.SysMemPitch = 0;
	bData.SysMemSlicePitch = 0;

	hr = this->gDevice->CreateBuffer(&bDesc, &bData, &this->gVertexBuffer);
	if (FAILED(hr))
	{
		return hr;
	}

	texLoad->loadFromFile(this->gDevice, L"teak_R.jpg");
	this->textureRV = texLoad->getTextureRV();

	delete texLoad;

	return S_OK;
}

HRESULT Box::createConstantBuffers()
{
	HRESULT hr = 0; 

	D3D11_BUFFER_DESC camDesc;
	camDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	camDesc.ByteWidth = sizeof(CAMERA_BUFFER);
	camDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	camDesc.MiscFlags = 0;
	camDesc.StructureByteStride = 0;
	camDesc.Usage = D3D11_USAGE_DYNAMIC;

	D3D11_SUBRESOURCE_DATA camData;
	camData.pSysMem = &this->gCamData;
	camData.SysMemPitch = 0;
	camData.SysMemSlicePitch = 0;

	hr = this->gDevice->CreateBuffer(&camDesc, &camData, &this->gCameraBuffer);
	if (FAILED(hr))
	{
		return hr;
	}

	D3D11_BUFFER_DESC vsbDesc;
	vsbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	vsbDesc.ByteWidth = sizeof(SHADOW_VS_CONSTANT_BUFFER);
	vsbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vsbDesc.MiscFlags = 0;
	vsbDesc.StructureByteStride = 0;
	vsbDesc.Usage = D3D11_USAGE_DYNAMIC;

	D3D11_SUBRESOURCE_DATA vsbData;
	vsbData.pSysMem = &this->vssConstData;
	vsbData.SysMemPitch = 0;
	vsbData.SysMemSlicePitch = 0;

	hr = this->gDevice->CreateBuffer(&vsbDesc, &vsbData, &this->gVSShadowConstantBuffer);
	if (FAILED(hr))
	{
		return hr;
	}

	D3D11_BUFFER_DESC sbDesc;
	sbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	sbDesc.ByteWidth = sizeof(SHADOW_GS_CONSTANT_BUFFER);
	sbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	sbDesc.MiscFlags = 0;
	sbDesc.StructureByteStride = 0;
	sbDesc.Usage = D3D11_USAGE_DYNAMIC;

	D3D11_SUBRESOURCE_DATA sbData;
	sbData.pSysMem = &this->sConstData;
	sbData.SysMemPitch = 0;
	sbData.SysMemSlicePitch = 0;

	hr = this->gDevice->CreateBuffer(&sbDesc, &sbData, &this->gShadowConstantBuffer);
	if (FAILED(hr))
	{
		return hr;
	}	

	return S_OK;
}

void Box::createMatrices()
{
	this->world = XMMatrixRotationY(0);
	XMMATRIX scaling = XMMatrixScaling(3.0f, 3.0f, 3.0f);
	XMMATRIX transform = XMMatrixTranslation(19.0f, 5.25f, -10.0f); // 19, 5.25, -10
	this->mPostion = XMFLOAT3(40.0f, 40.0f, 0.0f);
	this->world = XMMatrixMultiply(this->world, scaling);
	this->world = XMMatrixMultiply(this->world, transform);

	this->worldViewProj = XMMatrixMultiply(this->world, this->mCamera->getView());
	this->worldViewProj = XMMatrixMultiplyTranspose(this->worldViewProj, this->mCamera->getProj());

	this->sunWorldViewProj = XMMatrixMultiply(this->world, this->mSun->getViewXM());
	this->sunWorldViewProj = XMMatrixMultiplyTranspose(this->sunWorldViewProj, this->mSun->getProjXM());

	this->gDeviceContext->Map(this->gShadowConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &this->dataPtr);
	this->sConstData = (SHADOW_GS_CONSTANT_BUFFER*)this->dataPtr.pData;
	this->sConstData->theWorld = XMMatrixTranspose(this->world);
	this->sConstData->theWorldViewProj = this->worldViewProj;
	this->sConstData->theSunWorldViewProj = this->sunWorldViewProj;
	this->sConstData->theView = XMMatrixTranspose(this->mCamera->getView());
	this->gDeviceContext->Unmap(this->gShadowConstantBuffer, 0);

	this->gDeviceContext->Map(this->gVSShadowConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &dataPtr);
	this->vssConstData = (SHADOW_VS_CONSTANT_BUFFER*)dataPtr.pData;
	this->vssConstData->theWorld = XMMatrixTranspose(this->world);
	this->vssConstData->theWorldViewProj = this->sunWorldViewProj;
	this->gDeviceContext->Unmap(this->gVSShadowConstantBuffer, 0);
}

void Box::createMatricesForStaticCamera()
{
	this->world = XMMatrixRotationY(0);
	XMMATRIX scaling = XMMatrixScaling(3.0f, 3.0f, 3.0f);
	XMMATRIX transform = XMMatrixTranslation(19.0f, 5.25f, -10.0f); // 19, 5.25, -10
	this->mPostion = XMFLOAT3(40.0f, 40.0f, 0.0f);
	this->world = XMMatrixMultiply(this->world, scaling);
	this->world = XMMatrixMultiply(this->world, transform);

	this->worldViewProj = XMMatrixMultiply(this->world, this->mStaticCamera->getView());
	this->worldViewProj = XMMatrixMultiplyTranspose(this->worldViewProj, this->mStaticCamera->getProj());

	this->sunWorldViewProj = XMMatrixMultiply(this->world, this->mSun->getViewXM());
	this->sunWorldViewProj = XMMatrixMultiplyTranspose(this->sunWorldViewProj, this->mSun->getProjXM());

	this->gDeviceContext->Map(this->gShadowConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &this->dataPtr);
	this->sConstData = (SHADOW_GS_CONSTANT_BUFFER*)this->dataPtr.pData;
	this->sConstData->theWorld = XMMatrixTranspose(this->world);
	this->sConstData->theWorldViewProj = this->worldViewProj;
	this->sConstData->theSunWorldViewProj = this->sunWorldViewProj;
	this->sConstData->theView = XMMatrixTranspose(this->mCamera->getView());
	this->gDeviceContext->Unmap(this->gShadowConstantBuffer, 0);

	this->gDeviceContext->Map(this->gVSShadowConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &dataPtr);
	this->vssConstData = (SHADOW_VS_CONSTANT_BUFFER*)dataPtr.pData;
	this->vssConstData->theWorld = XMMatrixTranspose(this->world);
	this->vssConstData->theWorldViewProj = this->sunWorldViewProj;
	this->gDeviceContext->Unmap(this->gVSShadowConstantBuffer, 0);
}

void Box::updateMatrices()
{
	this->worldViewProj = XMMatrixMultiply(this->world, this->mCamera->getView());
	this->worldViewProj = XMMatrixMultiplyTranspose(this->worldViewProj, this->mCamera->getProj());

	this->gDeviceContext->Map(this->gShadowConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &this->dataPtr);
	this->sConstData = (SHADOW_GS_CONSTANT_BUFFER*)this->dataPtr.pData;	
	this->sConstData->theWorld = XMMatrixTranspose(this->world);
	this->sConstData->theWorldViewProj = this->worldViewProj;
	this->sConstData->theSunWorldViewProj = this->sunWorldViewProj;
	this->sConstData->theView = XMMatrixTranspose(this->mCamera->getView());
	this->gDeviceContext->Unmap(this->gShadowConstantBuffer, 0);
}

void Box::updateMatricesForStaticCamera()
{
	this->worldViewProj = XMMatrixMultiply(this->world, this->mStaticCamera->getView());
	this->worldViewProj = XMMatrixMultiplyTranspose(this->worldViewProj, this->mStaticCamera->getProj());

	this->gDeviceContext->Map(this->gShadowConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &this->dataPtr);
	this->sConstData = (SHADOW_GS_CONSTANT_BUFFER*)this->dataPtr.pData;
	this->sConstData->theWorld = XMMatrixTranspose(this->world);
	this->sConstData->theWorldViewProj = this->worldViewProj;
	this->sConstData->theSunWorldViewProj = this->sunWorldViewProj;
	this->sConstData->theView = XMMatrixTranspose(this->mCamera->getView());
	this->gDeviceContext->Unmap(this->gShadowConstantBuffer, 0);
}

HRESULT Box::createSamplerState()
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

HRESULT Box::createBlendState()
{
	HRESULT hr = 0;

	D3D11_BLEND_DESC blendDesc;
	memset(&blendDesc, 0, sizeof(blendDesc));
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	hr = this->gDevice->CreateBlendState(&blendDesc, &this->gBlendState);
	if (FAILED(hr))
	{
		return hr;
	}

	return S_OK;
}

void Box::setPipelineShadowPass()
{
	this->gDeviceContext->VSSetShader(this->shadowShader->getVertexShader(), nullptr, 0);
	this->gDeviceContext->HSSetShader(nullptr, nullptr, 0);
	this->gDeviceContext->DSSetShader(nullptr, nullptr, 0);
	this->gDeviceContext->GSSetShader(nullptr, nullptr, 0);
	this->gDeviceContext->PSSetShader(nullptr, nullptr, 0);

	UINT32 vertexSize = sizeof(TriangleVertexTex);
	UINT32 offset = 0;

	// specify which vertex buffer to use next.
	this->gDeviceContext->IASetVertexBuffers(0, 1, &this->gVertexBuffer, &vertexSize, &offset);	
	//specify which constant buffer to use next
	this->gDeviceContext->VSSetConstantBuffers(0, 1, &this->gVSShadowConstantBuffer);
	// specify the topology to use when drawing
	this->gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	
	this->gDeviceContext->IASetInputLayout(this->shader->getVertexLayout());

	this->gDeviceContext->RSSetState(this->mShadowMap->getRasterizerState());
}

void Box::setPipeline(ID3D11RenderTargetView * inRTV, ID3D11DepthStencilView * inDSV)
{
	this->gDeviceContext->VSSetShader(this->shader->getVertexShader(), nullptr, 0);
	this->gDeviceContext->HSSetShader(nullptr, nullptr, 0);
	this->gDeviceContext->DSSetShader(nullptr, nullptr, 0);
	this->gDeviceContext->GSSetShader(this->shader->getGeometryShader(), nullptr, 0);
	this->gDeviceContext->PSSetShader(this->shader->getPixelShader(), nullptr, 0);

	UINT32 vertexSize = sizeof(TriangleVertexTex);
	UINT32 offset = 0;

	// specify which vertex buffer to use next.
	this->gDeviceContext->IASetVertexBuffers(0, 1, &this->gVertexBuffer, &vertexSize, &offset);
	//Set render targets (textures)
	this->gDeviceContext->OMSetRenderTargets(1,
		&inRTV,
		inDSV);
	FLOAT blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	//Bind blend state to the OM state
	this->gDeviceContext->OMSetBlendState(this->gBlendState, blendFactor, 0xffffffff);
	//Bind constant buffer to the geometry shader
	this->gDeviceContext->GSSetConstantBuffers(0, 1, &this->gShadowConstantBuffer);
	this->gDeviceContext->GSSetConstantBuffers(1, 1, &this->gCameraBuffer);
	// specify the topology to use when drawing
	this->gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	
	//Bind textures to pixel shader.	
	ID3D11ShaderResourceView* nullSRV = nullptr;
	ID3D11ShaderResourceView* textures[3] = { this->textureRV, nullSRV, this->mShadowMap->getDepthMapSRV() };
	this->gDeviceContext->PSSetShaderResources(0, 3, textures);
	//Set sampler state
	this->gDeviceContext->PSSetSamplers(0, 1, &this->gSamplerState);
	//Set vertex layout
	this->gDeviceContext->IASetInputLayout(this->shader->getVertexLayout());
}

void Box::clearPipeline()
{
	ID3D11ShaderResourceView* nullSRV[3] = { nullptr, nullptr, nullptr };
	ID3D11Buffer* nullBuffer[1] = { nullptr };
	ID3D11Buffer* nullBuffer2[2] = { nullptr, nullptr };
	ID3D11RasterizerState* nullR = nullptr;
	FLOAT blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	this->gDeviceContext->VSSetConstantBuffers(0, 1, nullBuffer);	
	this->gDeviceContext->GSSetConstantBuffers(1, 1, nullBuffer2);
	this->gDeviceContext->OMSetBlendState(0, blendFactor, 0xffffffff);	
	this->gDeviceContext->RSSetState(nullR);
	this->gDeviceContext->PSSetShaderResources(0, 3, nullSRV);
}

Box::Box(ID3D11Device * inDevice, ID3D11DeviceContext * inDeviceContext,
	CameraClass * inCamera, StaticCamera* inStaticCamera, Sun* inSun, ShadowMap* inShadowMap, ShaderType shaderType)
{
	this->gDevice = inDevice;
	this->gDeviceContext = inDeviceContext;
	this->shaderType = shaderType;
	this->mCamera = inCamera;
	this->mStaticCamera = inStaticCamera;
	this->mSun = inSun;
	this->mShadowMap = inShadowMap;

	this->gVertexBuffer = nullptr;
	this->gShadowConstantBuffer = nullptr;
	this->gShadowConstantBuffer = nullptr;
	this->gSamplerState = nullptr;
	this->textureRV = nullptr;

	this->createBox();
	this->createBoundingBox();
	this->createConstantBuffers();

	if (this->mCamera->updateAndGetStatic() == false)
	{
		this->createMatrices();
	}
	else
	{
		this->createMatricesForStaticCamera();
	}
	
	inDeviceContext->Map(this->gCameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &this->dataPtr);
	this->gCamData = (CAMERA_BUFFER*)this->dataPtr.pData;
	this->gCamData->camPos = this->mCamera->getPositionXM();
	inDeviceContext->Unmap(this->gCameraBuffer, 0);

	this->createSamplerState();
	this->createBlendState();

	this->shader = new Shader();
	this->shadowShader = new Shader();
	this->shader->createShader(inDevice, shaderType);
	this->shadowShader->createShader(inDevice, ShaderType::PositionTexShadowPass);
}

Box::~Box()
{
	this->gVertexBuffer->Release();
	this->gShadowConstantBuffer->Release();
	this->gVSShadowConstantBuffer->Release();
	this->gCameraBuffer->Release();

	this->gSamplerState->Release();
	this->gBlendState->Release();
	this->textureRV->Release();

	delete this->shader;
	delete this->shadowShader;
}

void Box::createBoundingBox()
{
	this->mOBB.CreateFromPoints(this->mOBB, this->nrOfVertices, &this->vertex[0].pos, sizeof(Vertex));

	XMMATRIX scaling = XMMatrixScaling(3.0f, 3.0f, 3.0f);
	XMMATRIX transform = XMMatrixTranslation(19.0f, 5.25f, -10.0f); // 19, 5.25, -10

	this->mOBB.Transform(this->mOBB, scaling);
	this->mOBB.Transform(this->mOBB, transform);
}

int Box::getId() const
{
	return this->id;
}

BoundingBox Box::getBoundingBox() const
{
	return this->mOBB;
}

bool Box::getDraw() const
{
	return this->draw;
}

void Box::setId(int id)
{
	this->id = id;
}

void Box::setPosition(XMFLOAT3 pos)
{
	this->world = XMMatrixTranslation(-this->mPostion.x, -this->mPostion.y, -this->mPostion.z);
	this->world = XMMatrixTranslation(pos.x, pos.y, pos.z);
	this->mPostion = pos;
}

void Box::setDraw(bool value)
{
	this->draw = value;
}

void Box::update()
{
	if (this->mCamera->updateAndGetStatic() == false)
	{
		this->updateMatrices();
	}
	else
	{
		this->updateMatricesForStaticCamera();
	}
	
	this->gDeviceContext->Map(this->gCameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &this->dataPtr);
	this->gCamData = (CAMERA_BUFFER*)this->dataPtr.pData;
	this->gCamData->camPos = this->mCamera->getPositionXM();
	this->gDeviceContext->Unmap(this->gCameraBuffer, 0);
}

void Box::renderShadowPass()
{
	this->setPipelineShadowPass();
	this->gDeviceContext->Draw(this->nrOfVertices, 0);
	this->clearPipeline();
}

void Box::render(ID3D11RenderTargetView* inRTV, ID3D11DepthStencilView* inDSV)
{
	this->setPipeline(inRTV, inDSV);
	this->gDeviceContext->Draw(this->nrOfVertices, 0);
	this->clearPipeline();
}
