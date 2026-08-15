#include "PixelShader.h"
#include "GraphicsThrowMacros.h"

PixelShader::PixelShader(Graphics& graphicsObject, const std::wstring& path)
{
	INFOMAN(graphicsObject);

	Microsoft::WRL::ComPtr<ID3DBlob> pID3DBlob;

	GFX_THROW_INFO(D3DReadFileToBlob(path.c_str(), &pID3DBlob));
	GFX_THROW_INFO(GetDevice(graphicsObject) -> CreatePixelShader
	(
		pID3DBlob ->GetBufferPointer(), pID3DBlob -> GetBufferSize(), 
		nullptr, &pID3D11PixelShader
	));
}

void PixelShader::Bind(Graphics& graphicsObject) noexcept
{
	GetContext(graphicsObject)->PSSetShader(pID3D11PixelShader.Get(), nullptr, 0u);
}
