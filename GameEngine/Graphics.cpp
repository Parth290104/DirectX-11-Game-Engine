#include "Graphics.h"
#include "dxerr.h"
#include <sstream>
#include <d3dcompiler.h>
#include <DirectXMath.h>

namespace wrl = Microsoft::WRL;
namespace dx = DirectX;

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

// graphics exception checking/throwing macros (some dxgi infos)
#define GFX_EXCEPT_NOINFO(hr) Graphics::HrException(__LINE__, __FILE__, (hr))
#define GFX_THROW_NOINFO(hrcall) if(FAILED(hr = (hrcall))) throw Graphics::HrException(__LINE__, __FILE__, (hr))

#ifndef NDEBUG
#define GFX_EXCEPT(hr) Graphics::HrException(__LINE__, __FILE__, (hr), dxgiInfoManager.GetMessages())
#define GFX_THROW_INFO(hrcall) dxgiInfoManager.Set(); if(FAILED(hr = (hrcall))) throw GFX_EXCEPT(hr)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException(__LINE__, __FILE__, (hr), dxgiInfoManager.GetMessages())
#define GFX_THROW_INFO_ONLY(call) \
	do { \
		dxgiInfoManager.Set(); \
		call; \
		auto v = dxgiInfoManager.GetMessages(); \
		if(!v.empty()) { throw Graphics::InfoException(__LINE__, __FILE__, v); } \
	} while(0)
#else
#define GFX_EXCEPT(hr) Graphics::HrException(__LINE__, __FILE__, (hr))
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException(__LINE__, __FILE__, (hr))
#define GFX_THROW_INFO_ONLY(call) (call)
#endif

Graphics::Graphics(HWND hWnd)
{
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc{};

	dxgiSwapChainDesc.BufferDesc.Width = 0;
	dxgiSwapChainDesc.BufferDesc.Height = 0;
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 0;
	dxgiSwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	dxgiSwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	dxgiSwapChainDesc.SampleDesc.Count = 1;
	dxgiSwapChainDesc.SampleDesc.Quality = 0;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.BufferCount = 1;
	dxgiSwapChainDesc.OutputWindow = (hWnd);
	dxgiSwapChainDesc.Windowed = TRUE;
	dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	dxgiSwapChainDesc.Flags = 0;

	UINT swapCreateFlags = 0u;
#ifndef NDEBUG
	swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// For checking results of d3d functions
	HRESULT hr;

	GFX_THROW_INFO(
	// create device and front/back buffers, and swap chain and rendering context
	D3D11CreateDeviceAndSwapChain
	(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		swapCreateFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&dxgiSwapChainDesc,
		&pIDXGISwapChain, // & operator first releases and then return the address
		&pID3D11Device, // whereas GetAdreessOf() Directly returns the object.
		nullptr,
		&pID3D11DeviceContext
	));

	// gain access to texture subresource in swap chain (back buffer)
	wrl::ComPtr<ID3D11Resource> pID3D11Resource_BackBuffer = nullptr;

	GFX_THROW_INFO(pIDXGISwapChain->GetBuffer(
		0,
		__uuidof(ID3D11Resource),
		&pID3D11Resource_BackBuffer
	));

	GFX_THROW_INFO(pID3D11Device->CreateRenderTargetView(
		pID3D11Resource_BackBuffer.Get(),
		nullptr,
		&pID3D11RenderTargetView
	));

	// create depth stencil buffer
	D3D11_DEPTH_STENCIL_DESC d3d11DepthStencilDesc{};
	d3d11DepthStencilDesc.DepthEnable = TRUE;
	d3d11DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	d3d11DepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	wrl::ComPtr<ID3D11DepthStencilState> pID3D11DepthStencilState;
	GFX_THROW_INFO(pID3D11Device->CreateDepthStencilState(&d3d11DepthStencilDesc, &pID3D11DepthStencilState));

	// bind depth state
	pID3D11DeviceContext->OMSetDepthStencilState(pID3D11DepthStencilState.Get(), 1u);

	// create depth stencil texture
	wrl::ComPtr<ID3D11Texture2D> pID3D11Texture2D;
	D3D11_TEXTURE2D_DESC d3d11Texture2DDesc{};
	d3d11Texture2DDesc.Width = 800u;
	d3d11Texture2DDesc.Height = 600u;
	d3d11Texture2DDesc.MipLevels = 1u;
	d3d11Texture2DDesc.ArraySize = 1u;
	d3d11Texture2DDesc.Format = DXGI_FORMAT_D32_FLOAT;
	d3d11Texture2DDesc.SampleDesc.Count = 1u;
	d3d11Texture2DDesc.SampleDesc.Quality = 0u;
	d3d11Texture2DDesc.Usage = D3D11_USAGE_DEFAULT;
	d3d11Texture2DDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	GFX_THROW_INFO(pID3D11Device -> CreateTexture2D(&d3d11Texture2DDesc, nullptr, &pID3D11Texture2D));

	// create view of depth stencil texture
	D3D11_DEPTH_STENCIL_VIEW_DESC d3d11DepthStencilViewDesc{};
	d3d11DepthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	d3d11DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	d3d11DepthStencilViewDesc.Texture2D.MipSlice = 0u;

	GFX_THROW_INFO(pID3D11Device -> CreateDepthStencilView(pID3D11Texture2D.Get(), &d3d11DepthStencilViewDesc, &pID3D11DepthStencilView));

	// bind depth stencil view to Output Merger
	pID3D11DeviceContext->OMSetRenderTargets(1u, pID3D11RenderTargetView.GetAddressOf(), pID3D11DepthStencilView.Get());
}

