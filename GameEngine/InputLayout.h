#pragma once
#include "Bindable.h"

class InputLayout : public Bindable
{
public:
	InputLayout(Graphics& graphicsObject, const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout, ID3DBlob* pID3DBlob_VertexShaderByteCode);
	void Bind(Graphics& graphicsObject) noexcept override;

protected:
	Microsoft::WRL::ComPtr<ID3D11InputLayout> pID3D11InputLayout;
};
