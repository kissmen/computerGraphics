#pragma once
#include "Adapter.h"
#include "window.h"
#include "memory.h"
#include "mesh.h"

class DXcore {
public:
	ID3D11Device* device;
	ID3D11DeviceContext* devicecontext;
	IDXGISwapChain* swapchain;

	ID3D11RenderTargetView* backbufferRenderTargetView;
	ID3D11Texture2D* backbuffer;
	ID3D11DepthStencilView* depthStencilView;
	ID3D11Texture2D* depthbuffer;

	ID3D11RasterizerState* rasterizerState;
	ID3D11DepthStencilState* defaultDepthState;
	ID3D11DepthStencilState* skyDepthState;
	// ID3D11BlendState* blendState;  // For blending if use it

	// New add for Gbuffer
	/*ID3D11Texture2D* gbufferAlbedo;
	ID3D11Texture2D* gbufferNormal;
	ID3D11Texture2D* gbufferDepth;

	ID3D11RenderTargetView* gbufferAlbedoRTV;
	ID3D11RenderTargetView* gbufferNormalRTV;
	ID3D11RenderTargetView* gbufferDepthRTV;

	ID3D11ShaderResourceView* gbufferAlbedoSRV;
	ID3D11ShaderResourceView* gbufferNormalSRV;
	ID3D11ShaderResourceView* gbufferDepthSRV;*/



	void init(int width, int height, HWND hwnd, bool window_fullscreen) {

		DXGI_SWAP_CHAIN_DESC sd;
		memset(&sd, 0, sizeof(DXGI_SWAP_CHAIN_DESC));
		sd.BufferCount = 1;
		sd.BufferDesc.Width = width;
		sd.BufferDesc.Height = height;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = hwnd;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = window_fullscreen ? false : true;
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;


		D3D_FEATURE_LEVEL fl;
		fl = D3D_FEATURE_LEVEL_11_0;

		Adapter adapter;
		adapter.findAdapter();

		HRESULT hr = D3D11CreateDeviceAndSwapChain(adapter.adapter,
			D3D_DRIVER_TYPE_UNKNOWN,
			NULL,
			D3D11_CREATE_DEVICE_DEBUG,
			&fl,
			1,
			D3D11_SDK_VERSION,
			&sd,
			&swapchain,
			&device,
			NULL,
			&devicecontext);
		if (FAILED(hr)) {
			printf("D3D11CreateDeviceAndSwapChain failed. HR=0x%08X\n", hr);
			exit(0);
		}
	    swapchain->SetFullscreenState(window_fullscreen, NULL);

		hr = swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backbuffer);
		if (FAILED(hr)) {
			printf("GetBuffer failed. HR=0x%08X\n", hr);
			exit(0);
		}
		hr = device->CreateRenderTargetView(backbuffer, NULL, &backbufferRenderTargetView);
		if (FAILED(hr)) {
			printf("CreateRenderTargetView failed. HR=0x%08X\n", hr);
			exit(0);
		}


		D3D11_TEXTURE2D_DESC dsvDesc; 
		dsvDesc.Width = width;
		dsvDesc.Height = height;
		dsvDesc.MipLevels = 1;
		dsvDesc.ArraySize = 1;
		dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsvDesc.SampleDesc.Count = 1;
		dsvDesc.SampleDesc.Quality = 0;
		dsvDesc.Usage = D3D11_USAGE_DEFAULT;
		dsvDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		dsvDesc.CPUAccessFlags = 0;
		dsvDesc.MiscFlags = 0;

		hr = device->CreateTexture2D(&dsvDesc, NULL, &depthbuffer);
		if (FAILED(hr)) {
			printf("CreateTexture2D (depthbuffer) failed. HR=0x%08X\n", hr);
			exit(0);
		}
		hr = device->CreateDepthStencilView(depthbuffer, NULL, &depthStencilView);
		if (FAILED(hr)) {
			printf("CreateDepthStencilView failed. HR=0x%08X\n", hr);
			exit(0);
		}
		devicecontext->OMSetRenderTargets(1, &backbufferRenderTargetView, depthStencilView);

