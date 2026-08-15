#pragma once
// graphics exception checking/throwing macros (some dxgi infos)
#define GFX_EXCEPT_NOINFO(hr) Graphics::HrException(__LINE__, __FILE__, (hr))
#define GFX_THROW_NOINFO(hrcall) if(FAILED(hr = (hrcall))) throw Graphics::HrException(__LINE__, __FILE__, (hr))

#ifndef NDEBUG
#define GFX_EXCEPT(hr) Graphics::HrException(__LINE__, __FILE__, (hr), dxgiInfoManager.GetMessages())
#define GFX_THROW_INFO(hrcall) dxgiInfoManager.Set(); if(FAILED(hr = (hrcall))) throw GFX_EXCEPT(hr)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException(__LINE__, __FILE__, (hr), dxgiInfoManager.GetMessages())
#define GFX_THROW_INFO_ONLY(call) \
	do { \
		dxgiInfoManager.Set(); \
		call; \
		auto v = dxgiInfoManager.GetMessages(); \
		if(!v.empty()) { throw Graphics::InfoException(__LINE__, __FILE__, v); } \
	} while(0)
#else
#define GFX_EXCEPT(hr) Graphics::HrException(__LINE__, __FILE__, (hr))
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException(__LINE__, __FILE__, (hr))
#define GFX_THROW_INFO_ONLY(call) (call)
#endif

#ifdef NDEBUG
#define INFOMAN(graphicsObject) HRESULT hr
#else
#define INFOMAN(graphicsObject) HRESULT hr; DXGIInfoManager& dxgiInfoManager = GetInfoManager((graphicsObject))
#endif