void Graphics::EndFrame()
{
	HRESULT hr;
#ifndef NDEBUG
	dxgiInfoManager.Set();
#endif

	if (FAILED(hr = pIDXGISwapChain->Present(1u, 0u)))
	{
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
			throw GFX_DEVICE_REMOVED_EXCEPT(pID3D11Device->GetDeviceRemovedReason());
		else
			throw GFX_EXCEPT(hr);
	}
}

void Graphics::ClearBuffer(float red, float green, float blue) noexcept
{
	const float color[] = { red, green, blue, 1.0f };

	pID3D11DeviceContext->ClearRenderTargetView(pID3D11RenderTargetView.Get(), color);
	pID3D11DeviceContext->ClearDepthStencilView(pID3D11DepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
}

void Graphics::DrawTestTriangle(float angle, float x, float z)
{
	HRESULT hr;

	// Create vertex buffer (1 2D Triangle in the center of the screen)
	struct Vertex
	{
		struct
		{
			float x;
			float y;
			float z;
		} pos;
	};

	const Vertex vertices[] =
	{
		{ -1.0f, -1.0f, -1.0f },
		{  1.0f, -1.0f, -1.0f },
		{ -1.0f,  1.0f, -1.0f },
		{  1.0f,  1.0f, -1.0f },
		{ -1.0f, -1.0f,  1.0f },
		{  1.0f, -1.0f,  1.0f },
		{ -1.0f,  1.0f,  1.0f },
		{  1.0f,  1.0f,  1.0f },
	};

	wrl::ComPtr<ID3D11Buffer> pID3D11Buffer_VertexBuffer;

	D3D11_BUFFER_DESC d3d11BufferDesc{};
	d3d11BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3d11BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3d11BufferDesc.CPUAccessFlags = 0u;
	d3d11BufferDesc.MiscFlags = 0;
	d3d11BufferDesc.ByteWidth = sizeof(vertices);
	d3d11BufferDesc.StructureByteStride = sizeof(Vertex);

	D3D11_SUBRESOURCE_DATA d3d11SubResourceData{};
	d3d11SubResourceData.pSysMem = vertices;

	GFX_THROW_INFO(pID3D11Device->CreateBuffer(&d3d11BufferDesc, &d3d11SubResourceData, &pID3D11Buffer_VertexBuffer));


	// Bind Vertex buffer to pipeline
	const UINT stride = sizeof(Vertex);
	const UINT offset = 0u;

	pID3D11DeviceContext->IASetVertexBuffers(0u, 1u, pID3D11Buffer_VertexBuffer.GetAddressOf(), &stride, &offset);

	// craeate index buffer
	const unsigned short indices[] =
	{
		0, 2, 1, 2, 3, 1,
		1, 3, 5, 3, 7, 5,
		2, 6, 3, 3, 6, 7,
		4, 5, 7, 4, 7, 6,
		0, 4, 2, 2, 4, 6,
		0, 1, 4, 1, 5, 4,
	};
	
	wrl::ComPtr<ID3D11Buffer> pID3D11Buffer_IndexBuffer;

	d3d11BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	d3d11BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3d11BufferDesc.CPUAccessFlags = 0u;
	d3d11BufferDesc.MiscFlags = 0u;
	d3d11BufferDesc.ByteWidth = sizeof(indices);
	d3d11BufferDesc.StructureByteStride = sizeof(unsigned short);

	d3d11SubResourceData.pSysMem = indices;
	GFX_THROW_INFO(pID3D11Device->CreateBuffer(&d3d11BufferDesc, &d3d11SubResourceData, &pID3D11Buffer_IndexBuffer));

	// bind index buffer
	pID3D11DeviceContext->IASetIndexBuffer(pID3D11Buffer_IndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);

	// create constant buffer for transformation matrix
	struct ConstantBuffer
	{
		dx::XMMATRIX transform;
	};

	const ConstantBuffer constantBuffer =
	{
		dx::XMMatrixTranspose
		(
			dx::XMMatrixRotationZ(angle) * 
			dx::XMMatrixRotationX(angle) *
			dx::XMMatrixTranslation(x, 0.0f, z + 4.0f) * 
			dx::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 10.0f)
		)
	};

	wrl::ComPtr<ID3D11Buffer> pID3D11Buffer_ConstantBuffer;

	d3d11BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	d3d11BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3d11BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	d3d11BufferDesc.MiscFlags = 0u;
	d3d11BufferDesc.ByteWidth = sizeof(constantBuffer);
	d3d11BufferDesc.StructureByteStride = 0u;

	d3d11SubResourceData.pSysMem = &constantBuffer;

	GFX_THROW_INFO(pID3D11Device->CreateBuffer(&d3d11BufferDesc, &d3d11SubResourceData, &pID3D11Buffer_ConstantBuffer));

	// bind the constant buffer
	pID3D11DeviceContext->VSSetConstantBuffers(0u, 1u, pID3D11Buffer_ConstantBuffer.GetAddressOf());

	// create vertex shader
	wrl::ComPtr<ID3D11VertexShader> pID3D11VertexShader;
	wrl::ComPtr<ID3DBlob> pID3DBlob;

	GFX_THROW_INFO(D3DReadFileToBlob(L"VertexShader.cso", &pID3DBlob)); // CSO - Compiled shader object
	GFX_THROW_INFO(pID3D11Device->CreateVertexShader(pID3DBlob->GetBufferPointer(), pID3DBlob->GetBufferSize(), nullptr, &pID3D11VertexShader));

	// bind vertex shader
	pID3D11DeviceContext->VSSetShader(pID3D11VertexShader.Get(), nullptr, 0u);

	// input layout object (2D position only)
	wrl::ComPtr<ID3D11InputLayout> pID3D11InputLayout;

	const D3D11_INPUT_ELEMENT_DESC d3d11InputElementDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	GFX_THROW_INFO(pID3D11Device->CreateInputLayout
	(
		d3d11InputElementDesc,
		static_cast<UINT>(std::size(d3d11InputElementDesc)),
		pID3DBlob->GetBufferPointer(),
		pID3DBlob->GetBufferSize(),
		&pID3D11InputLayout
	));

	pID3D11DeviceContext->IASetInputLayout(pID3D11InputLayout.Get());

	struct ConstantBuffer2
	{
		struct
		{
			float r;
			float g;
			float b;
			float a;
		}  face_colors[6];
	};

	const ConstantBuffer2 constantBuffer2 =
	{
		{
			{ 1.0f, 0.0f, 1.0f },
			{ 1.0f, 0.0f, 0.0f },
			{ 0.0f, 1.0f, 0.0f },
			{ 0.0f, 0.0f, 1.0f },
			{ 1.0f, 1.0f, 0.0f },
			{ 0.0f, 1.0f, 1.0f },
		}
	};

	// create the second constant buffer
	wrl::ComPtr<ID3D11Buffer> pID3D11Buffer_ConstantBuffer2;
	d3d11BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	d3d11BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3d11BufferDesc.CPUAccessFlags = 0u;
	d3d11BufferDesc.MiscFlags = 0u;
	d3d11BufferDesc.ByteWidth = sizeof(ConstantBuffer2);
	d3d11BufferDesc.StructureByteStride = 0u;

	d3d11SubResourceData.pSysMem = &constantBuffer2;

	GFX_THROW_INFO(pID3D11Device -> CreateBuffer(&d3d11BufferDesc, &d3d11SubResourceData, pID3D11Buffer_ConstantBuffer2.GetAddressOf()));

	// bind the second constant buffer to pixel
	pID3D11DeviceContext->PSSetConstantBuffers(0u, 1u, pID3D11Buffer_ConstantBuffer2.GetAddressOf());

	// create pixel shader
	wrl::ComPtr<ID3D11PixelShader> pID3D11PixelShader;
	

	GFX_THROW_INFO(D3DReadFileToBlob(L"PixelShader.cso", &pID3DBlob));
	GFX_THROW_INFO(pID3D11Device->CreatePixelShader(pID3DBlob->GetBufferPointer(), pID3DBlob->GetBufferSize(), nullptr, &pID3D11PixelShader));

	// bind pixel shader
	pID3D11DeviceContext->PSSetShader(pID3D11PixelShader.Get(), nullptr, 0u);

	
	// bind render target
	pID3D11DeviceContext->OMSetRenderTargets(1u, pID3D11RenderTargetView.GetAddressOf(), pID3D11DepthStencilView.Get()); // OM - Output Merger

	// Set primitive topology to triangle list
	pID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// configure viewport
	D3D11_VIEWPORT d3d11Viewport{};
	d3d11Viewport.Width = 800;
	d3d11Viewport.Height = 600;
	d3d11Viewport.MinDepth = 0;
	d3d11Viewport.MaxDepth = 1;
	d3d11Viewport.TopLeftX = 0;
	d3d11Viewport.TopLeftY = 0;

	pID3D11DeviceContext->RSSetViewports(1u, &d3d11Viewport); // RS - Rasterizer Stage

	GFX_THROW_INFO_ONLY(pID3D11DeviceContext->DrawIndexed(static_cast<UINT>(std::size(indices)), 0u, 0u));
}

