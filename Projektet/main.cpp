#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <chrono>
#include <thread>
#include <DirectXMath.h>
#include <Winuser.h>
#include "ObjLoader.h"
#include <vector>
#include "GeometryPass.h"
#include "LightPass.h"
#include "TestPass.h"
#include "GlobalVaribles.h"
#include <cstdlib>
#include <crtdbg.h>
using namespace DirectX;

#define _CRTDBG_MAP_ALLOC

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")	

//const float PI = 3.14;
float dt = 0.0;

RECT windowRect;

HWND InitWindow(HINSTANCE hInstance);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//void CreateTimer(HWND hwnd);
HRESULT CreateDepthStencil();

HRESULT CreateDirect3DContext(HWND wndHandle);

// Most directX Objects are COM Interfaces
// https://es.wikipedia.org/wiki/Component_Object_Model
IDXGISwapChain* gSwapChain = nullptr;

// Device and DeviceContext are the most common objects to
// instruct the API what to do. It is handy to have a reference
// to them available for simple applications.
ID3D11Device* gDevice = nullptr;
ID3D11DeviceContext* gDeviceContext = nullptr;

ID3D11RenderTargetView* gBackbufferRTV = nullptr;
ID3D11Texture2D* pGBufferDepthStencilTexture[NR_OF_TARGET_AND_RESOURCE_VIEWS];
ID3D11ShaderResourceView* gGBufferShaderResourceView[NR_OF_TARGET_AND_RESOURCE_VIEWS];
ID3D11RenderTargetView* gGBufferRenderTargetView[NR_OF_TARGET_AND_RESOURCE_VIEWS];

ID3D11DepthStencilView* gDepthStencilView = nullptr;
ID3D11DepthStencilState * pDSState = nullptr;

void InitializeData()
{
	for (int i = 0; i < NR_OF_TARGET_AND_RESOURCE_VIEWS; i++)
	{
		gGBufferShaderResourceView[i] = 0;
		gGBufferRenderTargetView[i] = 0;
	}
}
HRESULT CreateDepthStencil()
{
	ID3D11Texture2D* pDepthStencil = NULL;
	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width = WINDOW_WIDTH;
	descDepth.Height = WINDOW_HEIGHT;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;	

	HRESULT hr = 0;
	hr = gDevice->CreateTexture2D(&descDepth, NULL, &pDepthStencil);
	if (FAILED(hr))
	{
		return hr;
	}

	D3D11_DEPTH_STENCIL_DESC dsDesc;	
	ZeroMemory(&dsDesc, sizeof(dsDesc));
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	
	dsDesc.StencilEnable = false;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;
	
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;	
	
	gDevice->CreateDepthStencilState(&dsDesc, &pDSState);

	// Bind depth stencil state
	gDeviceContext->OMSetDepthStencilState(pDSState, 1);

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	memset(&descDSV, 0, sizeof(descDSV));
	descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;

	// Create the depth stencil view	 
	hr = gDevice->CreateDepthStencilView(pDepthStencil, 
		&descDSV, 
		&gDepthStencilView);  
	if (FAILED(hr))
	{
		return hr;
	}	    

	pDepthStencil->Release();
}

HRESULT CreateGBuffer()
{
	D3D11_TEXTURE2D_DESC descGBuffer;
	ZeroMemory(&descGBuffer, sizeof(descGBuffer));
	descGBuffer.Width = WINDOW_WIDTH;
	descGBuffer.Height = WINDOW_HEIGHT;
	descGBuffer.MipLevels = 1;
	descGBuffer.ArraySize = 1;
	descGBuffer.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	descGBuffer.SampleDesc.Count = 1;
	descGBuffer.SampleDesc.Quality = 0;
	descGBuffer.Usage = D3D11_USAGE_DEFAULT;
	descGBuffer.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	descGBuffer.CPUAccessFlags = 0;
	descGBuffer.MiscFlags = 0;

	D3D11_RENDER_TARGET_VIEW_DESC pGBufferView;
	pGBufferView.Format = descGBuffer.Format;
	pGBufferView.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	pGBufferView.Texture2D.MipSlice = 0;

	D3D11_SHADER_RESOURCE_VIEW_DESC pGBufferResourceViewDesc;
	pGBufferResourceViewDesc.Format = descGBuffer.Format;
	pGBufferResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	pGBufferResourceViewDesc.Texture2D.MostDetailedMip = 0;
	pGBufferResourceViewDesc.Texture2D.MipLevels = 1;

	HRESULT hr = 0;

	for (int i = 0; i < NR_OF_TARGET_AND_RESOURCE_VIEWS; i++)
	{
		hr = gDevice->CreateTexture2D(&descGBuffer, NULL, &pGBufferDepthStencilTexture[i]);
		if (FAILED(hr))
		{
			return hr;
		}

		hr = gDevice->CreateRenderTargetView(pGBufferDepthStencilTexture[i], &pGBufferView, &gGBufferRenderTargetView[i]);
		if (FAILED(hr))
		{
			return hr;
		}	

		hr = gDevice->CreateShaderResourceView(pGBufferDepthStencilTexture[i], &pGBufferResourceViewDesc, &gGBufferShaderResourceView[i]);
		if (FAILED(hr))
		{
			return hr;
		}
	}

	for (int i = 0; i < NR_OF_TARGET_AND_RESOURCE_VIEWS; i++)
	{		
		pGBufferDepthStencilTexture[i]->Release();
	}

	return S_OK;
}

