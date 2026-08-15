#pragma once
#include "Bindable.h"

class IndexBuffer : public Bindable
{
public:
	IndexBuffer(Graphics& graphicsObject, const std::vector<unsigned short>& indices);
	void Bind(Graphics& graphicsObject) noexcept override;
	UINT GetCount() const noexcept;

private:
	UINT count;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pI3D11Buffer_IndexBuffer;
};
