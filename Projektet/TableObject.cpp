#include "TableObject.h"

TableObject::TableObject(ID3D11Device* inDevice, ID3D11DeviceContext* inDeviceContext,
	CameraClass* inCamera, StaticCamera* inStaticCamera, Sun* inSun, ShadowMap* inShadowMap, ShaderType shaderType)
{
	this->gDevice = inDevice; 
	this->gDeviceContext = inDeviceContext;
	this->camera = inCamera;
	this->staticCamera = inStaticCamera;
	this->mSun = inSun;
	this->mShadowMap = inShadowMap;

	this->textureRV = new ID3D11ShaderResourceView*[2];
	this->samplerStates = new ID3D11SamplerState*[2];

	this->nrOfVertices = this->createTriangleData();	
	this->psConstData = { XMFLOAT3(0.5f, 0.5f, 0.5f), 2.0f };	

	this->createConstantBuffers();	

	if (this->camera->updateAndGetStatic() == false)
	{
		this->createMatrices();
	}
	else
	{
		this->createMatricesForStaticCamera();
	}	

	inDeviceContext->Map(this->gCameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &this->dataPtr);
	this->gCamData = (CAMERA_BUFFER*)this->dataPtr.pData;
	this->gCamData->camPos = this->camera->getPositionXM();
	inDeviceContext->Unmap(this->gCameraBuffer, 0);

	this->createBoundingBox();

	this->shader = new Shader();
	this->shadowShader = new Shader();
	this->debugShader = new Shader();

	this->shader->createShader(inDevice, shaderType);
	if (shaderType == ShaderType::PositionTexture)
	{
		this->shadowShader->createShader(inDevice, ShaderType::PositionTexShadowPass);
	}

	this->debugShader->createShader(inDevice, ShaderType::Position);

	this->createSamplerStates();	

	
}

TableObject::~TableObject()
{	
	this->gShadowConstantBuffer->Release();
	this->gVSShadowConstantBuffer->Release();
	this->gVertexBuffer->Release();	
	this->gPSConstantBuffer->Release();		
	this->gCameraBuffer->Release();
	//this->gBoundingVertexBuffer->Release();
	this->gDbgConstantBuffer->Release();

	for(int i = 0; i < 2; i++)
		this->textureRV[i]->Release();

	delete[] this->textureRV;

	for (int i = 0; i < 2; i++)
		this->samplerStates[i]->Release();

	delete[] this->samplerStates;
	delete this->shader;	
	delete this->shadowShader;		
	delete this->debugShader;
}

int TableObject::createTriangleData()
{
	ObjLoader* tableObj = new ObjLoader();
	TextureLoader* tableTex = new TextureLoader();
	bool loaded = false;

	//Load the object and create a vertex buffer
	loaded = tableObj->loadObj("table.obj");

	int sizeOfTriangleVertex = tableObj->getNrOfIndices();
	this->triangleVertices.resize(sizeOfTriangleVertex);
	/*Vertex * vertex;
	vertex = new Vertex[tableObj->getNrOfIndices()];*/
	this->vertex.resize(tableObj->getNrOfIndices());
	Texture* texture = new Texture[tableObj->getNrOfIndices()];
	tableObj->getVertices(&this->vertex[0]);
	tableObj->getTextures(texture);

	for (int i = 0; i < sizeOfTriangleVertex; i++)
	{
		this->triangleVertices[i].x = this->vertex[i].pos.x;
		this->triangleVertices[i].y = this->vertex[i].pos.y;
		this->triangleVertices[i].z = this->vertex[i].pos.z;
		this->triangleVertices[i].u = texture[i].u;
		this->triangleVertices[i].v = texture[i].v;		
	}

	// Describe the Vertex Buffer
	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	// what type of buffer will this be?
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	// what type of usage (press F1, read the docs)
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	// how big in bytes each element in the buffer is.
	bufferDesc.ByteWidth = sizeof(TriangleVertexTex) * sizeOfTriangleVertex;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	// this struct is created just to set a pointer to the
	// data containing the vertices.
	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &this->triangleVertices[0];
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	// create a Vertex Buffer
	this->gDevice->CreateBuffer(&bufferDesc, &data, &this->gVertexBuffer);

	//Load texture

	tableTex->loadFromFile(this->gDevice, L"tableTex.jpg");
	this->textureRV[0] = tableTex->getTextureRV();
	tableTex->loadFromFile(this->gDevice, L".//Images//Hello.jpeg");
	this->textureRV[1] = tableTex->getTextureRV();

	//delete vertex;
	delete texture;
	delete tableObj;
	delete tableTex;	

	return sizeOfTriangleVertex;
}

