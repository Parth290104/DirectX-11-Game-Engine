#pragma once
#include "ChilliWin.h"
#include "ChilliException.h"
#include <d3d11.h>
#include <vector>
#include "DXGIInfoManager.h"
#include <wrl/client.h>

class Graphics
{
public:
	class Exception : public ChilliException
	{
		using ChilliException::ChilliException;
	};
	class HrException : public Exception
	{
	public:
		HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMessages) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
		std::string GetErrorDescription() const noexcept;
		std::string GetErrorInfo() const noexcept;
	private:
		HRESULT hr;
		std::string info;
	};

	class InfoException : public Exception
	{
	public:
		InfoException(int line, const char* file, std::vector<std::string> infoMessages) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		std::string GetErrorInfo() const noexcept;

	private:
		std::string info;
	};

	class DeviceRemovedException : public HrException
	{
		using HrException::HrException;
	public:
		const char* GetType() const noexcept override;
	private:
		std::string reason;
	};

public:
	Graphics(HWND hWnd);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics() = default;
	void EndFrame();
	void ClearBuffer(float red, float green, float blue) noexcept;
	void DrawTestTriangle(float angle, float x, float z);

private:
#ifndef NDEBUG
	DXGIInfoManager dxgiInfoManager;
#endif
	Microsoft::WRL::ComPtr<ID3D11Device> pID3D11Device = nullptr;
	Microsoft::WRL::ComPtr<IDXGISwapChain> pIDXGISwapChain = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pID3D11DeviceContext = nullptr;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pID3D11RenderTargetView = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pID3D11DepthStencilView = nullptr;
};

