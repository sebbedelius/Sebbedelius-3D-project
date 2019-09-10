#include "Shader.h"

HRESULT Shader::createPositionShader(ID3D11Device * inDevice)
{
	// Binary Large OBject (BLOB), for compiled shader, and errors.
	this->pVS = nullptr;
	ID3DBlob* errorBlob = nullptr;

	// https://msdn.microsoft.com/en-us/library/windows/desktop/hh968107(v=vs.85).aspx
	HRESULT result = D3DCompileFromFile(
		L"PositionVertexShader.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"PVS_main",		// entry point
		"vs_5_0",		// shader model (target)
		D3DCOMPILE_DEBUG,	// shader compile options (DEBUGGING)
		0,				// IGNORE...DEPRECATED.
		&this->pVS,			// double pointer to ID3DBlob		
		&errorBlob		// pointer for Error Blob messages.
	);

	// compilation failed?
	if (FAILED(result))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			// release "reference" to errorBlob interface object
			errorBlob->Release();
		}
		if (this->pVS)
			this->pVS->Release();
		return result;
	}

	inDevice->CreateVertexShader(
		this->pVS->GetBufferPointer(),
		this->pVS->GetBufferSize(),
		nullptr,
		&this->gVertexShader
	);

	this->createInputLayoutPosition(inDevice);

	//create pixel shader
	this->pPS = nullptr;
	if (errorBlob) errorBlob->Release();
	errorBlob = nullptr;

	result = D3DCompileFromFile(
		L"PositionPixelShader.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"PPS_main",		// entry point
		"ps_5_0",		// shader model (target)
		D3DCOMPILE_DEBUG,	// shader compile options
		0,				// effect compile options
		&this->pPS,			// double pointer to ID3DBlob		
		&errorBlob			// pointer for Error Blob messages.
	);

	// compilation failed?
	if (FAILED(result))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			// release "reference" to errorBlob interface object
			errorBlob->Release();
		}
		if (this->pPS)
			this->pPS->Release();
		return result;
	}

	inDevice->CreatePixelShader(pPS->GetBufferPointer(), pPS->GetBufferSize(), nullptr, &this->gPixelShader);

	return S_OK;
}

HRESULT Shader::createPositionColorShader(ID3D11Device* inDevice)
{
	// Binary Large OBject (BLOB), for compiled shader, and errors.
	this->pVS = nullptr;
	ID3DBlob* errorBlob = nullptr;

	// https://msdn.microsoft.com/en-us/library/windows/desktop/hh968107(v=vs.85).aspx
	HRESULT result = D3DCompileFromFile(
		L"ColorVertexShader.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"CVS_main",		// entry point
		"vs_5_0",		// shader model (target)
		D3DCOMPILE_DEBUG,	// shader compile options (DEBUGGING)
		0,				// IGNORE...DEPRECATED.
		&this->pVS,			// double pointer to ID3DBlob		
		&errorBlob		// pointer for Error Blob messages.
	);

	// compilation failed?
	if (FAILED(result))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			// release "reference" to errorBlob interface object
			errorBlob->Release();
		}
		if (this->pVS)
			this->pVS->Release();
		return result;
	}

	inDevice->CreateVertexShader(
		this->pVS->GetBufferPointer(),
		this->pVS->GetBufferSize(),
		nullptr,
		&this->gVertexShader
	);

	this->createInputLayoutPositionColor(inDevice);
	
	//create geometry shader
	this->pGS = nullptr;
	if (errorBlob) errorBlob->Release();
	errorBlob = nullptr;

	result = D3DCompileFromFile(
		L"ColorGeometryShader.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"CGS_main",		// entry point
		"gs_5_0",		// shader model (target)
		D3DCOMPILE_DEBUG,	// shader compile options
		0,					// effect compile options
		&this->pGS,			// double pointer to ID3DBlob		
		&errorBlob			// pointer for Error Blob messages.
	);

	// compilation failed?
	if (FAILED(result))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			// release "reference" to errorBlob interface object
			errorBlob->Release();
		}
		if (this->pGS)
			this->pGS->Release();
		return result;
	}
	inDevice->CreateGeometryShader(
		pGS->GetBufferPointer(),
		pGS->GetBufferSize(),
		nullptr,
		&this->gGeometryShader
	);	

	//create pixel shader
	this->pPS = nullptr;
	if (errorBlob) errorBlob->Release();
	errorBlob = nullptr;

	result = D3DCompileFromFile(
		L"ColorPixelShader.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"CPS_main",		// entry point
		"ps_5_0",		// shader model (target)
		D3DCOMPILE_DEBUG,	// shader compile options
		0,				// effect compile options
		&this->pPS,			// double pointer to ID3DBlob		
		&errorBlob			// pointer for Error Blob messages.
	);

	// compilation failed?
	if (FAILED(result))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			// release "reference" to errorBlob interface object
			errorBlob->Release();
		}
		if (this->pPS)
			this->pPS->Release();
		return result;
	}

	inDevice->CreatePixelShader(pPS->GetBufferPointer(), pPS->GetBufferSize(), nullptr, &this->gPixelShader);	
	

	return S_OK;
}

