#pragma once
#include "Bindable.h"
#include "GraphicsThrowMacros.h"

template<typename C>
class ConstantBuffer : public Bindable
{
public:
	void Update(Graphics& graphicsObject, const C& constants)
	{
		INFOMAN(graphicsObject);

		D3D11_MAPPED_SUBRESOURCE d3d11MappedSubResource{};
		GFX_THROW_INFO(GetContext(graphicsObject) -> Map(pID3D11Buffer_ConstantBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &d3d11MappedSubResource));

		memcpy(d3d11MappedSubResource.pData, &constants, sizeof(constants));
		GetContext(graphicsObject)-> Unmap(pID3D11Buffer_ConstantBuffer.Get(), 0u);
	}

	ConstantBuffer(Graphics& graphicsObject, const C& constants)
	{
		INFOMAN(graphicsObject);

		D3D11_BUFFER_DESC d3d11BufferDesc{};
		d3d11BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		d3d11BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		d3d11BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		d3d11BufferDesc.ByteWidth = sizeof(constants);
		d3d11BufferDesc.StructureByteStride = 0u;

		D3D11_SUBRESOURCE_DATA d3d11SubResourceData{};
		d3d11SubResourceData.pSysMem = &constants;
		GFX_THROW_INFO(GetDevice(graphicsObject) -> CreateBuffer(&d3d11BufferDesc, &d3d11SubResourceData, &pID3D11Buffer_ConstantBuffer));
	}

	ConstantBuffer(Graphics& graphicsObject)
	{
		INFOMAN(graphicsObject);
		
		D3D11_BUFFER_DESC d3d11BufferDesc{};
		d3d11BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		d3d11BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		d3d11BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		d3d11BufferDesc.MiscFlags = 0u;
		d3d11BufferDesc.ByteWidth = sizeof(C);
		d3d11BufferDesc.StructureByteStride = 0u;

		GFX_THROW_INFO(GetDevice(graphicsObject) -> CreateBuffer(&d3d11BufferDesc, nullptr, &pID3D11Buffer_ConstantBuffer));
	}

protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> pID3D11Buffer_ConstantBuffer;
};

template<typename C>
class VertexConstantBuffer : public ConstantBuffer<C>
{
	using ConstantBuffer<C>::pID3D11Buffer_ConstantBuffer;
	using Bindable::GetContext;

public:
	using ConstantBuffer<C>::ConstantBuffer;
	void Bind(Graphics& graphicsObject) noexcept override
	{
		GetContext(graphicsObject)->VSSetConstantBuffers(0u, 1u, pID3D11Buffer_ConstantBuffer.GetAddressOf());
	}
};

template<typename C>
class PixelConstantBuffer : public ConstantBuffer<C>
{
	using ConstantBuffer<C>::pID3D11Buffer_ConstantBuffer;
	using Bindable::GetContext;

public:
	using ConstantBuffer<C>::ConstantBuffer;
	void Bind(Graphics& graphicsObject) noexcept override
	{
		GetContext(graphicsObject)->PSSetConstantBuffers(0u, 1u, pID3D11Buffer_ConstantBuffer.GetAddressOf());
	}
};
