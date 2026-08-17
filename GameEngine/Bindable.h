#pragma once
#include "Graphics.h"

class Bindable
{
public:
	virtual void Bind(Graphics& graphicsObject) noexcept = 0;
	virtual ~Bindable() = default;

protected:
	static ID3D11DeviceContext* GetContext(Graphics& graphicsObject) noexcept;
	static ID3D11Device* GetDevice(Graphics& graphicsObject) noexcept;
	static DXGIInfoManager& GetInfoManager(Graphics& graphicObject) noexcept(!IS_DEBUG);
};