void TableObject::createMatrices()
{
	this->scaling = DirectX::XMMatrixScaling(3.5f, 3.5f, 3.5f);
	XMMATRIX transform = XMMatrixTranslation(-5.0f, 0.0f, 20.0f); // -5.0, 0.0, 20
	XMMATRIX rotation = XMMatrixRotationY(90.3f);
	this->world = XMMatrixMultiply(this->scaling, XMMatrixRotationY(0));
	this->world = XMMatrixMultiply(this->world, transform);
	this->world = XMMatrixMultiply(this->world, rotation);

	this->worldViewProj = XMMatrixMultiply(this->world, this->camera->getView());
	this->worldViewProj = XMMatrixMultiply(this->worldViewProj, this->camera->getProj());
	this->worldViewProj = XMMatrixTranspose(this->worldViewProj);

	this->sunWorldViewProj = XMMatrixMultiply(this->world, this->mSun->getViewXM());
	this->sunWorldViewProj = XMMatrixMultiply(this->sunWorldViewProj, this->mSun->getProjXM());
	this->sunWorldViewProj = XMMatrixTranspose(this->sunWorldViewProj);

	this->gDeviceContext->Map(this->gShadowConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &this->dataPtr);
	this->sConstData = (SHADOW_GS_CONSTANT_BUFFER*)this->dataPtr.pData;
	this->sConstData->theWorld = XMMatrixTranspose(this->world);
	this->sConstData->theWorldViewProj = this->worldViewProj;
	this->sConstData->theSunWorldViewProj = this->sunWorldViewProj;
	this->sConstData->theView = XMMatrixTranspose(this->camera->getView());
	this->gDeviceContext->Unmap(this->gShadowConstantBuffer, 0);

	this->gDeviceContext->Map(this->gVSShadowConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &dataPtr);
	this->vssConstData = (SHADOW_VS_CONSTANT_BUFFER*)dataPtr.pData;
	this->vssConstData->theWorld = XMMatrixTranspose(this->world);
	this->vssConstData->theWorldViewProj = this->sunWorldViewProj;
	this->gDeviceContext->Unmap(this->gVSShadowConstantBuffer, 0);

	this->gDeviceContext->Map(this->gDbgConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &dataPtr);
	this->gDbgConstData = (GS_CONSTANT_BUFFER*)dataPtr.pData;
	this->gDbgConstData->theWorld = XMMatrixTranspose(this->world);
	this->gDbgConstData->theWorldViewProj = this->worldViewProj;
	this->gDeviceContext->Unmap(this->gDbgConstantBuffer, 0);
}