HRESULT Shader::createPositionTextureShader(ID3D11Device* inDevice)
{

	// Binary Large OBject (BLOB), for compiled shader, and errors.
	this->pVS = nullptr;
	ID3DBlob* errorBlob = nullptr;

	// https://msdn.microsoft.com/en-us/library/windows/desktop/hh968107(v=vs.85).aspx
	HRESULT result = D3DCompileFromFile(
		L"Vertex.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"VS_main",		// entry point
		"vs_5_0",		// shader model (target)
		D3DCOMPILE_DEBUG,	// shader compile options (DEBUGGING)
		0,				// IGNORE...DEPRECATED.
		&this->pVS,			// double pointer to ID3DBlob		
		&errorBlob		// pointer for Error Blob messages.
	);

	// compilation failed?
	if (FAILED(result))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			// release "reference" to errorBlob interface object
			errorBlob->Release();
		}
		if (this->pVS)
			this->pVS->Release();
		return result;
	}

	inDevice->CreateVertexShader(
		this->pVS->GetBufferPointer(),
		this->pVS->GetBufferSize(),
		nullptr,
		&this->gVertexShader
	);	

	this->createInputLayoutPositionTexture(inDevice);

	//create geometry shader
	this->pGS = nullptr;
	if (errorBlob) errorBlob->Release();
	errorBlob = nullptr;

	result = D3DCompileFromFile(
		L"GeometryShader.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"GS_main",		// entry point
		"gs_5_0",		// shader model (target)
		D3DCOMPILE_DEBUG,	// shader compile options
		0,					// effect compile options
		&this->pGS,			// double pointer to ID3DBlob		
		&errorBlob			// pointer for Error Blob messages.
	);

	// compilation failed?
	if (FAILED(result))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			// release "reference" to errorBlob interface object
			errorBlob->Release();
		}
		if (this->pGS)
			this->pGS->Release();
		return result;
	}
	inDevice->CreateGeometryShader(
		this->pGS->GetBufferPointer(),
		this->pGS->GetBufferSize(),
		nullptr,
		&this->gGeometryShader
	);	

	//create pixel shader
	this->pPS = nullptr;
	if (errorBlob) errorBlob->Release();
	errorBlob = nullptr;

	result = D3DCompileFromFile(
		L"ForwardPixel.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"PST_main",		// entry point
		"ps_5_0",		// shader model (target)
		D3DCOMPILE_DEBUG,	// shader compile options
		0,				// effect compile options
		&this->pPS,			// double pointer to ID3DBlob		
		&errorBlob			// pointer for Error Blob messages.
	);

	// compilation failed?
	if (FAILED(result))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			// release "reference" to errorBlob interface object
			errorBlob->Release();
		}
		if (this->pPS)
			this->pPS->Release();
		return result;
	}

	inDevice->CreatePixelShader(this->pPS->GetBufferPointer(), this->pPS->GetBufferSize(), nullptr, &this->gPixelShader);
	
	return S_OK;
}

