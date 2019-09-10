#include "Scene.h"

void Scene::checkKeys(float dt)
{
	if (this->mCamera->updateAndGetNoclip() == On || this->renderType == DeferredRendering)
	{
		this->mCamera->checkKeysFly(dt);
	}
	else //Terrain walking
	{
		float height = this->terrain->getHeight(this->mCamera->getPosition().x, this->mCamera->getPosition().z);
		if (height < 0.0f)
		{
			float x = this->mCamera->getPosition().x;
			float z = this->mCamera->getPosition().z;

			//Push player back into the terrain
			switch ((int)height)
			{
			case West:
				this->mCamera->setPositionX(x + 0.5f);
				break;
			case East:
				this->mCamera->setPositionX(x - 0.5f);
				break;
			case North:
				this->mCamera->setPositionZ(z - 0.5f);
				break;
			case South:
				this->mCamera->setPositionZ(z + 0.5f);
				break;
			}		

			height = this->terrain->getHeight(this->mCamera->getPosition().x, this->mCamera->getPosition().z);
		}
		
		height += 5.0f;	

		this->mCamera->checkKeysTerrainWalk(dt);
		this->mCamera->setHeight(height);
	}	
}

BoundingBox Scene::createBoundingBox(float widthLeft, float widthRight, float depthClose, float depthFar)
{
	BoundingBox tempBB;
	XMVECTOR min = XMVectorSet(widthLeft, 0.0f, depthClose, 1.0f);
	XMVECTOR max = XMVectorSet(widthRight, 20.0f, depthFar, 1.0f);
	tempBB.CreateFromPoints(tempBB, min, max);	

	return tempBB;
}

void Scene::createChildren(BoxNode * parent, float middleWidth, float middleDepth, float parentWidth, float parentDepth)
{	
	BoundingBox bb;
	int nrOfLevels = 4;
	int whenToStop = nrOfLevels - 2;
	this->currentLevel++;

	parent->initializeChildren(this->idCount + 1);	
	bb = createBoundingBox(middleWidth - (0.5f * parentWidth), middleWidth, middleDepth, middleDepth + (0.5f * parentDepth));
	parent->children[0]->mBB = bb;
	bb = createBoundingBox(middleWidth, middleWidth + (0.5f * parentWidth), middleDepth, middleDepth + (0.5f * parentDepth));
	parent->children[1]->mBB = bb;
	bb = createBoundingBox(middleWidth - (0.5f * parentWidth), middleWidth, middleDepth - (0.5f * parentDepth), middleDepth);
	parent->children[2]->mBB = bb;
	bb = createBoundingBox(middleWidth, middleWidth + (0.5f * parentWidth), middleDepth - (0.5f * parentDepth), middleDepth);
	parent->children[3]->mBB = bb;
	
	//Nr of children in this case is nr of children in a row, for example root->children[0]->children[0]->children[0] = 3 children. 
	//Then reset
	this->nrOfChildren++;
	// 4 levels, counting the root node as a level. Change the value to the right to change nr of levels
	if (this->nrOfChildren <= whenToStop) 
	{
		//Middle works for both width and depth as long as it's a square
		this->createChildren(parent->children[0], middleWidth - (0.25f * parentWidth), middleDepth + (0.25f * parentDepth), 0.5f * parentWidth, 0.5f * parentDepth);
		this->currentLevel--;
		this->nrOfChildren = currentLevel - 1;
		this->createChildren(parent->children[1], middleWidth + (0.25f * parentWidth), middleDepth + (0.25 * parentDepth), 0.5f * parentWidth, 0.5f * parentDepth);
		this->currentLevel--;
		this->nrOfChildren = currentLevel - 1;
		this->createChildren(parent->children[2], middleWidth - (0.25f * parentWidth), middleDepth - (0.25 * parentDepth), 0.5f * parentWidth, 0.5f * parentDepth);
		this->currentLevel--;
		this->nrOfChildren = currentLevel - 1;
		this->createChildren(parent->children[3], middleWidth + (0.25f * parentWidth), middleDepth - (0.25 * parentDepth), 0.5f * parentWidth, 0.5f * parentDepth);
		this->currentLevel--;
	}	
}