void TableObject::createMatricesForStaticCamera()
{
	this->scaling = DirectX::XMMatrixScaling(3.5f, 3.5f, 3.5f);
	XMMATRIX transform = XMMatrixTranslation(-5.0f, 0.0f, 20.0f); // -5.0, 0.0, 20
	XMMATRIX rotation = XMMatrixRotationY(90.3f);
	this->world = XMMatrixMultiply(this->scaling, XMMatrixRotationY(0));
	this->world = XMMatrixMultiply(this->world, transform);
	this->world = XMMatrixMultiply(this->world, rotation);

	this->worldViewProj = XMMatrixMultiply(this->world, this->staticCamera->getView());
	this->worldViewProj = XMMatrixMultiply(this->worldViewProj, this->staticCamera->getProj());
	this->worldViewProj = XMMatrixTranspose(this->worldViewProj);

	this->sunWorldViewProj = XMMatrixMultiply(this->world, this->mSun->getViewXM());
	this->sunWorldViewProj = XMMatrixMultiply(this->sunWorldViewProj, this->mSun->getProjXM());
	this->sunWorldViewProj = XMMatrixTranspose(this->sunWorldViewProj);

	this->gDeviceContext->Map(this->gShadowConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &this->dataPtr);
	this->sConstData = (SHADOW_GS_CONSTANT_BUFFER*)this->dataPtr.pData;
	this->sConstData->theWorld = XMMatrixTranspose(this->world);
	this->sConstData->theWorldViewProj = this->worldViewProj;
	this->sConstData->theSunWorldViewProj = this->sunWorldViewProj;
	this->sConstData->theView = XMMatrixTranspose(this->camera->getView());
	this->gDeviceContext->Unmap(this->gShadowConstantBuffer, 0);

	this->gDeviceContext->Map(this->gVSShadowConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &dataPtr);
	this->vssConstData = (SHADOW_VS_CONSTANT_BUFFER*)dataPtr.pData;
	this->vssConstData->theWorld = XMMatrixTranspose(this->world);
	this->vssConstData->theWorldViewProj = this->sunWorldViewProj;
	this->gDeviceContext->Unmap(this->gVSShadowConstantBuffer, 0);

	this->gDeviceContext->Map(this->gDbgConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &dataPtr);
	this->gDbgConstData = (GS_CONSTANT_BUFFER*)dataPtr.pData;
	this->gDbgConstData->theWorld = XMMatrixTranspose(this->world);
	this->gDbgConstData->theWorldViewProj = this->worldViewProj;
	this->gDeviceContext->Unmap(this->gDbgConstantBuffer, 0);
}

void TableObject::updateMatrices()
{
	this->worldViewProj = XMMatrixMultiply(this->world, this->camera->getView());
	this->worldViewProj = XMMatrixMultiply(this->worldViewProj, this->camera->getProj());
	this->worldViewProj = XMMatrixTranspose(this->worldViewProj);

	this->gDeviceContext->Map(this->gShadowConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &this->dataPtr);	
	this->sConstData = (SHADOW_GS_CONSTANT_BUFFER*)this->dataPtr.pData;	
	this->sConstData->theWorld = XMMatrixTranspose(this->world);
    this->sConstData->theWorldViewProj = this->worldViewProj;
	this->sConstData->theSunWorldViewProj = this->sunWorldViewProj;
	this->sConstData->theView = XMMatrixTranspose(this->camera->getView());
	this->gDeviceContext->Unmap(this->gShadowConstantBuffer, 0);

	XMMATRIX worldTemp = XMMatrixIdentity();
	XMMATRIX worldViewProjTemp = XMMatrixMultiply(worldTemp, this->camera->getView());
	worldViewProjTemp = XMMatrixMultiply(worldViewProjTemp, this->camera->getProj());
	worldViewProjTemp = XMMatrixTranspose(worldViewProjTemp);

	this->gDeviceContext->Map(this->gDbgConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &dataPtr);
	this->gDbgConstData = (GS_CONSTANT_BUFFER*)dataPtr.pData;
	this->gDbgConstData->theWorld = XMMatrixTranspose(this->world);
	this->gDbgConstData->theWorldViewProj = worldViewProjTemp;
	this->gDeviceContext->Unmap(this->gDbgConstantBuffer, 0);
}