/*HRESULT CreateTexture()
{
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = BTH_IMAGE_WIDTH;
	desc.Height = BTH_IMAGE_HEIGHT;
	desc.MipLevels = 1;
	desc.ArraySize = 1;	
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = (void*)BTH_IMAGE_DATA;
	initData.SysMemPitch = BTH_IMAGE_WIDTH * 4 * sizeof(char);

	ID3D11Texture2D* tex = NULL;
	HRESULT hr = 0;
	hr = gDevice->CreateTexture2D(&desc, &initData, &tex);
	if (FAILED(hr))
	{
		tex->Release();
		return hr;
	}		
	
	D3D11_SHADER_RESOURCE_VIEW_DESC resViewDesc;	
	memset(&resViewDesc, 0, sizeof(resViewDesc));	
	resViewDesc.Format = desc.Format;
	resViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	resViewDesc.Texture2D.MipLevels = desc.MipLevels;
	resViewDesc.Texture2D.MostDetailedMip = 0;

	hr = gDevice->CreateShaderResourceView(tex, &resViewDesc, &gTextureView);
	if (FAILED(hr))
	{
		tex->Release();
		return hr;
	}
	tex->Release();
}*/

void SetViewport()
{
	D3D11_VIEWPORT vp;
	vp.Width = (float)WINDOW_WIDTH;
	vp.Height = (float)WINDOW_HEIGHT;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	gDeviceContext->RSSetViewports(1, &vp);
}

