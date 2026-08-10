#pragma once
#include "ChilliWin.h"
#include <string>
#include <vector>

class DXGIInfoManager
{
public:
	DXGIInfoManager();
	~DXGIInfoManager();
	DXGIInfoManager(const DXGIInfoManager&) = delete;
	DXGIInfoManager& operator=(const DXGIInfoManager&) = delete;
	void Set() noexcept;
	std::vector<std::string> GetMessages() const;

private:
	unsigned long long next = 0u;
	struct IDXGIInfoQueue* pIDXGIInfoQueue = nullptr;
};