void TableObject::updateMatricesForStaticCamera()
{
	this->worldViewProj = XMMatrixMultiply(this->world, this->staticCamera->getView());
	this->worldViewProj = XMMatrixMultiply(this->worldViewProj, this->staticCamera->getProj());
	this->worldViewProj = XMMatrixTranspose(this->worldViewProj);

	this->gDeviceContext->Map(this->gShadowConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &this->dataPtr);
	this->sConstData = (SHADOW_GS_CONSTANT_BUFFER*)this->dataPtr.pData;
	this->sConstData->theWorld = XMMatrixTranspose(this->world);
	this->sConstData->theWorldViewProj = this->worldViewProj;
	this->sConstData->theSunWorldViewProj = this->sunWorldViewProj;
	this->sConstData->theView = XMMatrixTranspose(this->camera->getView());
	this->gDeviceContext->Unmap(this->gShadowConstantBuffer, 0);

	XMMATRIX worldTemp = XMMatrixIdentity();
	XMMATRIX worldViewProjTemp = XMMatrixMultiply(worldTemp, this->staticCamera->getView());
	worldViewProjTemp = XMMatrixMultiply(worldViewProjTemp, this->staticCamera->getProj());
	worldViewProjTemp = XMMatrixTranspose(worldViewProjTemp);

	this->gDeviceContext->Map(this->gDbgConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &dataPtr);
	this->gDbgConstData = (GS_CONSTANT_BUFFER*)dataPtr.pData;
	this->gDbgConstData->theWorld = XMMatrixTranspose(this->world);
	this->gDbgConstData->theWorldViewProj = worldViewProjTemp;
	this->gDeviceContext->Unmap(this->gDbgConstantBuffer, 0);
}

HRESULT TableObject::createConstantBuffers()
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

	D3D11_BUFFER_DESC dbgDesc;
	dbgDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	dbgDesc.ByteWidth = sizeof(GS_CONSTANT_BUFFER);
	dbgDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	dbgDesc.MiscFlags = 0;
	dbgDesc.StructureByteStride = 0;
	dbgDesc.Usage = D3D11_USAGE_DYNAMIC;

	D3D11_SUBRESOURCE_DATA dbgData;
	dbgData.pSysMem = &this->gDbgConstData;
	dbgData.SysMemPitch = 0;
	dbgData.SysMemSlicePitch = 0;

	hr = this->gDevice->CreateBuffer(&dbgDesc, &dbgData, &this->gDbgConstantBuffer);
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

	D3D11_BUFFER_DESC pcbDesc;
	pcbDesc.ByteWidth = sizeof(PS_CONSTANT_BUFFER);
	pcbDesc.Usage = D3D11_USAGE_DYNAMIC;
	pcbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	pcbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pcbDesc.MiscFlags = 0;
	pcbDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA pdata;
	pdata.pSysMem = &this->psConstData;
	pdata.SysMemPitch = 0;
	pdata.SysMemSlicePitch = 0;

	hr = this->gDevice->CreateBuffer(&pcbDesc, &pdata,
		&this->gPSConstantBuffer);
	if (FAILED(hr))
	{
		return hr;
	}

	return S_OK;	
}

HRESULT TableObject::createSamplerStates()
{
	HRESULT hr = 0; 

	D3D11_SAMPLER_DESC sampAni;
	sampAni.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampAni.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampAni.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampAni.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampAni.MaxAnisotropy = 4;
	sampAni.MipLODBias = 0;	

	D3D11_SAMPLER_DESC sampBorder;
	sampBorder.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;		
	sampBorder.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	sampBorder.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	sampBorder.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	sampBorder.MaxAnisotropy = 4;
	sampBorder.MipLODBias = 0;

	sampBorder.BorderColor[0] = 1.0f;
	sampBorder.BorderColor[1] = 1.0f;
	sampBorder.BorderColor[2] = 1.0f;
	sampBorder.BorderColor[3] = 1.0f;

	hr = this->gDevice->CreateSamplerState(&sampAni, &this->samplerStates[0]);
	if (FAILED(hr))
	{
		return hr;
	}
	hr = this->gDevice->CreateSamplerState(&sampBorder, &this->samplerStates[1]);
	if (FAILED(hr))
	{
		return hr;
	}

	return S_OK;
}

void TableObject::setPipelineShadowPass()
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

	this->gDeviceContext->VSSetConstantBuffers(0, 1, &this->gVSShadowConstantBuffer);

	// specify the topology to use when drawing
	this->gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	this->gDeviceContext->IASetInputLayout(this->shadowShader->getVertexLayout());

	this->gDeviceContext->RSSetState(this->mShadowMap->getRasterizerState());
}