HRESULT Shader::createPositionTextureForWavesShader(ID3D11Device * inDevice)
{
	// Binary Large OBject (BLOB), for compiled shader, and errors.
	this->pVS = nullptr;
	ID3DBlob* errorBlob = nullptr;

	// https://msdn.microsoft.com/en-us/library/windows/desktop/hh968107(v=vs.85).aspx
	HRESULT result = D3DCompileFromFile(
		L"WavesVertex.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"VSW_main",		// entry point
		"vs_5_0",		// shader model (target)
		D3DCOMPILE_DEBUG,	// shader compile options (DEBUGGING)
		0,				// IGNORE...DEPRECATED.
		&this->pVS,			// double pointer to ID3DBlob		
		&errorBlob		// pointer for Error Blob messages.
	);

	// compilation failed?
	if (FAILED(result))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			// release "reference" to errorBlob interface object
			errorBlob->Release();
		}
		if (this->pVS)
			this->pVS->Release();
		return result;
	}

	inDevice->CreateVertexShader(
		this->pVS->GetBufferPointer(),
		this->pVS->GetBufferSize(),
		nullptr,
		&this->gVertexShader
	);

	this->createInputLayoutPositionTexture(inDevice);

	//create geometry shader
	this->pGS = nullptr;
	if (errorBlob) errorBlob->Release();
	errorBlob = nullptr;

	result = D3DCompileFromFile(
		L"GeometryShader.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"GS_main",		// entry point
		"gs_5_0",		// shader model (target)
		D3DCOMPILE_DEBUG,	// shader compile options
		0,					// effect compile options
		&this->pGS,			// double pointer to ID3DBlob		
		&errorBlob			// pointer for Error Blob messages.
	);

	// compilation failed?
	if (FAILED(result))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			// release "reference" to errorBlob interface object
			errorBlob->Release();
		}
		if (this->pGS)
			this->pGS->Release();
		return result;
	}
	inDevice->CreateGeometryShader(
		this->pGS->GetBufferPointer(),
		this->pGS->GetBufferSize(),
		nullptr,
		&this->gGeometryShader
	);

	//create pixel shader
	this->pPS = nullptr;
	if (errorBlob) errorBlob->Release();
	errorBlob = nullptr;

	result = D3DCompileFromFile(
		L"ForwardPixel.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"PST_main",		// entry point
		"ps_5_0",		// shader model (target)
		D3DCOMPILE_DEBUG,	// shader compile options
		0,				// effect compile options
		&this->pPS,			// double pointer to ID3DBlob		
		&errorBlob			// pointer for Error Blob messages.
	);

	// compilation failed?
	if (FAILED(result))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			// release "reference" to errorBlob interface object
			errorBlob->Release();
		}
		if (this->pPS)
			this->pPS->Release();
		return result;
	}

	inDevice->CreatePixelShader(this->pPS->GetBufferPointer(), this->pPS->GetBufferSize(), nullptr, &this->gPixelShader);

	return S_OK;
}

HRESULT Shader::createPositionColorForShadowShader(ID3D11Device * inDevice)
{
	// Binary Large OBject (BLOB), for compiled shader, and errors.
	this->pVS = nullptr;
	ID3DBlob* errorBlob = nullptr;
	
	HRESULT result = D3DCompileFromFile(
		L"ShadowVertexRGB.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"CVSS_main",		// entry point
		"vs_5_0",		// shader model (target)
		D3DCOMPILE_DEBUG,	// shader compile options (DEBUGGING)
		0,				// IGNORE...DEPRECATED.
		&this->pVS,			// double pointer to ID3DBlob		
		&errorBlob		// pointer for Error Blob messages.
	);

	// compilation failed?
	if (FAILED(result))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			// release "reference" to errorBlob interface object
			errorBlob->Release();
		}
		if (this->pVS)
			this->pVS->Release();
		return result;
	}

	inDevice->CreateVertexShader(
		this->pVS->GetBufferPointer(),
		this->pVS->GetBufferSize(),
		nullptr,
		&this->gVertexShader
	);

	this->createInputLayoutPositionColor(inDevice);		

	return S_OK;
}

HRESULT Shader::createPositionTextureForShadowShader(ID3D11Device * inDevice)
{
	// Binary Large OBject (BLOB), for compiled shader, and errors.
	this->pVS = nullptr;
	ID3DBlob* errorBlob = nullptr;

	// https://msdn.microsoft.com/en-us/library/windows/desktop/hh968107(v=vs.85).aspx
	HRESULT result = D3DCompileFromFile(
		L"ShadowVertex.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"VSS_main",		// entry point
		"vs_5_0",		// shader model (target)
		D3DCOMPILE_DEBUG,	// shader compile options (DEBUGGING)
		0,				// IGNORE...DEPRECATED.
		&this->pVS,			// double pointer to ID3DBlob		
		&errorBlob		// pointer for Error Blob messages.
	);

	// compilation failed?
	if (FAILED(result))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			// release "reference" to errorBlob interface object
			errorBlob->Release();
		}
		if (this->pVS)
			this->pVS->Release();
		return result;
	}

	inDevice->CreateVertexShader(
		this->pVS->GetBufferPointer(),
		this->pVS->GetBufferSize(),
		nullptr,
		&this->gVertexShader
	);

	this->createInputLayoutPositionTexture(inDevice);		

	return S_OK;
}