		D3D11_VIEWPORT viewport;
		viewport.Width = (float)width;
		viewport.Height = (float)height;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		devicecontext->RSSetViewports(1, &viewport);


		D3D11_RASTERIZER_DESC rsdesc;
		ZeroMemory(&rsdesc, sizeof(D3D11_RASTERIZER_DESC));
		rsdesc.FillMode = D3D11_FILL_SOLID;
		rsdesc.CullMode = D3D11_CULL_NONE;
	    rsdesc.DepthClipEnable = TRUE;
		hr = device->CreateRasterizerState(&rsdesc, &rasterizerState);
		if (FAILED(hr)) {
			printf("CreateRasterizerState failed. HR=0x%08X\n", hr);
			exit(0);
		}

		devicecontext->RSSetState(rasterizerState);

		D3D11_DEPTH_STENCIL_DESC dsDesc = {};
		dsDesc.DepthEnable = TRUE;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
		hr = device->CreateDepthStencilState(&dsDesc, &defaultDepthState);
		if (FAILED(hr)) {
			printf("CreateDepthStencilState (default) failed. HR=0x%08X\n", hr);
			exit(0);
		}


		D3D11_DEPTH_STENCIL_DESC skyDSDesc = dsDesc;
		skyDSDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		skyDSDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
		hr= device->CreateDepthStencilState(&skyDSDesc, &skyDepthState);
		if (FAILED(hr)) {
			printf("CreateDepthStencilState (sky) failed. HR=0x%08X\n", hr);
			exit(0);
		}
	}

	//void createGBuffer(int width, int height) {
	//	// create gbuffer texture desc
	//	D3D11_TEXTURE2D_DESC texDesc = {};
	//	texDesc.Width = width;
	//	texDesc.Height = height;
	//	texDesc.MipLevels = 1;
	//	texDesc.ArraySize = 1;
	//	texDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT; 
	//	texDesc.SampleDesc.Count = 1;
	//	texDesc.Usage = D3D11_USAGE_DEFAULT;
	//	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	//	// create Albedo
	//	device->CreateTexture2D(&texDesc, NULL, &gbufferAlbedo);
	//	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	//	rtvDesc.Format = texDesc.Format;
	//	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	//	device->CreateRenderTargetView(gbufferAlbedo, &rtvDesc, &gbufferAlbedoRTV);

	//	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	//	srvDesc.Format = texDesc.Format;
	//	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	//	srvDesc.Texture2D.MipLevels = 1;
	//	device->CreateShaderResourceView(gbufferAlbedo, &srvDesc, &gbufferAlbedoSRV);

	//	// create Normal
	//	device->CreateTexture2D(&texDesc, NULL, &gbufferNormal);
	//	device->CreateRenderTargetView(gbufferNormal, &rtvDesc, &gbufferNormalRTV);
	//	device->CreateShaderResourceView(gbufferNormal, &srvDesc, &gbufferNormalSRV);

	//	// create Depth/Position texture
	//	device->CreateTexture2D(&texDesc, NULL, &gbufferDepth);
	//	device->CreateRenderTargetView(gbufferDepth, &rtvDesc, &gbufferDepthRTV);
	//	device->CreateShaderResourceView(gbufferDepth, &srvDesc, &gbufferDepthSRV);
	//}

	void setDepthStateSky() {
		devicecontext->OMSetDepthStencilState(skyDepthState, 0);
	}

	void setDepthStateDefault() {
		devicecontext->OMSetDepthStencilState(defaultDepthState, 0);
	}

	void clear() {
		float ClearColour[4] = { 0.1f, 0.1f, 0.25f, 1.0f };
		devicecontext->ClearRenderTargetView(backbufferRenderTargetView, ClearColour);
		devicecontext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	}

	void present() {
		swapchain->Present(0, 0);

	}
};
