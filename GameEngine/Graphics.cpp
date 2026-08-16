#include "Graphics.h"
#include "dxerr.h"
#include <sstream>
#include "GraphicsThrowMacros.h"

namespace wrl = Microsoft::WRL;
namespace dx = DirectX;

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

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
		pIDXGISwapChain.GetAddressOf(),
		pID3D11Device.GetAddressOf(),
		nullptr,
		pID3D11DeviceContext.GetAddressOf()
	));

	// gain access to texture subresource in swap chain (back buffer)
	wrl::ComPtr<ID3D11Resource> pID3D11Resource_BackBuffer = nullptr;

	GFX_THROW_INFO(pIDXGISwapChain->GetBuffer(
		0,
		__uuidof(ID3D11Resource),
		reinterpret_cast<void**>(pID3D11Resource_BackBuffer.GetAddressOf())
	));

	GFX_THROW_INFO(pID3D11Device->CreateRenderTargetView(
		pID3D11Resource_BackBuffer.Get(),
		nullptr,
		pID3D11RenderTargetView.GetAddressOf()
	));

	// create depth stencil buffer
	D3D11_DEPTH_STENCIL_DESC d3d11DepthStencilDesc{};
	d3d11DepthStencilDesc.DepthEnable = TRUE;
	d3d11DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	d3d11DepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	wrl::ComPtr<ID3D11DepthStencilState> pID3D11DepthStencilState;
GFX_THROW_INFO(pID3D11Device->CreateDepthStencilState(&d3d11DepthStencilDesc, pID3D11DepthStencilState.GetAddressOf()));

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

	GFX_THROW_INFO(pID3D11Device -> CreateTexture2D(&d3d11Texture2DDesc, nullptr, pID3D11Texture2D.GetAddressOf()));

	// create view of depth stencil texture
	D3D11_DEPTH_STENCIL_VIEW_DESC d3d11DepthStencilViewDesc{};
	d3d11DepthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	d3d11DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	d3d11DepthStencilViewDesc.Texture2D.MipSlice = 0u;

	GFX_THROW_INFO(pID3D11Device -> CreateDepthStencilView(pID3D11Texture2D.Get(), &d3d11DepthStencilViewDesc, pID3D11DepthStencilView.GetAddressOf()));

	// bind depth stencil view to Output Merger
	pID3D11DeviceContext->OMSetRenderTargets(1u, pID3D11RenderTargetView.GetAddressOf(), pID3D11DepthStencilView.Get());

	D3D11_VIEWPORT d3d11Viewport{};
	d3d11Viewport.Width = 800.0f;
	d3d11Viewport.Height = 600.0f;
	d3d11Viewport.MinDepth = 0.0f;
	d3d11Viewport.MaxDepth = 1.0f;
	d3d11Viewport.TopLeftX = 0.0f;
	d3d11Viewport.TopLeftY = 0.0f;

	pID3D11DeviceContext->RSSetViewports(1u, &d3d11Viewport);
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

void Graphics::DrawIndexed(UINT count)
{
	GFX_THROW_INFO_ONLY(pID3D11DeviceContext->DrawIndexed(count, 0u, 0u));
}

void Graphics::SetProjection(DirectX::FXMMATRIX _projectionMatrix) noexcept
{
	projectionMatrix = _projectionMatrix;
}

DirectX::XMMATRIX Graphics::GetProjection() const noexcept
{
	return projectionMatrix;
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
