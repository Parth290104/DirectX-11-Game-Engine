#include "TransformCBuffer.h"

TransformCBuffer::TransformCBuffer(Graphics& graphicsObject, const Drawable& parent) : parent(parent)
{
	if (!pVertexConstantBuffer)
	{
		pVertexConstantBuffer = std::make_unique<VertexConstantBuffer<DirectX::XMMATRIX>>(graphicsObject);
	}
}

void TransformCBuffer::Bind(Graphics& graphicsObject) noexcept
{
	pVertexConstantBuffer -> Update(graphicsObject, DirectX::XMMatrixTranspose(parent.GetTransformXM() * graphicsObject.GetProjection()));

	return pVertexConstantBuffer -> Bind(graphicsObject);
}

std::unique_ptr<VertexConstantBuffer<DirectX::XMMATRIX>> TransformCBuffer::pVertexConstantBuffer;
