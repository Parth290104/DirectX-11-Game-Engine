#pragma once
#include "ChilliWin.h"
#include <string>
#include <vector>
#include <dxgidebug.h>
#include <wrl/client.h>

class DXGIInfoManager
{
public:
	DXGIInfoManager();
	~DXGIInfoManager() = default;
	DXGIInfoManager(const DXGIInfoManager&) = delete;
	DXGIInfoManager& operator=(const DXGIInfoManager&) = delete;
	void Set() noexcept;
	std::vector<std::string> GetMessages() const;

private:
	unsigned long long next = 0u;
	Microsoft::WRL::ComPtr<IDXGIInfoQueue> pIDXGIInfoQueue = nullptr;
};
