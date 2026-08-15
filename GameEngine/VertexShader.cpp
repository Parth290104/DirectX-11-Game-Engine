#include "VertexShader.h"
#include "GraphicsThrowMacros.h"

VertexShader::VertexShader(Graphics& graphicsObject, const std::wstring& path)
{
	INFOMAN(graphicsObject);

	GFX_THROW_INFO(D3DReadFileToBlob(path.c_str(), &pID3DBlob_ByteCodeBlob));
	GFX_THROW_INFO(GetDevice(graphicsObject)->CreateVertexShader(
		pID3DBlob_ByteCodeBlob->GetBufferPointer(),
		pID3DBlob_ByteCodeBlob->GetBufferSize(),
		nullptr,
		&pID3D11VertexShader));
}

void VertexShader::Bind(Graphics& graphicsObject) noexcept
{
	GetContext(graphicsObject)->VSSetShader(pID3D11VertexShader.Get(), nullptr, 0u);
}

ID3DBlob* VertexShader::GetByteCode() const noexcept
{
	return pID3DBlob_ByteCodeBlob.Get();
}