void Scene::sortGeometry(BoxNode * parent, int objectId)
{	
	if (objectId == this->table->getId())
	{
		for (int i = 0; i < 4; i++)
		{
			if (parent->children[i] != nullptr)
			{
				if (parent->children[i]->mBB.Intersects(this->table->getBoundingBox()))
				{						
					parent->children[i]->addPolygon(this->table->getId());					
					this->sortGeometry(parent->children[i], objectId);
				}
			}			
		}
	}
	else if (objectId == this->terrain->getId())
	{
		for (int i = 0; i < 4; i++)
		{
			if (parent->children[i] != nullptr)
			{				
				if (parent->children[i]->mBB.Intersects(this->terrain->getBoundingBox()))
				{
					parent->children[i]->addPolygon(this->terrain->getId());
					this->sortGeometry(parent->children[i], objectId);
				}
			}			
		}
	}
	else if (objectId == this->box->getId())
	{
		for (int i = 0; i < 4; i++)
		{
			if (parent->children[i] != nullptr)
			{
				if (parent->children[i]->mBB.Intersects(this->box->getBoundingBox()))
				{
					parent->children[i]->addPolygon(this->box->getId());
					this->sortGeometry(parent->children[i], objectId);
				}
			}			
		}
	}
	else if (objectId == this->mWaves->getId())
	{
		for (int i = 0; i < 4; i++)
		{
			if (parent->children[i] != nullptr)
			{
				if (parent->children[i]->mBB.Intersects(this->mWaves->getBoundingBox()))
				{
					parent->children[i]->addPolygon(this->mWaves->getId());
					this->sortGeometry(parent->children[i], objectId);
				}
			}
		}
	}
}

void Scene::clearQuadTree(BoxNode* node)
{	
	node->setNrOfIds(0);
	if (node->children[0] == nullptr)
	{
		for (int j = 0; j < this->idCount + 1; j++)
		{
			node->polygonList[j] = -1;
		}
	}
	else
	{
		for (int i = 0; i < 4; i++)
		{
			this->clearQuadTree(node->children[i]);
		}		
	}	
}

void Scene::createFrustumAndTest()
{
	BoundingFrustum bf;
	bf.CreateFromMatrix(bf, this->mCamera->getProj());

	this->testFrustumAgainstTree(bf, this->rootNode);
	
}

void Scene::testFrustumAgainstTree(BoundingFrustum bf, BoxNode * parent)
{
	BoundingBox bbTest = parent->mBB;
	bbTest.Transform(bbTest, this->mCamera->getView());

	if (bf.Intersects(bbTest))
	{
		if (parent->children[0] == nullptr) //This means its the last node
		{
			this->checkPolygonsInNode(bf, parent);
		}
		else
		{
			for (int i = 0; i < 4; i++)
			{
				this->testFrustumAgainstTree(bf, parent->children[i]);
			}			
		}
	}
}

void Scene::checkPolygonsInNode(BoundingFrustum bf, BoxNode* node)
{
	BoundingBox bbTest; 
	XMMATRIX view = this->mCamera->getView();

	for (int i = 0; i < node->getNrOfIds(); i++)
	{
		if (node->polygonList[i] == this->table->getId())
		{
			bbTest = this->table->getBoundingBox();
			bbTest.Transform(bbTest, view);

			if (bf.Intersects(bbTest))
				this->table->setDraw(true);
		}
		else if (node->polygonList[i] == this->terrain->getId())
		{
			bbTest = this->terrain->getBoundingBox();
			bbTest.Transform(bbTest, view);

			if (bf.Intersects(bbTest))
				this->terrain->setDraw(true);
		}
		else if (node->polygonList[i] == this->box->getId())
		{
			bbTest = this->box->getBoundingBox();
			bbTest.Transform(bbTest, view);

			if (bf.Intersects(bbTest))
				this->box->setDraw(true);
		}
		else if (node->polygonList[i] == this->mWaves->getId())
		{
			bbTest = this->mWaves->getBoundingBox();
			bbTest.Transform(bbTest, view);

			if (bf.Intersects(bbTest))
				this->mWaves->setDraw(true);
		}
	}
}

