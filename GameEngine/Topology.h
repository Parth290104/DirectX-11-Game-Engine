#pragma once
#include "Bindable.h"

class Topology : public Bindable
{
public:
	Topology(Graphics& graphicsObject, D3D11_PRIMITIVE_TOPOLOGY type);
	void Bind(Graphics& graphicsObject) noexcept override;

protected:
	D3D11_PRIMITIVE_TOPOLOGY type;
};
