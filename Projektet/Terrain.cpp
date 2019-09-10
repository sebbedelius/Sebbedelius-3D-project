#include "Terrain.h"
#include "ObjLoader.h"
#include "TextureLoader.h"
#include <fstream>

Terrain::Terrain(ID3D11Device* inDevice, ID3D11DeviceContext* inDeviceContext,
	CameraClass* inCamera, StaticCamera* inStaticCamera, Sun* inSun, ShadowMap* inShadowMap, ShaderType shaderType)
{
	this->gDevice = inDevice;
	this->gDeviceContext = inDeviceContext;
	this->camera = inCamera;	
	this->staticCamera = inStaticCamera;
	this->mSun = inSun;
	this->mShadowMap = inShadowMap;

	this->terrainHeight = 1000;
	this->terrainWidth = 1000;
	this->gridDepth = 100.0f;
	this->gridWidth = 100.0f;
	this->scale = 20.0f;

	this->nrOfFaces = 0;
	this->nrOfIndices = 0;
	this->nrOfVertices = 0; 

	this->psConstData = { XMFLOAT3(0.5f, 0.5f, 0.5f), 5.0f };
	
	this->loadHeightmap();	
	this->smooth();
	this->createGrid(this->gridWidth, this->gridDepth);
	this->colorTerrain();
	this->createVertexAndIndexBuffers();	

	this->shader = new Shader();
	this->shadowShader = new Shader();

	this->createConstantBuffers();
	
	if (inCamera->updateAndGetStatic() == false)
	{
		this->createMatrices();
	}
	else
	{
		this->createMatricesForStaticCamera();
	}	

	this->createSamplerState();

	this->shader->createShader(this->gDevice, shaderType);		
	this->shadowShader->createShader(this->gDevice, ShaderType::PositionColorShadowPass);
}

Terrain::~Terrain()
{
	this->gShadowConstantBuffer->Release();		
	this->gVSShadowConstantBuffer->Release();	
	this->gPSConstantBuffer->Release();
	this->gVertexBuffer->Release();	
	this->gIndexBuffer->Release();	
	this->gDbgConstantBuffer->Release();	

	this->gSamplerState->Release();

	delete this->shader;	
	delete this->shadowShader;
}

void Terrain::createGrid(float width, float depth)
{		
	float halfWidth, halfDepth;
	float x, z;

	this->nrOfVertices = this->terrainHeight * this->terrainWidth;
	this->nrOfFaces = (this->terrainHeight - 1) * (this->terrainWidth - 1) * 2;
	this->nrOfIndices = this->nrOfFaces * 3;  // A face is a triangle. 3 indices per triangle. 

	this->triangleVertices.resize(this->nrOfVertices);

	halfWidth = 0.5f * width;
	halfDepth = 0.5f * depth;

	this->cellSpaceX = width / (this->terrainWidth - 1);
	this->cellSpaceZ = depth / (this->terrainHeight - 1);

	//Begins in the top left with (-halfwidth, halfDepth). The middle of the grid is (0, 0). 
	for (int i = 0; i < this->terrainHeight; i++)
	{
		z = halfDepth - i * this->cellSpaceZ;

		for (int j = 0; j < this->terrainWidth; j++)
		{
			x = -halfWidth + j * this->cellSpaceX;
			triangleVertices[i * this->terrainWidth + j].x = x;
			triangleVertices[i * this->terrainWidth + j].y = this->heights[i * this->terrainWidth + j];
			triangleVertices[i * this->terrainWidth + j].z = z;			
		}
	}

	this->indices.resize(this->nrOfIndices);

	//Compute indices for each quad in the grid. (Two triangles, line list)
	int k = 0;
	for (int i = 0; i < this->terrainHeight - 1; i++)
	{
		for (int j = 0; j < this->terrainWidth - 1; j++)
		{
			indices[k] = i * this->terrainWidth + j; // Top left
			indices[k + 1] = i * this->terrainWidth + j + 1; // Top right
			indices[k + 2] = (i + 1) * this->terrainWidth + j; // Bottom left
			indices[k + 3] = (i + 1) * this->terrainWidth + j; // Bottom left
			indices[k + 4] = i * this->terrainWidth + j + 1; // Top right
			indices[k + 5] = (i + 1) * this->terrainWidth + j + 1; // Bottom right
			k += 6; // Next quad.
		}
	}	
}