void Scene::createCameraBuffer(ID3D11Device * inDevice)
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
	camData.pSysMem = &this->gCameraData;
	camData.SysMemPitch = 0;
	camData.SysMemSlicePitch = 0;

	hr = inDevice->CreateBuffer(&camDesc, &camData, &this->gCameraBuffer);
}

void Scene::createWavesConstantBuffer(ID3D11Device * inDevice)
{
	HRESULT hr = 0;

	D3D11_BUFFER_DESC wavesDesc;
	memset(&wavesDesc, 0, sizeof(wavesDesc));
	wavesDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	wavesDesc.ByteWidth = sizeof(SHADOW_GS_CONSTANT_BUFFER);
	wavesDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	wavesDesc.MiscFlags = 0;
	wavesDesc.StructureByteStride = 0;
	wavesDesc.Usage = D3D11_USAGE_DYNAMIC;

	D3D11_SUBRESOURCE_DATA wavesData;
	wavesData.pSysMem = &this->gWavesGSConstData;
	wavesData.SysMemPitch = 0;
	wavesData.SysMemSlicePitch = 0;

	hr = inDevice->CreateBuffer(&wavesDesc, &wavesData, &this->gWavesGSConstBuffer);

	D3D11_BUFFER_DESC wavesVSDesc;
	memset(&wavesVSDesc, 0, sizeof(wavesVSDesc));
	wavesVSDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	wavesVSDesc.ByteWidth = sizeof(VS_WAVES_BUFFER);
	wavesVSDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	wavesVSDesc.MiscFlags = 0;
	wavesVSDesc.StructureByteStride = 0;
	wavesVSDesc.Usage = D3D11_USAGE_DYNAMIC;

	D3D11_SUBRESOURCE_DATA wavesVSData;
	wavesVSData.pSysMem = &this->gWavesVSConstData;
	wavesVSData.SysMemPitch = 0;
	wavesVSData.SysMemSlicePitch = 0;

	hr = inDevice->CreateBuffer(&wavesVSDesc, &wavesVSData, &this->gWavesVSConstBuffer);
}

void Scene::updateWavesMatrices(ID3D11DeviceContext * inDeviceContext)
{
	XMMATRIX world = this->mWaves->getWorld();
	XMMATRIX worldViewProj = XMMatrixMultiplyTranspose(XMMatrixMultiply(world, this->mCamera->getView()), this->mCamera->getProj());
	XMMATRIX sunWVP = XMMatrixMultiplyTranspose(XMMatrixMultiply(world, this->mSun->getViewXM()), this->mSun->getProjXM());

	inDeviceContext->Map(this->gWavesVSConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &this->dataPtr);
	this->gWavesVSConstData = (VS_WAVES_BUFFER*) this->dataPtr.pData;
	this->gWavesVSConstData->theWaterTexTransform = this->waterTexTransform;
	inDeviceContext->Unmap(this->gWavesVSConstBuffer, 0);

	inDeviceContext->Map(this->gWavesGSConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &this->dataPtr);
	this->gWavesGSConstData = (SHADOW_GS_CONSTANT_BUFFER*) this->dataPtr.pData;
	this->gWavesGSConstData->theWorld = world;
	this->gWavesGSConstData->theWorldViewProj = worldViewProj;
	this->gWavesGSConstData->theSunWorldViewProj = sunWVP;
	this->gWavesGSConstData->theView = XMMatrixTranspose(this->mCamera->getView());
	inDeviceContext->Unmap(this->gWavesGSConstBuffer, 0);
}

