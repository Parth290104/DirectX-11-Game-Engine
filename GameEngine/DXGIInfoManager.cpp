#include "DxgiInfoManager.h"
#include "Window.h"
#include "Graphics.h"
#include <dxgidebug.h>
#include <memory>
#include "WindowsThrowMacros.h"

#pragma comment(lib, "dxguid.lib")

#define GFX_THROW_NOINFO(hrcall) if(FAILED(hr = (hrcall))) throw Graphics::HrException(__LINE__, __FILE__, hr, {})

DXGIInfoManager::DXGIInfoManager()
{
	// define the function signature of DXGIGetDebugInterface
	typedef HRESULT(WINAPI* DXGIGetDebugInterface)(REFIID, void**);

	// load the dll that contains the function DXGIGetDebugInterface
	const auto hModDXGIDebug = LoadLibraryEx("dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
	if (hModDXGIDebug == nullptr)
	{
		throw CHWND_LAST_EXCEPT();
	}

	// get address of DXGIGetDebugInterface in dll
	const auto pDXGIGetDebugInterface = reinterpret_cast<DXGIGetDebugInterface>
		(reinterpret_cast<void*>(GetProcAddress(hModDXGIDebug, "DXGIGetDebugInterface")));

	if(pDXGIGetDebugInterface == nullptr)
	{
		throw CHWND_LAST_EXCEPT();
	}

	HRESULT hr;
	GFX_THROW_NOINFO(pDXGIGetDebugInterface(__uuidof(IDXGIInfoQueue), &pIDXGIInfoQueue));
}

void DXGIInfoManager::Set() noexcept
{
	// set the index (next) so that the next all to GetMessages()
	// will only get errors generated after this call
	next = pIDXGIInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
}

std::vector<std::string> DXGIInfoManager::GetMessages() const
{
	std::vector<std::string> messages;
	const auto end = pIDXGIInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
	for (auto i = next; i < end; ++i)
	{
		HRESULT hr;
		SIZE_T messageLength;

		GFX_THROW_NOINFO(pIDXGIInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, nullptr, &messageLength));

		// allocate memory for message
		auto bytes = std::make_unique<byte[]>(messageLength);
		auto pMessage = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(bytes.get());

		// get the message and push its description into the vector
		GFX_THROW_NOINFO(pIDXGIInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, pMessage, &messageLength));
		messages.emplace_back(pMessage->pDescription);
	}

	return messages;
}
