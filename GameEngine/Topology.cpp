#include "Topology.h"

Topology::Topology(Graphics& graphicsObject, D3D11_PRIMITIVE_TOPOLOGY type) : type(type)
{

}

void Topology::Bind(Graphics& graphicsObject) noexcept
{
	GetContext(graphicsObject)->IASetPrimitiveTopology(type);
}