void Scene::updateWavesMatricesForStaticCamera(ID3D11DeviceContext * inDeviceContext)
{
	XMMATRIX world = this->mWaves->getWorld();
	XMMATRIX worldViewProj = XMMatrixMultiplyTranspose(
		XMMatrixMultiply(world, this->mStaticCamera->getView()), this->mStaticCamera->getProj());
	XMMATRIX sunWVP = XMMatrixMultiplyTranspose(
		XMMatrixMultiply(world, this->mSun->getViewXM()), this->mSun->getProjXM());

	inDeviceContext->Map(this->gWavesVSConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &this->dataPtr);
	this->gWavesVSConstData = (VS_WAVES_BUFFER*)this->dataPtr.pData;
	this->gWavesVSConstData->theWaterTexTransform = this->waterTexTransform;
	inDeviceContext->Unmap(this->gWavesVSConstBuffer, 0);

	inDeviceContext->Map(this->gWavesGSConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &this->dataPtr);
	this->gWavesGSConstData = (SHADOW_GS_CONSTANT_BUFFER*)this->dataPtr.pData;
	this->gWavesGSConstData->theWorld = world;
	this->gWavesGSConstData->theWorldViewProj = worldViewProj;
	this->gWavesGSConstData->theSunWorldViewProj = sunWVP;
	this->gWavesGSConstData->theView = XMMatrixTranspose(this->mCamera->getView());
	inDeviceContext->Unmap(this->gWavesGSConstBuffer, 0);
}

void Scene::createWavesTriangleData(ID3D11Device* inDevice)
{
	HRESULT hr = 0;
	
	TextureLoader* texLoader = new TextureLoader();

	this->nrOfWavesVertices = this->mWaves->getNrOfVertices();	

	D3D11_BUFFER_DESC wavesDesc;
	wavesDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	wavesDesc.ByteWidth = sizeof(TriangleVertexTexXM) * this->nrOfWavesVertices; //För nu
	wavesDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	wavesDesc.MiscFlags = 0;
	wavesDesc.StructureByteStride = 0;
	wavesDesc.Usage = D3D11_USAGE_DYNAMIC;	

	hr = inDevice->CreateBuffer(&wavesDesc, 0, &this->gWavesVertexBuffer);	

	this->nrOfWavesIndices = this->mWaves->getNrOfTriangles() * 3;
	std::vector<unsigned int> indices(this->nrOfWavesIndices);

	int m = this->mWaves->getNrOfRows();
	int n = this->mWaves->getNrOfCols();
	int k = 0;

	for (int i = 0; i < m - 1; i++)
	{
		for (int j = 0; j < n - 1; j++)
		{
			indices[k] = i * n + j;
			indices[k + 1] = i * n + j + 1;
			indices[k + 2] = (i + 1)*n + j;
			indices[k + 3] = (i + 1)*n + j;
			indices[k + 4] = i * n + j + 1;
			indices[k + 5] = (i + 1)*n + j + 1;

			k += 6; //Next quad
		}
	}

	D3D11_BUFFER_DESC indexDesc;
	indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexDesc.ByteWidth = sizeof(unsigned int) * this->nrOfWavesIndices;
	indexDesc.CPUAccessFlags = 0;
	indexDesc.MiscFlags = 0;
	indexDesc.StructureByteStride = 0;
	indexDesc.Usage = D3D11_USAGE_IMMUTABLE;

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = &indices[0];
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	hr = inDevice->CreateBuffer(&indexDesc, &indexData, &this->gWavesIndexBuffer);

	//Load water texture
	texLoader->loadFromFile(inDevice, L".//Images//water2.dds");
	this->waterTex = texLoader->getTextureRV();

	delete texLoader;
}

