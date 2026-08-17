#include "Bindable.h"

ID3D11DeviceContext* Bindable::GetContext(Graphics& graphicsObject) noexcept
{
	return graphicsObject.pID3D11DeviceContext.Get();
}

ID3D11Device* Bindable::GetDevice(Graphics& graphicsObject) noexcept
{
	return graphicsObject.pID3D11Device.Get();
}

DXGIInfoManager& Bindable::GetInfoManager(Graphics& graphicsObject) noexcept(!IS_DEBUG)
{
#ifndef NDEBUG
	return graphicsObject.dxgiInfoManager;
#else
	throw std::logic_error("(Tried to access graphicsObject.dxgiInfoManager in Release Config)");
#endif
}
