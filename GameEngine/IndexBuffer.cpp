#include "IndexBuffer.h"
#include "GraphicsThrowMacros.h"

IndexBuffer::IndexBuffer(Graphics& graphicsObject, const std::vector<unsigned short>& indices) : count(static_cast<UINT>(indices.size()))
{
	INFOMAN(graphicsObject);

	D3D11_BUFFER_DESC d3d11BufferDesc{};
	d3d11BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	d3d11BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3d11BufferDesc.CPUAccessFlags = 0u;
	d3d11BufferDesc.MiscFlags = 0u;
	d3d11BufferDesc.ByteWidth = static_cast<UINT>(count * sizeof(unsigned short));
	d3d11BufferDesc.StructureByteStride = sizeof(unsigned short);
	D3D11_SUBRESOURCE_DATA d3d11SubResourceData{};
	d3d11SubResourceData.pSysMem = indices.data();

	GFX_THROW_INFO(GetDevice(graphicsObject)->CreateBuffer(&d3d11BufferDesc, &d3d11SubResourceData, &pI3D11Buffer_IndexBuffer));
}

void IndexBuffer::Bind(Graphics& graphicsObject) noexcept
{
	GetContext(graphicsObject)->IASetIndexBuffer(pI3D11Buffer_IndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);
}

UINT IndexBuffer::GetCount() const noexcept
{
	return count;
}