void Terrain::loadHeightmap()
{
	std::vector<unsigned char>in(this->terrainWidth * this->terrainHeight);

	std::ifstream inFile;
	inFile.open(".//Images//Heightmap.raw",std::ios_base::binary);

	if (inFile)
	{
		inFile.read((char*)&in[0], (std::streamsize)in.size());

		inFile.close();
	}

	this->heights.resize(this->terrainHeight * this->terrainWidth, 0);

	for (int i = 0; i < this->terrainHeight * this->terrainWidth; i++)
	{			
		this->heights[i] = (in[i] / 255.0f) * this->scale;			
	}
}

void Terrain::createVertexAndIndexBuffers()
{
	// Describe the Vertex Buffer
	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	// what type of buffer will this be?
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	// what type of usage (press F1, read the docs)
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	// how big in bytes each element in the buffer is.
	bufferDesc.ByteWidth = sizeof(TriangleVertexColor) * this->nrOfVertices;
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

	//Describe the Index Buffer
	D3D11_BUFFER_DESC indexBufferDesc;
	memset(&indexBufferDesc, 0, sizeof(indexBufferDesc));
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	// how big in bytes each element in the buffer is.
	indexBufferDesc.ByteWidth = sizeof(unsigned int) * this->nrOfIndices;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = &this->indices[0]; 
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	//Create the index buffer
	this->gDevice->CreateBuffer(&indexBufferDesc, &indexData, &this->gIndexBuffer);
}

void Terrain::smooth()
{
	std::vector<float> smoothenedHeights;
	smoothenedHeights.resize(this->heights.size());

	for (int i = 0; i < this->terrainHeight; i++)
	{
		for (int j = 0; j < this->terrainWidth; j++)
		{
			smoothenedHeights[i * this->terrainWidth + j] = this->average(i, j);
		}
	}

	this->heights = smoothenedHeights;
}

float Terrain::average(int i, int j)
{	
	float avg = 0.0f;
	float nrOfNeighbours = 0.0f;

	for (int m = i - 1; m <= i + 1; m++)
	{
		for (int n = j - 1; n <= j + 1; n++)
		{
			if (inBounds(m, n))
			{
				avg += this->heights[m * this->terrainWidth + n];
				nrOfNeighbours++;
			}
		}
	}

	avg /= nrOfNeighbours;

	return avg;
}

bool Terrain::inBounds(int i, int j)
{
	bool inBounds = false;

	//True if the ij indices are in the grid
	if (i >= 0 && i < (int)this->terrainHeight
		&& j >= 0 && j < (int)this->terrainWidth)
	{
		inBounds = true;
	}		

	return inBounds;
}

void Terrain::colorTerrain()
{
	for (int i = 0; i < this->nrOfVertices; i++)
	{
		if (this->triangleVertices[i].y < 3.0f)
		{
			this->triangleVertices[i].r = 0.25f;
			this->triangleVertices[i].g = 0.20f;
			this->triangleVertices[i].b = 0.10f;
		}
		else if (this->triangleVertices[i].y < 8.0f)
		{
			this->triangleVertices[i].r = 0.1f;
			this->triangleVertices[i].g = 0.48f;
			this->triangleVertices[i].b = 0.19f;
		}
		else if (this->triangleVertices[i].y < 12.0f)
		{
			this->triangleVertices[i].r = 0.25f;
			this->triangleVertices[i].g = 0.25f;
			this->triangleVertices[i].b = 0.25f;
		}
		else
		{
			this->triangleVertices[i].r = 0.7f;
			this->triangleVertices[i].g = 0.7f;
			this->triangleVertices[i].b = 0.7f;
		}
	}
	
}

void Terrain::createMatrices()
{
	this->scaling = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	this->world = XMMatrixMultiply(this->scaling, XMMatrixRotationY(0));

	this->worldViewProj = XMMatrixMultiply(this->world, this->camera->getView());
	this->worldViewProj = XMMatrixMultiply(this->worldViewProj, this->camera->getProj());
	this->worldViewProj = XMMatrixTranspose(this->worldViewProj);		

	this->sunWorldViewProj = XMMatrixMultiply(this->world, this->mSun->getViewXM());
	this->sunWorldViewProj = XMMatrixMultiply(this->sunWorldViewProj, this->mSun->getProjXM());
	this->sunWorldViewProj = XMMatrixTranspose(this->sunWorldViewProj);

	this->gDeviceContext->Map(this->gShadowConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &dataPtr);
	this->sConstData = (SHADOW_GS_CONSTANT_BUFFER*)dataPtr.pData;
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

	this->gDeviceContext->Map(this->gDbgConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &this->dataPtr);
	this->gDbgConstData = (GS_CONSTANT_BUFFER*)this->dataPtr.pData;
	this->gDbgConstData->theWorld = XMMatrixTranspose(this->world);
	this->gDbgConstData->theWorldViewProj = this->worldViewProj;
	this->gDeviceContext->Unmap(this->gDbgConstantBuffer, 0);
}

