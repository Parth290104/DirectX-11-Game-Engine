#include "InputLayout.h"
#include "GraphicsThrowMacros.h"

InputLayout::InputLayout(Graphics& graphicsObject, const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout, ID3DBlob* pID3DBlob_VertexShaderByteCode)
{
	INFOMAN(graphicsObject);

	GFX_THROW_INFO(GetDevice(graphicsObject) -> CreateInputLayout(layout.data(), static_cast<UINT>(layout.size()),
		pID3DBlob_VertexShaderByteCode -> GetBufferPointer(), pID3DBlob_VertexShaderByteCode -> GetBufferSize(), &pID3D11InputLayout));
}

void InputLayout::Bind(Graphics& graphicsObject) noexcept
{
	GetContext(graphicsObject)->IASetInputLayout(pID3D11InputLayout.Get());
}
