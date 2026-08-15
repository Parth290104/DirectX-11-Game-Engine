#include "TransformCBuffer.h"

TransformCBuffer::TransformCBuffer(Graphics& graphicsObject, const Drawable& parent) : VertexConstantBuffer(graphicsObject), parent(parent)
{

}

void TransformCBuffer::Bind(Graphics& graphicsObject) noexcept
{
	VertexConstantBuffer.Update(graphicsObject, DirectX::XMMatrixTranspose(parent.GetTransformXM() * graphicsObject.GetProjection()));

	return VertexConstantBuffer.Bind(graphicsObject);
}