void Terrain::createMatricesForStaticCamera()
{
	this->scaling = XMMatrixScaling(1.0f, 1.0f, 1.0f);	
	this->world = XMMatrixMultiply(this->scaling, XMMatrixRotationY(0));

	this->worldViewProj = XMMatrixMultiply(this->world, this->staticCamera->getView());
	this->worldViewProj = XMMatrixMultiply(this->worldViewProj, this->staticCamera->getProj());
	this->worldViewProj = XMMatrixTranspose(this->worldViewProj);

	this->sunWorldViewProj = XMMatrixMultiply(this->world, this->mSun->getViewXM());
	this->sunWorldViewProj = XMMatrixMultiply(this->sunWorldViewProj, this->mSun->getProjXM());
	this->sunWorldViewProj = XMMatrixTranspose(this->sunWorldViewProj);

	this->gDeviceContext->Map(this->gShadowConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &dataPtr);
	this->sConstData = (SHADOW_GS_CONSTANT_BUFFER*)dataPtr.pData;
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

	this->gDeviceContext->Map(this->gDbgConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &this->dataPtr);
	this->gDbgConstData = (GS_CONSTANT_BUFFER*)this->dataPtr.pData;
	this->gDbgConstData->theWorld = XMMatrixTranspose(this->world);
	this->gDbgConstData->theWorldViewProj = this->worldViewProj;
	this->gDeviceContext->Unmap(this->gDbgConstantBuffer, 0);
}

void Terrain::updateMatrices()
{
	this->worldViewProj = XMMatrixMultiply(this->world, this->camera->getView());
	this->worldViewProj = XMMatrixMultiply(this->worldViewProj, this->camera->getProj());
	this->worldViewProj = XMMatrixTranspose(this->worldViewProj);

	this->gDeviceContext->Map(this->gShadowConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &dataPtr);
	this->sConstData = (SHADOW_GS_CONSTANT_BUFFER*)dataPtr.pData;
	this->sConstData->theWorld = XMMatrixTranspose(this->world);
	this->sConstData->theWorldViewProj = this->worldViewProj;
	this->sConstData->theSunWorldViewProj = this->sunWorldViewProj;
	this->sConstData->theView = XMMatrixTranspose(this->camera->getView());
	this->gDeviceContext->Unmap(this->gShadowConstantBuffer, 0);

	this->gDeviceContext->Map(this->gDbgConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &this->dataPtr);
	this->gDbgConstData = (GS_CONSTANT_BUFFER*)this->dataPtr.pData;
	this->gDbgConstData->theWorld = XMMatrixTranspose(this->world);
	this->gDbgConstData->theWorldViewProj = this->worldViewProj;
	this->gDeviceContext->Unmap(this->gDbgConstantBuffer, 0);
}

void Terrain::updateMatricesForStaticCamera()
{
	this->worldViewProj = XMMatrixMultiply(this->world, this->staticCamera->getView());
	this->worldViewProj = XMMatrixMultiply(this->worldViewProj, this->staticCamera->getProj());
	this->worldViewProj = XMMatrixTranspose(this->worldViewProj);

	this->gDeviceContext->Map(this->gShadowConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &dataPtr);
	this->sConstData = (SHADOW_GS_CONSTANT_BUFFER*)dataPtr.pData;
	this->sConstData->theWorld = XMMatrixTranspose(this->world);
	this->sConstData->theWorldViewProj = this->worldViewProj;
	this->sConstData->theSunWorldViewProj = this->sunWorldViewProj;
	this->sConstData->theView = XMMatrixTranspose(this->camera->getView());
	this->gDeviceContext->Unmap(this->gShadowConstantBuffer, 0);

	this->gDeviceContext->Map(this->gDbgConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &this->dataPtr);
	this->gDbgConstData = (GS_CONSTANT_BUFFER*)this->dataPtr.pData;
	this->gDbgConstData->theWorld = XMMatrixTranspose(this->world);
	this->gDbgConstData->theWorldViewProj = this->worldViewProj;
	this->gDeviceContext->Unmap(this->gDbgConstantBuffer, 0);
}