void Scene::updateWaves(ID3D11DeviceContext* inDeviceContext, float dt)
{
	auto end = std::chrono::steady_clock::now();
	float timeSinceLastWave = std::chrono::duration<float>(end - this->start).count();

	if (timeSinceLastWave >= 0.25f)
	{
		int i = 5 + rand() % (this->mWaves->getNrOfRows() - 10);
		int j = 5 + rand() % (this->mWaves->getNrOfCols() - 10);

		float magnitude = (float)(rand()) / (float)RAND_MAX;

		this->mWaves->disturb(i, j, magnitude);

		this->start = end;
	}
	
	this->mWaves->update();
	std::vector<XMFLOAT3> temp = this->mWaves->getCurrentVertices();
	
	inDeviceContext->Map(this->gWavesVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &this->dataPtr);
	TriangleVertexTexXM* v = reinterpret_cast<TriangleVertexTexXM*>(this->dataPtr.pData);
	for (int i = 0; i < this->mWaves->getNrOfVertices(); i++)
	{
		v[i].pos = temp[i];

		//Derive texcoords in [0, 1] from position
		v[i].u = 0.5f + temp[i].x / this->mWaves->width();
		v[i].v = 0.5f - temp[i].z / this->mWaves->depth();
	}
	inDeviceContext->Unmap(this->gWavesVertexBuffer, 0);		

	//Animate water texture coordinates

	XMMATRIX wavesScale = XMMatrixScaling(5.0f, 5.0f, 0.0f);

	this->waterTexOffsetY += 0.05f*dt;
	this->waterTexOffsetX += 0.1*dt;

	XMMATRIX wavesOffset = XMMatrixTranslation(this->waterTexOffsetX, this->waterTexOffsetY, 0.0f);

	this->waterTexTransform = XMMatrixMultiplyTranspose(wavesScale, wavesOffset); //Maybe not transpose?
}

void Scene::createRasterizerState(ID3D11Device * inDevice)
{
	D3D11_RASTERIZER_DESC rsDesc;
	memset(&rsDesc, 0, sizeof(rsDesc));
	rsDesc.FillMode = D3D11_FILL_WIREFRAME;
	rsDesc.CullMode = D3D11_CULL_NONE;
	rsDesc.DepthClipEnable = false;
	rsDesc.FrontCounterClockwise = false;

	inDevice->CreateRasterizerState(&rsDesc, &this->rsState);	
}

void Scene::createBlendState(ID3D11Device* inDevice)
{
	D3D11_BLEND_DESC blendDesc;
	memset(&blendDesc, 0, sizeof(blendDesc));
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_BLEND_FACTOR;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	inDevice->CreateBlendState(&blendDesc, &this->gBlendState);	
}

