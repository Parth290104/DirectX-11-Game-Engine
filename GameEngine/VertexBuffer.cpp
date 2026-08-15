#include "VertexBuffer.h"

void VertexBuffer::Bind(Graphics& graphicsObject) noexcept
{
	const UINT offset = 0u;
	GetContext(graphicsObject)->IASetVertexBuffers(0u, 1u, pID3D11Buffer_VertexBuffer.GetAddressOf(), &stride, &offset);
}
