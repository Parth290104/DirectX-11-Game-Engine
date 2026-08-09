#pragma once
#include "ChilliWin.h"
#include "ChilliException.h"
#include <d3d11.h>

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
		HrException(int line, const char* file, HRESULT hr) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
		std::string GetErrorDescription() const noexcept;

	private:
		HRESULT hr;
	};

	class DeviceRemovedException : public HrException
	{
		using HrException::HrException;
	public:
		const char* GetType() const noexcept override;
	};

public:
	Graphics(HWND hWnd);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics();
	void EndFrame();
	void ClearBuffer(float red, float green, float blue) noexcept;

private:
	ID3D11Device* pID3D11Device = nullptr;
	IDXGISwapChain* pIDXGISwapChain = nullptr;
	ID3D11DeviceContext* pID3D11DeviceContext = nullptr;
	ID3D11RenderTargetView* pID3D11RenderTargetView = nullptr;
};

