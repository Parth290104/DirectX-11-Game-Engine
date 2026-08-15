#pragma once
#include "Bindable.h"
#include "GraphicsThrowMacros.h"

class VertexBuffer : public Bindable
{
public:
	template<class V>
	VertexBuffer(Graphics& graphicsObject, const std::vector<V>& vertices) : stride(sizeof(V))
	{
		INFOMAN(graphicsObject);

		D3D11_BUFFER_DESC d3d11BufferDesc{};
		d3d11BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		d3d11BufferDec.Usage = D3D11_USAGE_DEFAULT;
		d3d11BufferDesc.CPUAccessFlags = 0u;
		d3d11BufferDesc.MiscFlags = 0u;
		d3d11BufferDesc.ByteWidth = static_cast<UINT>(sizeof(V) * vertices.size());
		d3d11BufferDesc.StructureByteStride = sizeof(V);

		D3D11_SUBRESOURCE_DATA d3d11SubResourceData{};
		d3d11SubResourceData.pSysMem = vertices.data();

		GFX_THROW_INFO(GetDevice(graphcisObject)->CreateBuffer(&d3d11BufferDesc, &d3d11SubResourceData, &pID3D11VertexBuffer));
	}

	void Bind(Graphics& graphicsObject) noexcept override;

protected:
	UINT stride;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pID3D11Buffer_VertexBuffer;
};