void Render(ID3D11DeviceContext* inDeviceContext, TestPass* inTPass, float dt)
{
	// clear the back buffer to a deep blue
	float clearColor[] = { 0, 0, 1, 1 };

	gDeviceContext->ClearRenderTargetView(gBackbufferRTV, clearColor);
	gDeviceContext->ClearDepthStencilView(gDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	inTPass->render(inDeviceContext, dt);
}
void Render(GeometryPass* inGPass, LightPass* inLPass, float dt)

{	
	// clear the back buffer to a deep blue
	float clearColor[] = { 0, 0, 1, 1 };	

	ID3D11ShaderResourceView* nullSRV[4] = { nullptr, nullptr, nullptr, nullptr };
	gDeviceContext->PSSetShaderResources(0, 4, nullSRV);

	for (int i = 0; i < NR_OF_TARGET_AND_RESOURCE_VIEWS; i++)
	{
		gDeviceContext->ClearRenderTargetView(gGBufferRenderTargetView[i], clearColor);
	}
	// use DeviceContext to talk to the API
	gDeviceContext->ClearDepthStencilView(gDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);	
	
	inGPass->update(gDevice, gDeviceContext, dt);
	inGPass->render(dt);	

	ID3D11RenderTargetView* nullRTV[4] = { nullptr, nullptr, nullptr, nullptr };
	gDeviceContext->OMSetRenderTargets(4, nullRTV, gDepthStencilView);

	//////////////////////////////////////Light shader pass //////////////////////////////////////////////////////////		
	
	gDeviceContext->ClearRenderTargetView(gBackbufferRTV, clearColor);
	gDeviceContext->ClearDepthStencilView(gDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	
	inLPass->update();
	inLPass->render();		
}

int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	MSG msg = { 0 };
	HWND wndHandle = InitWindow(hInstance); //1. Skapa f�nster
	
	if (wndHandle)
	{
		Rendering renderType = Forward;	
		//Rendering renderType = Deferred;
		TestPass* tPass = nullptr;
		GeometryPass* gPass = nullptr;
		LightPass* lPass = nullptr;
		
		CreateDirect3DContext(wndHandle); //2. Skapa och koppla SwapChain, Device och Device Context
		
		SetViewport(); //3. S�tt viewport

		InitializeData();		

		CreateDepthStencil();

		CreateGBuffer();		
		
		CameraClass* camera = new CameraClass(wndHandle);

		if (renderType == Forward)
		{
			tPass = new TestPass(gDevice,
				gDeviceContext,
				gBackbufferRTV,
				gDepthStencilView,
				camera);
		}
		else if (renderType == Deferred)
		{
			gPass = new GeometryPass(gDevice,
				gDeviceContext,
				gGBufferRenderTargetView,
				gDepthStencilView,
				camera);

			lPass = new LightPass(gDevice,
				gDeviceContext,
				gBackbufferRTV,
				gDepthStencilView,
				gGBufferShaderResourceView,
				camera);
		}		

		ShowWindow(wndHandle, nCmdShow);

		using namespace std::chrono;
		auto time = steady_clock::now();		
		while (WM_QUIT != msg.message)
		{	
			//camera->update();
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{	
				
				auto endTime = steady_clock::now();
				dt = duration<float>(endTime - time).count();
				time = endTime;				

				if (renderType == Forward)
				{
					tPass->update(gDevice, gDeviceContext, dt);
					Render(gDeviceContext, tPass, dt);
				}
				else if (renderType == Deferred)
				{
					Render(gPass, lPass, dt);
				}			
				
				gSwapChain->Present(0, 0); //9. V�xla front- och back-buffer
			}
		}		
		
		ID3D11Debug* g = nullptr;
		gDevice->QueryInterface(&g);

		if (renderType == Forward)
		{
			delete tPass;
		}
		else if (renderType == Deferred)
		{
			delete gPass;
			delete lPass;
		}		
		
		delete camera;
		
		for (int i = 0; i < NR_OF_TARGET_AND_RESOURCE_VIEWS; i++)
		{
			gGBufferRenderTargetView[i]->Release();
			gGBufferShaderResourceView[i]->Release();			
		}		

		pDSState->Release();
		gDepthStencilView->Release();			
		gBackbufferRTV->Release();
		gSwapChain->Release();	
		gDevice->Release();
		gDevice->Release();	
		gDeviceContext->Release();			
		DestroyWindow(wndHandle);		
		
		g->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	}

	return (int) msg.wParam;
}

HWND InitWindow(HINSTANCE hInstance)
{
	WNDCLASSEX wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style          = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc    = WndProc;
	wcex.hInstance      = hInstance;
	wcex.lpszClassName = L"BTH_D3D_DEMO";
	if (!RegisterClassEx(&wcex))
		return false;

	RECT rc = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	HWND handle = CreateWindow(
		L"BTH_D3D_DEMO",
		L"MATBORD",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rc.right - rc.left,
		rc.bottom - rc.top,
		nullptr,
		nullptr,
		hInstance,
		nullptr);

	return handle;
}

LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch (message) 
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;		
	}

	return DefWindowProc(hWnd, message, wParam, lParam);	
}

HRESULT CreateDirect3DContext(HWND wndHandle)
{
	// create a struct to hold information about the swap chain
	DXGI_SWAP_CHAIN_DESC scd;

	// clear out the struct for use
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	// fill the swap chain description struct
	scd.BufferCount = 1;                                    // one back buffer
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
	scd.OutputWindow = wndHandle;                           // the window to be used
	scd.SampleDesc.Count = 1;                               // how many multisamples
	scd.Windowed = TRUE;                                    // windowed/full-screen mode

	// create a device, device context and swap chain using the information in the scd struct
	HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		D3D11_CREATE_DEVICE_DEBUG, 
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scd,
		&gSwapChain,
		&gDevice,
		NULL,
		&gDeviceContext);	

	if (SUCCEEDED(hr))
	{
		// get the address of the back buffer
		ID3D11Texture2D* pBackBuffer = nullptr;
		gSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

		// use the back buffer address to create the render target
		gDevice->CreateRenderTargetView(pBackBuffer, NULL, &gBackbufferRTV);
		pBackBuffer->Release();

		// set the render target as the back buffer
		gDeviceContext->OMSetRenderTargets(1, &gBackbufferRTV, gDepthStencilView);		
	}

	GetWindowRect(wndHandle, &windowRect);

	return hr;
}