HRESULT Terrain::createConstantBuffers()
{
	HRESULT hr = 0;

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

HRESULT Terrain::createSamplerState()
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

void Terrain::setPipelineShadowPass()
{
	this->gDeviceContext->VSSetShader(this->shadowShader->getVertexShader(), nullptr, 0);
	this->gDeviceContext->HSSetShader(nullptr, nullptr, 0);
	this->gDeviceContext->DSSetShader(nullptr, nullptr, 0);
	this->gDeviceContext->GSSetShader(nullptr, nullptr, 0);
	this->gDeviceContext->PSSetShader(nullptr, nullptr, 0);

	UINT32 vertexSize = sizeof(TriangleVertexColor);
	UINT32 offset = 0;

	// specify which vertex buffer to use next.
	this->gDeviceContext->IASetVertexBuffers(0, 1, &this->gVertexBuffer, &vertexSize, &offset);

	//specify which index buffer to use next
	this->gDeviceContext->IASetIndexBuffer(this->gIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	this->gDeviceContext->VSSetConstantBuffers(0, 1, &this->gVSShadowConstantBuffer);

	// specify the topology to use when drawing
	this->gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	this->gDeviceContext->IASetInputLayout(this->shadowShader->getVertexLayout());

	this->gDeviceContext->RSSetState(this->mShadowMap->getRasterizerState());
}

void Terrain::setPipeline(ID3D11RenderTargetView * inRTV, ID3D11DepthStencilView * inDSV)
{
	// specifying NULL or nullptr we are disabling that stage
	// in the pipeline
	this->gDeviceContext->VSSetShader(this->shader->getVertexShader(), nullptr, 0);
	this->gDeviceContext->HSSetShader(nullptr, nullptr, 0);
	this->gDeviceContext->DSSetShader(nullptr, nullptr, 0);
	this->gDeviceContext->GSSetShader(this->shader->getGeometryShader(), nullptr, 0);
	this->gDeviceContext->PSSetShader(this->shader->getPixelShader(), nullptr, 0);

	UINT32 vertexSize = sizeof(TriangleVertexColor);
	UINT32 offset = 0;
	// specify which vertex buffer to use next.
	this->gDeviceContext->IASetVertexBuffers(0, 1, &this->gVertexBuffer, &vertexSize, &offset);

	//specify which index buffer to use next
	this->gDeviceContext->IASetIndexBuffer(this->gIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//Set render targets (textures)
	this->gDeviceContext->OMSetRenderTargets(1,
		&inRTV,
		inDSV);

	//Bind constant buffer to the geometry shader
	this->gDeviceContext->GSSetConstantBuffers(0, 1, &this->gShadowConstantBuffer);

	// Bind shadow map texture to pixel shader
	ID3D11ShaderResourceView* shadowTex = this->mShadowMap->getDepthMapSRV();
	this->gDeviceContext->PSSetShaderResources(0, 1, &shadowTex);

	ID3D11SamplerState* samShadow = this->mShadowMap->getSamplerState();
	//Set sampler state
	this->gDeviceContext->PSSetSamplers(0, 1, &this->gSamplerState);
	this->gDeviceContext->PSSetSamplers(1, 1, &samShadow);
	
	// specify the topology to use when drawing
	this->gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	this->gDeviceContext->IASetInputLayout(this->shader->getVertexLayout());
}

void Terrain::setPipeline(ID3D11RenderTargetView * inRTV[], ID3D11DepthStencilView * inDSV)
{
}

void Terrain::clearPipeline()
{
	ID3D11ShaderResourceView* nullSRV[3] = { nullptr, nullptr, nullptr };
	ID3D11Buffer* nullBuffer = nullptr;
	ID3D11Buffer* nullBuffer2[2] = { nullptr, nullptr };
	ID3D11RasterizerState* nullR = nullptr;

	this->gDeviceContext->IASetIndexBuffer(nullBuffer, DXGI_FORMAT_R32_UINT, 0);
	this->gDeviceContext->VSSetConstantBuffers(0, 1, &nullBuffer);
	this->gDeviceContext->GSSetConstantBuffers(0, 2, nullBuffer2);
	this->gDeviceContext->RSSetState(nullR);
	this->gDeviceContext->PSSetShaderResources(0, 3, nullSRV);	
}

XMMATRIX Terrain::getWorld() const
{
	return this->world;
}

XMMATRIX Terrain::getWorldViewProj() const
{
	return this->worldViewProj;
}

XMMATRIX Terrain::getSunWorldViewProj() const
{
	return this->sunWorldViewProj;
}

int Terrain::getId() const
{
	return this->id;
}

float Terrain::getHeight(float x, float z) const
{
	float height = 0.0f;

	//Convert coordinate system to origin in the top left corner of the heightmap
	float c = (x + 0.5f * this->gridWidth) / this->cellSpaceX;
	float d = (z - 0.5f * this->gridDepth) / -this->cellSpaceZ;

	//Get the row and column we are in
	int row = (int)floorf(d);
	int col = (int)floorf(c);

	//Outside of the terrain
	if (row < 0 || row > this->terrainHeight - 2
		|| col  < 0 || col > this->terrainWidth - 2)
	{
		if (col < 0)
		{
			height = West; // Out of bouns on the west side
		}
		else if (col > this->terrainWidth - 2) // Out of bouns on the east side
		{
			height = East;
		}
		else if (row < 0) // Out of bouns on the north side
		{
			height = North;
		}
		else if (row > this->terrainHeight - 2) // Out of bouns on the south side
		{
			height = South;
		}
	}
	else
	{
		float A = this->heights[row * this->terrainWidth + col]; //Top left vertex's height of the square we are in
		float B = this->heights[row * this->terrainWidth + col + 1]; //Top right
		float C = this->heights[(row + 1) * this->terrainWidth + col]; // Bottom left
		float D = this->heights[(row + 1) * this->terrainWidth + col + 1]; //Bottom right

		//Where we are relative to the square
		float s = c - (float)col;
		float t = d - (float)row;

		//Check which triangle we are in the square. Upper or lower triangle.
		if (s + t <= 1.0f)
		{
			float uy = B - A;
			float vy = C - A;

			height = A + s * uy + t * vy;
		}
		else
		{
			float uy = C - D;
			float vy = B - D;

			height = D + (1.0f - s) * uy + (1.0f) * vy;
		}
	}

	return height;
}

BoundingBox Terrain::getBoundingBox() const
{
	return this->mBB;
}

int Terrain::getTerrainWidth() const
{
	return this->terrainWidth;
}

int Terrain::getTerrainHeight() const
{
	return this->terrainHeight;
}

float Terrain::getGridWidth() const
{
	return this->gridWidth;
}

float Terrain::getGridDepth() const
{
	return this->gridDepth;
}

bool Terrain::getDraw() const
{
	return this->draw;
}

void Terrain::setPipelineDebug()
{
	this->gDeviceContext->VSSetConstantBuffers(0, 1, &this->gDbgConstantBuffer);
}

void Terrain::setId(int id)
{
	this->id = id;
}

void Terrain::setBoundingBox(BoundingBox obb)
{
	this->mBB = obb;
}

void Terrain::setDraw(bool value)
{
	this->draw = value;
}

void Terrain::update()
{
	if (this->camera->updateAndGetStatic() == false)
	{
		this->updateMatrices();
	}
	else
	{
		this->updateMatricesForStaticCamera();
	}
}

void Terrain::renderShadowPass()
{
	this->setPipelineShadowPass();
	this->gDeviceContext->DrawIndexed(this->nrOfIndices, 0, 0);
	this->clearPipeline();
}

void Terrain::render(ID3D11RenderTargetView* inRTV, ID3D11DepthStencilView* inDSV)
{
	this->setPipeline(inRTV, inDSV);
	this->gDeviceContext->DrawIndexed(this->nrOfIndices, 0, 0);
	this->clearPipeline();
}

void Terrain::render(ID3D11RenderTargetView* inRTV[], ID3D11DepthStencilView* inDSV)
{
	this->setPipeline(inRTV, inDSV);
	this->gDeviceContext->DrawIndexed(this->nrOfIndices, 0, 0);
	this->clearPipeline();
}

void Terrain::dbgRender()
{
	this->setPipelineDebug();
	this->gDeviceContext->Draw(36, 0);	
	//this->clearPipeline();
}