Graphics::HrException::HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMessages) noexcept : Exception(line, file), hr(hr)
{
	// join all info messages with newline into single string
	for (const auto& message : infoMessages)
	{
		info += message;
		info.push_back('\n');
	}

	// remove final new line if exists
	if (!info.empty())
		info.pop_back();
}

const char* Graphics::HrException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << static_cast<unsigned long>(GetErrorCode()) << ")" << std::endl
		<< "[Error String] " << GetErrorString() << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl
		<< GetOriginString();

	if (!info.empty())
	{
		oss << "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	}

	oss << GetOriginString();
	whatBuffer = oss.str();

	return whatBuffer.c_str();
}

const char* Graphics::HrException::GetType() const noexcept
{
	return "Chilli Graphics Exception";
}

HRESULT Graphics::HrException::GetErrorCode() const noexcept
{
	return hr;
}

std::string Graphics::HrException::GetErrorString() const noexcept
{
	return DXGetErrorString(hr);
}

std::string Graphics::HrException::GetErrorDescription() const noexcept
{
	char buffer[512];
	DXGetErrorDescription(hr, buffer, sizeof(buffer));
	return buffer;
}

std::string Graphics::HrException::GetErrorInfo() const noexcept
{
	return info;
}

const char* Graphics::DeviceRemovedException::GetType() const noexcept
{
	return "Chilli Graphics Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}

Graphics::InfoException::InfoException(int line, const char* file, std::vector<std::string> infoMessages) noexcept :
	Exception(line, file)
{
	for (const auto& message : infoMessages)
	{
		info += message;
		info.push_back('\n');
	}

	if (!info.empty())
		info.pop_back();
}

const char* Graphics::InfoException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;

	oss << GetOriginString();

	whatBuffer = oss.str();
	return whatBuffer.c_str();
}


const char* Graphics::InfoException::GetType() const noexcept
{
	return "Chilli Graphics Info Exception";
}

std::string Graphics::InfoException::GetErrorInfo() const noexcept
{
	return info;
}
