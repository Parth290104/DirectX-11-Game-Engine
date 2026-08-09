#include "App.h"

//LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
//{
//	static WindowsMessageMap windowsMessageMap;
//
//	OutputDebugString(windowsMessageMap(msg, lParam, wParam).c_str());
//
//	switch(msg)
//	{
//	case WM_CLOSE:
//		PostQuitMessage(0);
//		break;
//	}
//
//	return DefWindowProc(hWnd, msg, wParam, lParam);
//}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	try
	{
		return App{}.Go();
	}

	catch (const ChilliException& e)
	{
		MessageBox(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}

	catch (const std::exception& e)
	{
		MessageBox(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
	}

	catch (...)
	{
		MessageBox(nullptr, "No details available", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
	}

	return -1;
}