void Scene::setPipelineWaves(ID3D11DeviceContext * inDeviceContext, ID3D11RenderTargetView * inRTV, ID3D11DepthStencilView * inDSV)
{
	inDeviceContext->VSSetShader(this->shader->getVertexShader(), nullptr, 0);
	inDeviceContext->HSSetShader(nullptr, nullptr, 0);
	inDeviceContext->DSSetShader(nullptr, nullptr, 0);
	inDeviceContext->GSSetShader(this->shader->getGeometryShader(), nullptr, 0);
	inDeviceContext->PSSetShader(this->shader->getPixelShader(), nullptr, 0);

	UINT32 vertexSize = sizeof(TriangleVertexTexXM);
	UINT32 offset = 0;

	inDeviceContext->IASetVertexBuffers(0, 1, &this->gWavesVertexBuffer, &vertexSize, &offset);
	inDeviceContext->IASetIndexBuffer(this->gWavesIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	inDeviceContext->OMSetRenderTargets(1,
		&inRTV,
		inDSV);

	//Set constant buffers
	inDeviceContext->VSSetConstantBuffers(0, 1, &this->gWavesVSConstBuffer);
	inDeviceContext->GSSetConstantBuffers(0, 1, &this->gWavesGSConstBuffer);
	inDeviceContext->GSSetConstantBuffers(1, 1, &this->gCameraBuffer);
	// specify the topology to use when drawing
	inDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//Bind textures to pixel shader.	
	ID3D11ShaderResourceView* nullSRV = nullptr;
	ID3D11ShaderResourceView* textures[3] = { this->waterTex, nullSRV, this->mShadowMap->getDepthMapSRV() };
	inDeviceContext->PSSetShaderResources(0, 3, textures);
	//Set vertex layout
	inDeviceContext->IASetInputLayout(this->shader->getVertexLayout());
	
	//Bind blend state to the OM state
	FLOAT blendFactor[4] = { 100.0f, 100.0f, 100.0f, 1.0f };
	inDeviceContext->OMSetBlendState(this->gBlendState, blendFactor, 0xffffffff);

	//inDeviceContext->RSSetState(this->rsState);
}

void Scene::setPipelineDebug(ID3D11DeviceContext* inDeviceContext,
	ID3D11RenderTargetView * inRTV, ID3D11DepthStencilView * inDSV)
{
	inDeviceContext->VSSetShader(this->debugShader->getVertexShader(), nullptr, 0);
	inDeviceContext->HSSetShader(nullptr, nullptr, 0);
	inDeviceContext->DSSetShader(nullptr, nullptr, 0);
	inDeviceContext->GSSetShader(nullptr, nullptr, 0);
	inDeviceContext->PSSetShader(this->debugShader->getPixelShader(), nullptr, 0);

	UINT32 vertexSize = sizeof(Vertex);
	UINT32 offset = 0;
	
	inDeviceContext->VSSetConstantBuffers(0, 1, &this->gWavesVSConstBuffer);
	inDeviceContext->IASetVertexBuffers(0, 1, &this->gBoundingVertexBuffer, &vertexSize, &offset);	

	inDeviceContext->OMSetRenderTargets(1,
		&inRTV,
		inDSV);

	// specify the topology to use when drawing
	inDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//Bind textures to pixel shader.		
	//Set vertex layout
	inDeviceContext->IASetInputLayout(this->debugShader->getVertexLayout());	

	inDeviceContext->RSSetState(this->rsState);	
}

void Scene::clearPipeline(ID3D11DeviceContext* inDeviceContext)
{
	ID3D11ShaderResourceView* nullSRV[3] = { nullptr, nullptr, nullptr };
	ID3D11SamplerState* nullSS[] = { nullptr, nullptr };
	ID3D11RasterizerState* nullR = nullptr;
	ID3D11Buffer* nullBuffer[1] = { nullptr };
	FLOAT blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	//inDeviceContext->VSSetConstantBuffers(0, 1, nullBuffer);
	inDeviceContext->RSSetState(nullR);
	inDeviceContext->PSSetSamplers(0, 2, nullSS);
	inDeviceContext->PSSetConstantBuffers(0, 1, nullBuffer);
	inDeviceContext->PSSetShaderResources(0, 3, nullSRV);
	inDeviceContext->OMSetBlendState(0, blendFactor, 0xffffffff);

	this->table->setDraw(false);
	this->terrain->setDraw(false);
	this->box->setDraw(false);
	this->mWaves->setDraw(false);
}

Scene::Scene(ID3D11Device* inDevice, ID3D11DeviceContext* inDeviceContext,
	CameraClass* inCamera, RenderType renderType)
{
	this->mCamera = inCamera;
	this->mStaticCamera = new StaticCamera();
	this->mSun = new Sun();
	this->mWaves = new Waves();
	this->mShadowMap = new ShadowMap(inDevice, WINDOW_WIDTH, WINDOW_HEIGHT);

	this->shader = new Shader();
	this->shader->createShader(inDevice, ShaderType::PositionTextureForWaves);
	this->debugShader = new Shader();
	this->debugShader->createShader(inDevice, ShaderType::Position);

	this->createRasterizerState(inDevice);

	this->idCount = 0;
	this->nodesCount = 0;
	this->nrOfChildren = 0;	

	if (renderType == ForwardRendering)
	{		
		this->table = new TableObject(inDevice,
			inDeviceContext,
			inCamera, this->mStaticCamera, this->mSun, this->mShadowMap, ShaderType::PositionTexture);
		this->table->setId(idCount++);
		this->table->setDraw(false); //Presume its outside the frustum until tested

		this->terrain = new Terrain(inDevice,
			inDeviceContext,
			inCamera, this->mStaticCamera, this->mSun, this->mShadowMap, ShaderType::PositionColor);
		this->terrain->setId(idCount++);
		this->terrain->setDraw(false);

		this->box = new Box(inDevice,
			inDeviceContext,
			inCamera, this->mStaticCamera, this->mSun, this->mShadowMap, ShaderType::PositionTexture);
		this->box->setId(idCount++);
		this->box->setDraw(false);

		//Id is used for frustum culling. This is a geometry representation of the sun,
		//hence it's always drawn and
		//no id is needed for the frustum culling check. 
		this->box2 = new Box(inDevice,
			inDeviceContext,
			inCamera, this->mStaticCamera, this->mSun, this->mShadowMap, ShaderType::PositionTexture);
		this->box2->setPosition(XMFLOAT3(50.0f, 50.0f, 50.0f));	
		this->box2->setDraw(true);

		this->mWaves->initialize(XMFLOAT3(45.0f, 0.4f, 0.0f), 102, 11, 1.0f, 0.03f, 3.25f, 0.4f);
		this->mWaves->setId(this->idCount++);
		this->mWaves->setDraw(false);
		this->createWavesTriangleData(inDevice);
		this->createWavesConstantBuffer(inDevice);
		this->createCameraBuffer(inDevice);
		this->updateWavesMatrices(inDeviceContext);
		this->mWaves->createBoundingBox();

		this->buildQuadTree(inDevice);
		this->createFrustumAndTest();
	}
	else if (renderType == DeferredRendering)
	{
		this->table = new TableObject(inDevice,
			inDeviceContext,
			inCamera, this->mStaticCamera, this->mSun, this->mShadowMap, ShaderType::PositionTextureForDeferred);
		this->table->setId(this->idCount++);
		this->table->setDraw(true);
		/*this->terrain = new Terrain(inDevice,
			inDeviceContext,
			inCamera, ShaderType::PositionColorForDeferred);
			this->terrain->setId(this->idCount)*/
	}
	this->createBlendState(inDevice);		

	this->renderType = renderType;

	this->start = std::chrono::steady_clock::now();
}

Scene::~Scene()
{
	if (this->renderType == ForwardRendering)
	{
		delete this->table;
		delete this->terrain;
		delete this->box;
		delete this->box2;
	}
	else if (this->renderType == DeferredRendering)
	{
		delete this->table;
		//delete this->terrain;
	}
	
	//this->gBoundingVertexBuffer->Release();	
	this->gWavesIndexBuffer->Release();
	this->gWavesVertexBuffer->Release();
	this->gWavesGSConstBuffer->Release();
	this->gWavesVSConstBuffer->Release();
	this->gCameraBuffer->Release();
	this->waterTex->Release();
	this->gBlendState->Release();
	this->rsState->Release();
	
	delete this->mWaves;
	delete this->mSun;
	delete this->mShadowMap;
	delete this->shader;
	delete this->debugShader;	
	delete this->rootNode;
	delete this->mStaticCamera;
}

void Scene::buildQuadTree(ID3D11Device* inDevice)
{
	this->rootNode = new BoxNode(this->idCount + 1);		
	float width = this->terrain->getGridWidth();
	float depth = this->terrain->getGridDepth();	

	float widthLeft = -0.5f * width;
	float widthRight = 0.5f * width;
	float depthClose = -0.5f * depth;
	float depthFar = 0.5f * depth;
	float middle = 0.0f;

	BoundingBox bb;

	bb = this->createBoundingBox(widthLeft, widthRight, depthClose, depthFar);		
	this->rootNode->mBB = bb;
	this->terrain->setBoundingBox(bb);
	
	this->nrOfChildren = 0;
	this->currentLevel = 1;

	this->createChildren(this->rootNode, middle, middle, width, depth);

	this->sortGeometry(this->rootNode, this->table->getId());
	this->sortGeometry(this->rootNode, this->terrain->getId());
	this->sortGeometry(this->rootNode, this->box->getId());
	this->sortGeometry(this->rootNode, this->mWaves->getId());		

	//For terrain bounding box debug draw
	//XMFLOAT3 xmf[8];
	//this->rootNode->children[3]->children[0]->children[0]->mBB.GetCorners(xmf);
	////this->mWaves->getBoundingBox().GetCorners(xmf);
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
	//
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

	//inDevice->CreateBuffer(&boxDesc, &boxData, &this->gBoundingVertexBuffer);
}

void Scene::update(ID3D11Device* inDevice, ID3D11DeviceContext* inDeviceContext, float dt)
{
	this->checkKeys(dt);
	this->mCamera->update(dt);	

	if (this->renderType == ForwardRendering)
	{
		this->table->update();
		this->terrain->update();
		this->box->update();
		this->box2->update();

		this->clearQuadTree(this->rootNode);
		this->sortGeometry(this->rootNode, this->table->getId());
		this->sortGeometry(this->rootNode, this->terrain->getId());
		this->sortGeometry(this->rootNode, this->box->getId());
		this->sortGeometry(this->rootNode, this->mWaves->getId());
		//Test which polygons to be drawn every frame
		this->createFrustumAndTest(); 

		if (this->mWaves->getDraw() == true)
			this->updateWaves(inDeviceContext, dt);

		if (this->mCamera->updateAndGetStatic() == false)
		{
			this->updateWavesMatrices(inDeviceContext);
		}
		else
		{
			this->updateWavesMatricesForStaticCamera(inDeviceContext);
		}

		inDeviceContext->Map(this->gCameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &this->dataPtr);
		this->gCameraData = (CAMERA_BUFFER*)this->dataPtr.pData;
		this->gCameraData->camPos = this->mCamera->getPositionXM();
		inDeviceContext->Unmap(this->gCameraBuffer, 0);
	}
	else if (this->renderType == DeferredRendering)
	{
		this->table->update();
		//this->terrain->update();
	}		
}

void Scene::render(ID3D11DeviceContext* inDeviceContext, ID3D11RenderTargetView* inRTV, ID3D11DepthStencilView* inDSV,
	float inDt)
{
	if (this->renderType == ForwardRendering)
	{
		this->mShadowMap->setPipelineAndViewPort(inDeviceContext, inRTV);
		if(this->table->getDraw() == true)
			this->table->renderShadowPass();
		if(this->terrain->getDraw() == true)
			this->terrain->renderShadowPass();
		//this->box->renderShadowPass();

		D3D11_VIEWPORT vp;
		vp.Width = (float)WINDOW_WIDTH;
		vp.Height = (float)WINDOW_HEIGHT;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		inDeviceContext->RSSetViewports(1, &vp);

		if(this->table->getDraw() == true)
			this->table->render(inRTV, inDSV, inDt, ShaderType::PositionTexture);	
		if(this->terrain->getDraw() == true)
			this->terrain->render(inRTV, inDSV);
		if(this->box->getDraw() == true)
			this->box->render(inRTV, inDSV);
		if(this->box2->getDraw() == true)
			this->box2->render(inRTV, inDSV);

		/*this->setPipelineDebug(inDeviceContext, inRTV, inDSV);
		this->terrain->dbgRender();	*/	
		

		if (this->mWaves->getDraw() == true)
		{
			this->setPipelineWaves(inDeviceContext, inRTV, inDSV);
			inDeviceContext->DrawIndexed(this->nrOfWavesIndices, 0, 0);
			this->clearPipeline(inDeviceContext);
		}		
	}	
}

void Scene::render(ID3D11RenderTargetView * inRTV[], int nrOfRenderTargets,
	ID3D11DepthStencilView * inDSV, float inDt)
{
	if (this->renderType == DeferredRendering)
	{
		this->table->render(inRTV, inDSV, inDt, ShaderType::PositionTextureForDeferred);
		//this->terrain->render(inRTV, inDSV, inDt);
	}
}
