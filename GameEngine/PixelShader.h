#pragma once
#include "Bindable.h"

class PixelShader : public Bindable
{
public:
	PixelShader(Graphics& graphicsObject, const std::wstring& path);
	void Bind(Graphics& graphicsObject) noexcept override;

protected:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pID3D11PixelShader;
};