HRESULT Shader::createPositionColorForDeferredShader(ID3D11Device * inDevice) // Gör nya shaders
{
	// Binary Large OBject (BLOB), for compiled shader, and errors.
	this->pVS = nullptr;
	ID3DBlob* errorBlob = nullptr;

	// https://msdn.microsoft.com/en-us/library/windows/desktop/hh968107(v=vs.85).aspx
	HRESULT result = D3DCompileFromFile(
		L"Vertex.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"VS_main",		// entry point
		"vs_5_0",		// shader model (target)
		D3DCOMPILE_DEBUG,	// shader compile options (DEBUGGING)
		0,				// IGNORE...DEPRECATED.
		&this->pVS,			// double pointer to ID3DBlob		
		&errorBlob		// pointer for Error Blob messages.
	);

	// compilation failed?
	if (FAILED(result))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			// release "reference" to errorBlob interface object
			errorBlob->Release();
		}
		if (this->pVS)
			this->pVS->Release();
		return result;
	}

	inDevice->CreateVertexShader(
		this->pVS->GetBufferPointer(),
		this->pVS->GetBufferSize(),
		nullptr,
		&this->gVertexShader
	);

	this->createInputLayoutPositionTexture(inDevice);

	//create geometry shader
	this->pGS = nullptr;
	if (errorBlob) errorBlob->Release();
	errorBlob = nullptr;

	result = D3DCompileFromFile(
		L"GeometryShader.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"GS_main",		// entry point
		"gs_5_0",		// shader model (target)
		D3DCOMPILE_DEBUG,	// shader compile options
		0,					// effect compile options
		&this->pGS,			// double pointer to ID3DBlob		
		&errorBlob			// pointer for Error Blob messages.
	);

	// compilation failed?
	if (FAILED(result))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			// release "reference" to errorBlob interface object
			errorBlob->Release();
		}
		if (this->pGS)
			this->pGS->Release();
		return result;
	}
	inDevice->CreateGeometryShader(
		this->pGS->GetBufferPointer(),
		this->pGS->GetBufferSize(),
		nullptr,
		&this->gGeometryShader
	);

	//create pixel shader
	this->pPS = nullptr;
	if (errorBlob) errorBlob->Release();
	errorBlob = nullptr;

	result = D3DCompileFromFile(
		L"DeferredPixel.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"PS_main",		// entry point
		"ps_5_0",		// shader model (target)
		D3DCOMPILE_DEBUG,	// shader compile options
		0,				// effect compile options
		&this->pPS,			// double pointer to ID3DBlob		
		&errorBlob			// pointer for Error Blob messages.
	);

	// compilation failed?
	if (FAILED(result))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			// release "reference" to errorBlob interface object
			errorBlob->Release();
		}
		if (this->pPS)
			this->pPS->Release();
		return result;
	}

	inDevice->CreatePixelShader(this->pPS->GetBufferPointer(), this->pPS->GetBufferSize(), nullptr, &this->gPixelShader);
	
	return S_OK;
}

