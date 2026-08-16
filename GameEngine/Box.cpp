#include "Box.h"
#include "BindableBase.h"
#include "GraphicsThrowMacros.h"

namespace
{
	struct Vertex
	{
		struct
		{
			float x;
			float y;
			float z;
		} position;
	};

}

Box::Box(Graphics& graphicsObject, std::mt19937& rng,
	std::uniform_real_distribution<float>& adist,
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist) : 
	r(rdist(rng)),
	deltaRoll(ddist(rng)),
	deltaPitch(ddist(rng)),
	deltaYaw(ddist(rng)),
	deltaPhi(odist(rng)),
	deltaTheta(odist(rng)),
	deltaChi(odist(rng)),
	chi(adist(rng)),
	theta(adist(rng)),
	phi(adist(rng))
{
	struct Vertex
	{
		struct
		{
			float x;
			float y;
			float z;
		}position;
	};

	const std::vector<Vertex> vertices = 
	{
		{ -1.0f,-1.0f,-1.0f },
		{ 1.0f,-1.0f,-1.0f },
		{ -1.0f,1.0f,-1.0f },
		{ 1.0f,1.0f,-1.0f },
		{ -1.0f,-1.0f,1.0f },
		{ 1.0f,-1.0f,1.0f },
		{ -1.0f,1.0f,1.0f },
		{ 1.0f,1.0f,1.0f },
	};

	AddBind(std::unique_ptr<VertexBuffer>(new VertexBuffer(graphicsObject, vertices)));

	auto pVertexShader = std::unique_ptr<VertexShader>(new VertexShader(graphicsObject, L"VertexShader.cso"));
	auto pVertexShaderByteCode = pVertexShader->GetByteCode();
	AddBind(std::move(pVertexShader));

	AddBind(std::unique_ptr<PixelShader>(new PixelShader(graphicsObject, L"PixelShader.cso")));

	const std::vector<unsigned short> indices =
	{
		0,2,1, 2,3,1,
		1,3,5, 3,7,5,
		2,6,3, 3,6,7,
		4,5,7, 4,7,6,
		0,4,2, 2,4,6,
		0,1,4, 1,5,4
	};


	AddIndexBuffer(std::unique_ptr<IndexBuffer>(new IndexBuffer(graphicsObject, indices)));
	// Constant buffer data
	struct ConstantBuffer2
	{
		struct
		{
			float r;
			float g;
			float b;
			float a;
		} face_colors[6];
	};

	const ConstantBuffer2 constantBuffer2 =
	{
		{
			{ 1.0f,0.0f,1.0f },
			{ 1.0f,0.0f,0.0f },
			{ 0.0f,1.0f,0.0f },
			{ 0.0f,0.0f,1.0f },
			{ 1.0f,1.0f,0.0f },
			{ 0.0f,1.0f,1.0f },
		}
	};

	AddBind(std::unique_ptr<PixelConstantBuffer<ConstantBuffer2>>(new PixelConstantBuffer<ConstantBuffer2>(graphicsObject, constantBuffer2)));

	const std::vector<D3D11_INPUT_ELEMENT_DESC> d3d11InputElementDescVector =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	
	AddBind(std::unique_ptr<InputLayout>(new InputLayout(graphicsObject, d3d11InputElementDescVector, pVertexShaderByteCode)));

	AddBind(std::unique_ptr<Topology>(new Topology(graphicsObject, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)));

	AddBind(std::unique_ptr<TransformCBuffer>(new TransformCBuffer(graphicsObject, *this)));
}

void Box::Update(float dt) noexcept
{
	roll += deltaRoll * dt;
	pitch += deltaPitch * dt;
	yaw += deltaYaw * dt;
	theta += deltaTheta * dt;
	phi += deltaPhi * dt;
	chi += deltaChi * dt;
}

DirectX::XMMATRIX Box::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
		DirectX::XMMatrixTranslation(r, 0.0f, 0.0f) *
		DirectX::XMMatrixRotationRollPitchYaw(theta, phi, chi) *
		DirectX::XMMatrixTranslation(0.0f, 0.0f, 20.0f);
}
