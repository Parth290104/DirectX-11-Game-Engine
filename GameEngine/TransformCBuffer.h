#pragma once
#include "ConstantBuffers.h"
#include "Drawable.h"
#include <DirectXMath.h>

class TransformCBuffer : public Bindable
{
public:
	TransformCBuffer(Graphics& graphicsObject, const Drawable& parent);
	void Bind(Graphics& graphicsObject) noexcept override;

private:
	static std::unique_ptr<VertexConstantBuffer<DirectX::XMMATRIX>> pVertexConstantBuffer;
	const Drawable& parent;
};