HRESULT Shader::createPositionTextureForDeferredShader(ID3D11Device * inDevice)
{
	// Binary Large OBject (BLOB), for compiled shader, and errors.
	this->pVS = nullptr;
	ID3DBlob* errorBlob = nullptr;

	// https://msdn.microsoft.com/en-us/library/windows/desktop/hh968107(v=vs.85).aspx
	HRESULT result = D3DCompileFromFile(
		L"Vertex.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"VS_main",		// entry point
		"vs_5_0",		// shader model (target)
		D3DCOMPILE_DEBUG,	// shader compile options (DEBUGGING)
		0,				// IGNORE...DEPRECATED.
		&this->pVS,			// double pointer to ID3DBlob		
		&errorBlob		// pointer for Error Blob messages.
	);

	// compilation failed?
	if (FAILED(result))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			// release "reference" to errorBlob interface object
			errorBlob->Release();
		}
		if (this->pVS)
			this->pVS->Release();
		return result;
	}

	inDevice->CreateVertexShader(
		this->pVS->GetBufferPointer(),
		this->pVS->GetBufferSize(),
		nullptr,
		&this->gVertexShader
	);

	this->createInputLayoutPositionTexture(inDevice);

	//create geometry shader
	this->pGS = nullptr;
	if (errorBlob) errorBlob->Release();
	errorBlob = nullptr;

	result = D3DCompileFromFile(
		L"GeometryShader.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"GS_main",		// entry point
		"gs_5_0",		// shader model (target)
		D3DCOMPILE_DEBUG,	// shader compile options
		0,					// effect compile options
		&this->pGS,			// double pointer to ID3DBlob		
		&errorBlob			// pointer for Error Blob messages.
	);

	// compilation failed?
	if (FAILED(result))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			// release "reference" to errorBlob interface object
			errorBlob->Release();
		}
		if (this->pGS)
			this->pGS->Release();
		return result;
	}
	inDevice->CreateGeometryShader(
		this->pGS->GetBufferPointer(),
		this->pGS->GetBufferSize(),
		nullptr,
		&this->gGeometryShader
	);	

	//create pixel shader
	this->pPS = nullptr;
	if (errorBlob) errorBlob->Release();
	errorBlob = nullptr;

	result = D3DCompileFromFile(
		L"DeferredPixel.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"PS_main",		// entry point
		"ps_5_0",		// shader model (target)
		D3DCOMPILE_DEBUG,	// shader compile options
		0,				// effect compile options
		&this->pPS,			// double pointer to ID3DBlob		
		&errorBlob			// pointer for Error Blob messages.
	);

	// compilation failed?
	if (FAILED(result))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			// release "reference" to errorBlob interface object
			errorBlob->Release();
		}
		if (this->pPS)
			this->pPS->Release();
		return result;
	}

	inDevice->CreatePixelShader(this->pPS->GetBufferPointer(), this->pPS->GetBufferSize(), nullptr, &this->gPixelShader);
	
	return S_OK;
}

HRESULT Shader::createPositionTextureForLightPassShader(ID3D11Device * inDevice)
{
	this->pVS = nullptr;
	ID3DBlob* errorBlob = nullptr;

	HRESULT result = D3DCompileFromFile(
		L"VSLight.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"VSL_main",		// entry point
		"vs_5_0",		// shader model (target)
		D3DCOMPILE_DEBUG,	// shader compile options (DEBUGGING)
		0,				// IGNORE...DEPRECATED.
		&this->pVS,			// double pointer to ID3DBlob		
		&errorBlob		// pointer for Error Blob messages.
	);

	if (FAILED(result))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			// release "reference" to errorBlob interface object
			errorBlob->Release();
		}
		if (this->pVS)
			this->pVS->Release();
		return result;
	}

	inDevice->CreateVertexShader(
		this->pVS->GetBufferPointer(),
		this->pVS->GetBufferSize(),
		nullptr,
		&this->gVertexShader
	);

	this->createInputLayoutPositionTexture(inDevice);

	//create pixel light shader
	this->pPS = nullptr;
	if (errorBlob) errorBlob->Release();
	errorBlob = nullptr;

	result = D3DCompileFromFile(
		L"PSLight.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"PSL_main",		// entry point
		"ps_5_0",		// shader model (target)
		D3DCOMPILE_DEBUG,	// shader compile options
		0,				// effect compile options
		&this->pPS,			// double pointer to ID3DBlob		
		&errorBlob			// pointer for Error Blob messages.
	);

	// compilation failed?
	if (FAILED(result))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			// release "reference" to errorBlob interface object
			errorBlob->Release();
		}
		if (this->pPS)
			this->pPS->Release();
		return result;
	}

	inDevice->CreatePixelShader(this->pPS->GetBufferPointer(), this->pPS->GetBufferSize(), nullptr, &this->gPixelShader);

	return S_OK;
}

void Shader::createInputLayoutPositionTexture(ID3D11Device* inDevice)
{	
	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{
			"POSITION",		// "semantic" name in shader
			0,				// "semantic" index (not used)
			DXGI_FORMAT_R32G32B32_FLOAT, // size of ONE element (3 floats)
			0,							 // input slot
			0,							 // offset of first element
			D3D11_INPUT_PER_VERTEX_DATA, // specify data PER vertex
			0							 // used for INSTANCING (ignore)
		},
		{
			"TEXCOORD", //Ändra namn till TEXCOORD är väl rimligt (glöm inte ändra i shaders)
			0,				// same slot as previous (same vertexBuffer)
			DXGI_FORMAT_R32G32_FLOAT, //Ta bort B32 när vi ska ha texturer för 2 floats istället för 3 (u, v)
			0,
			12,							// offset of FIRST element (after POSITION)
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		},
	};

	inDevice->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), pVS->GetBufferPointer(), pVS->GetBufferSize(), &this->gVertexLayout);

}