void TableObject::setPipeline(ID3D11RenderTargetView* inRTV, ID3D11DepthStencilView* inDSV)
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
	//Bind constant buffer to the geometry shader
	this->gDeviceContext->GSSetConstantBuffers(0, 1, &this->gShadowConstantBuffer);
	this->gDeviceContext->GSSetConstantBuffers(1, 1, &this->gCameraBuffer);
	this->gDeviceContext->PSSetConstantBuffers(0, 1, &this->gCameraBuffer);
	// specify the topology to use when drawing
	this->gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	
	//Bind textures to pixel shader.	
	ID3D11ShaderResourceView* textures[3] = { this->textureRV[0], this->textureRV[1], this->mShadowMap->getDepthMapSRV() };
	this->gDeviceContext->PSSetShaderResources(0, 3, textures);	
	//Set sampler state
	this->gDeviceContext->PSSetSamplers(0, 2, this->samplerStates);
	//Set vertex layout
	this->gDeviceContext->IASetInputLayout(this->shader->getVertexLayout());
}

void TableObject::setPipeline(ID3D11RenderTargetView * inRTV[], ID3D11DepthStencilView * inDSV)
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
	this->gDeviceContext->OMSetRenderTargets(NR_OF_TARGET_AND_RESOURCE_VIEWS,
		inRTV,
		inDSV);
	//Bind constant buffer to the geometry shader
	this->gDeviceContext->GSSetConstantBuffers(0, 1, &this->gShadowConstantBuffer);
	this->gDeviceContext->GSSetConstantBuffers(1, 1, &this->gCameraBuffer);
	this->gDeviceContext->PSSetConstantBuffers(0, 1, &this->gPSConstantBuffer);	
	// specify the topology to use when drawing
	this->gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//Bind textures to pixel shader.	
	ID3D11ShaderResourceView* textures[3] = { this->textureRV[0], this->textureRV[1], this->mShadowMap->getDepthMapSRV() };
	this->gDeviceContext->PSSetShaderResources(0, 3, textures);
	//Set sampler state
	this->gDeviceContext->PSSetSamplers(0, 2, this->samplerStates);
	//Set vertex layout
	this->gDeviceContext->IASetInputLayout(this->shader->getVertexLayout());
}

void TableObject::setPipelineDebug(ID3D11RenderTargetView * inRTV, ID3D11DepthStencilView * inDSV)
{
	this->gDeviceContext->VSSetShader(this->debugShader->getVertexShader(), nullptr, 0);
	this->gDeviceContext->HSSetShader(nullptr, nullptr, 0);
	this->gDeviceContext->DSSetShader(nullptr, nullptr, 0);
	this->gDeviceContext->GSSetShader(nullptr, nullptr, 0);
	this->gDeviceContext->PSSetShader(this->debugShader->getPixelShader(), nullptr, 0);

	UINT32 vertexSize = sizeof(Vertex);
	UINT32 offset = 0;

	this->gDeviceContext->IASetVertexBuffers(0, 1, &this->gBoundingVertexBuffer, &vertexSize, &offset);

	this->gDeviceContext->VSSetConstantBuffers(0, 1, &this->gDbgConstantBuffer);

	this->gDeviceContext->OMSetRenderTargets(1,
		&inRTV,
		inDSV);
	
	// specify the topology to use when drawing
	this->gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//Bind textures to pixel shader.		
	//Set vertex layout
	this->gDeviceContext->IASetInputLayout(this->debugShader->getVertexLayout());

	ID3D11RasterizerState* rsState;

	D3D11_RASTERIZER_DESC rsDesc;
	memset(&rsDesc, 0, sizeof(rsDesc));
	rsDesc.FillMode = D3D11_FILL_WIREFRAME;
	rsDesc.CullMode = D3D11_CULL_NONE;
	rsDesc.DepthClipEnable = false;
	rsDesc.FrontCounterClockwise = false;	

	HRESULT hr = this->gDevice->CreateRasterizerState(&rsDesc, &rsState);
	if (FAILED(hr))
	{

	}

	this->gDeviceContext->RSSetState(rsState);

	rsState->Release();
}