void Shader::createInputLayoutPositionColor(ID3D11Device* inDevice)
{	
	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{
			"POSITION",		// "semantic" name in shader
			0,				// "semantic" index (not used)
			DXGI_FORMAT_R32G32B32_FLOAT, // size of ONE element (3 floats)
			0,							 // input slot
			0,							 // offset of first element
			D3D11_INPUT_PER_VERTEX_DATA, // specify data PER vertex
			0							 // used for INSTANCING (ignore)
		},
		{
			"COLOR", 
			0,				// same slot as previous (same vertexBuffer)
			DXGI_FORMAT_R32G32B32_FLOAT, 
			0,
			12,							// offset of FIRST element (after POSITION)
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		},
	};

	inDevice->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), pVS->GetBufferPointer(), pVS->GetBufferSize(), &this->gVertexLayout);

}

void Shader::createInputLayoutPosition(ID3D11Device* inDevice)
{	
	D3D11_INPUT_ELEMENT_DESC inputDescLight[] = {
		{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			0,
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		},
	};

	inDevice->CreateInputLayout(inputDescLight, ARRAYSIZE(inputDescLight), this->pVS->GetBufferPointer(), this->pVS->GetBufferSize(), &this->gVertexLayout);

}

Shader::Shader()
{
}

Shader::~Shader()
{
	this->gVertexShader->Release();
	
	if (this->usingGeometryShader == true)
	{		
		this->gGeometryShader->Release();		
	}
	
	if (this->usingPixelShader == true)
	{
		this->gPixelShader->Release();
	}
	this->gVertexLayout->Release();	
}

HRESULT Shader::createShader(ID3D11Device* inDevice, ShaderType shaderType)
{
	HRESULT hr = 0;

	if (shaderType == Position)
	{
		hr = this->createPositionShader(inDevice);
		if (FAILED(hr))
		{
			return hr;
		}
		this->usingGeometryShader = false;
		this->usingPixelShader = true;
	}
	else if (shaderType == PositionColor)
	{
		hr = this->createPositionColorShader(inDevice);

		if (FAILED(hr))
		{
			return hr;
		}	

		this->usingGeometryShader = true;
	}
	else if (shaderType == PositionTexture)
	{
		hr = this->createPositionTextureShader(inDevice);
		
		if (FAILED(hr))
		{
			return hr;
		}	

		this->usingGeometryShader = true;
	}
	else if (shaderType == PositionTextureForWaves)
	{
		hr = this->createPositionTextureForWavesShader(inDevice);

		if (FAILED(hr))
		{
			return hr;
		}

		this->usingGeometryShader = true;
	}
	else if (shaderType == PositionColorShadowPass)
	{
		hr = this->createPositionColorForShadowShader(inDevice);
		if (FAILED(hr))
		{
			return hr;
		}
		this->usingGeometryShader = false;
		this->usingPixelShader = false;
	}
	else if (shaderType == PositionTexShadowPass)
	{
		hr = this->createPositionTextureForShadowShader(inDevice);
		if (FAILED(hr))
		{
			return hr;
		}

		this->usingGeometryShader = false;
		this->usingPixelShader = false;
	}
	else if (shaderType == PositionTextureForDeferred)
	{
		hr = this->createPositionTextureForDeferredShader(inDevice);

		if (FAILED(hr))
		{
			return hr;
		}

		this->usingGeometryShader = true;
	}
	else if (shaderType == PositionTextureForLightPass)
	{
		hr = this->createPositionTextureForLightPassShader(inDevice);

		if (FAILED(hr))
		{
			return hr;
		}

		this->usingGeometryShader = false;
	}		
	else if (shaderType == PositionColorWithIndexBufferForDeferred)
	{
		hr = this->createPositionColorForDeferredShader(inDevice);

		if (FAILED(hr))
		{
			return hr;
		}

		this->usingGeometryShader = true;
	}

	return S_OK;
}

ID3D11VertexShader * Shader::getVertexShader() const
{
	return this->gVertexShader;
}

ID3D11GeometryShader * Shader::getGeometryShader() const
{
	return this->gGeometryShader;
}

ID3D11PixelShader * Shader::getPixelShader() const
{
	return this->gPixelShader;
}

ID3D11InputLayout * Shader::getVertexLayout() const
{
	return this->gVertexLayout;
}