void TableObject::clearPipeline(ShaderType shaderType)
{
	ID3D11ShaderResourceView* nullSRV[3] = { nullptr, nullptr, nullptr };
	ID3D11SamplerState* nullSS[] = { nullptr, nullptr };
	ID3D11RasterizerState* nullR = nullptr;
	ID3D11Buffer* nullBuffer[1] = { nullptr };
	ID3D11Buffer* nullBuffer2[2] = { nullptr, nullptr };

	this->gDeviceContext->VSSetConstantBuffers(0, 1, nullBuffer);
	this->gDeviceContext->GSSetConstantBuffers(0, 2, nullBuffer2);
	this->gDeviceContext->RSSetState(nullR);	
	this->gDeviceContext->PSSetSamplers(0, 2, nullSS);	
	this->gDeviceContext->PSSetConstantBuffers(0, 1, nullBuffer);	
	this->gDeviceContext->PSSetShaderResources(0, 3, nullSRV);
}
void TableObject::createBoundingBox()
{
	//for (int i = 0; i < this->vertex.size(); i++)
	//{
	//	this->vertex[i].pos = XMFLOAT3(XMVector3Transform(XMLoadFloat3(&this->vertex[i].pos), this->world).x,;
	//}
	this->mOBB.CreateFromPoints(this->mOBB, this->vertex.size(),
		&this->vertex[0].pos,
		sizeof(Vertex));

	XMMATRIX transform = XMMatrixTranslation(-5.0f, 0.0f, 20.0f); // -5.0, 0.0, 20
	XMMATRIX rotation = XMMatrixRotationY(90.3f);

	this->mOBB.Transform(this->mOBB, this->scaling);
	this->mOBB.Transform(this->mOBB, transform);
	this->mOBB.Transform(this->mOBB, rotation);
	
	//XMFLOAT3* xmf = new XMFLOAT3[8];
	//this->mOBB.GetCorners(xmf);	
	//this->boundingBoxVertices.resize(36);

	//this->boundingBoxVertices[0].pos = xmf[0]; // Top left
	//this->boundingBoxVertices[1].pos = xmf[1];  // Top right
	//this->boundingBoxVertices[2].pos = xmf[3]; // Bottom left
	//this->boundingBoxVertices[3].pos = xmf[3]; // Bottom left
	//this->boundingBoxVertices[4].pos = xmf[1]; // Top right
	//this->boundingBoxVertices[5].pos = xmf[2]; // Bottom right	

	//this->boundingBoxVertices[6].pos = xmf[1]; // Top left
	//this->boundingBoxVertices[7].pos = xmf[5];  // Top right
	//this->boundingBoxVertices[8].pos = xmf[2]; // Bottom left
	//this->boundingBoxVertices[9].pos = xmf[2]; // Bottom left
	//this->boundingBoxVertices[10].pos = xmf[5]; // Top right
	//this->boundingBoxVertices[11].pos = xmf[6]; // Bottom right	

	//this->boundingBoxVertices[12].pos = xmf[5]; // Top left
	//this->boundingBoxVertices[13].pos = xmf[4];  // Top right
	//this->boundingBoxVertices[14].pos = xmf[6]; // Bottom left
	//this->boundingBoxVertices[15].pos = xmf[6]; // Bottom left
	//this->boundingBoxVertices[16].pos = xmf[4]; // Top right
	//this->boundingBoxVertices[17].pos = xmf[7]; // Bottom right	

	//this->boundingBoxVertices[18].pos = xmf[4]; // Top left
	//this->boundingBoxVertices[19].pos = xmf[0];  // Top right
	//this->boundingBoxVertices[20].pos = xmf[7]; // Bottom left
	//this->boundingBoxVertices[21].pos = xmf[7]; // Bottom left
	//this->boundingBoxVertices[22].pos = xmf[0]; // Top right
	//this->boundingBoxVertices[23].pos = xmf[3]; // Bottom right

	//this->boundingBoxVertices[24].pos = xmf[5]; // Top left
	//this->boundingBoxVertices[25].pos = xmf[1];  // Top right
	//this->boundingBoxVertices[26].pos = xmf[4]; // Bottom left
	//this->boundingBoxVertices[27].pos = xmf[4]; // Bottom left
	//this->boundingBoxVertices[28].pos = xmf[1]; // Top right
	//this->boundingBoxVertices[29].pos = xmf[0]; // Bottom right	

	//this->boundingBoxVertices[30].pos = xmf[7]; // Top left
	//this->boundingBoxVertices[31].pos = xmf[3];  // Top right
	//this->boundingBoxVertices[32].pos = xmf[6]; // Bottom left
	//this->boundingBoxVertices[33].pos = xmf[6]; // Bottom left
	//this->boundingBoxVertices[34].pos = xmf[3]; // Top right
	//this->boundingBoxVertices[35].pos = xmf[2]; // Bottom right	

	//D3D11_BUFFER_DESC boxDesc;
	//memset(&boxDesc, 0, sizeof(boxDesc));
	//boxDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//boxDesc.ByteWidth = sizeof(Vertex) * 36;
	//boxDesc.CPUAccessFlags = 0;
	//boxDesc.MiscFlags = 0;
	//boxDesc.StructureByteStride = 0;
	//boxDesc.Usage = D3D11_USAGE_DEFAULT;

	//D3D11_SUBRESOURCE_DATA boxData;
	//boxData.pSysMem = &this->boundingBoxVertices[0];
	//boxData.SysMemPitch = 0;
	//boxData.SysMemSlicePitch = 0;

	//this->gDevice->CreateBuffer(&boxDesc, &boxData, &this->gBoundingVertexBuffer);

	//delete xmf;
}

XMMATRIX* TableObject::getWorld() 
{
	return &this->world;
}

XMMATRIX* TableObject::getWorldViewProj() 
{
	return &this->worldViewProj;
}

XMMATRIX* TableObject::getSunWorldViewProj() 
{
	return &this->sunWorldViewProj;
}

void TableObject::update()
{
	if (this->camera->updateAndGetStatic() == false)
	{
		this->updateMatrices();
	}
	else
	{
		this->updateMatricesForStaticCamera();
	}

	this->gDeviceContext->Map(this->gCameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &this->dataPtr);
	this->gCamData = (CAMERA_BUFFER*)this->dataPtr.pData;
	this->gCamData->camPos = this->camera->getPositionXM();
	this->gDeviceContext->Unmap(this->gCameraBuffer, 0);
	
	//this->gBoundingVertexBuffer->Release();
	this->createBoundingBox();
}

void TableObject::renderShadowPass()
{	
	this->setPipelineShadowPass();	
	this->gDeviceContext->Draw(this->nrOfVertices, 0);
	this->clearPipeline(ShaderType::PositionTexture);
}

void TableObject::render(ID3D11RenderTargetView* inRTV, ID3D11DepthStencilView* inDSV,
	float inDt, ShaderType shaderType)
{
	this->setPipeline(inRTV, inDSV);
	this->gDeviceContext->Draw(this->nrOfVertices, 0);
	this->clearPipeline(shaderType);

	/*this->setPipelineDebug(inRTV, inDSV);
	this->gDeviceContext->Draw(36, 0);
	this->clearPipeline(shaderType);*/
}

void TableObject::render(ID3D11RenderTargetView* inRTV[], ID3D11DepthStencilView* inDSV,
	float inDt, ShaderType shaderType)
{
	this->setPipeline(inRTV, inDSV);
	this->gDeviceContext->Draw(this->nrOfVertices, 0);
	this->clearPipeline(shaderType);
}

int TableObject::getId() const
{
	return this->id;
}

BoundingBox TableObject::getBoundingBox() const
{
	return this->mOBB;
}

bool TableObject::getDraw() const
{
	return this->draw;
}

void TableObject::setId(int id)
{
	this->id = id;
}

void TableObject::setDraw(bool value)
{
	this->draw = value;
}

