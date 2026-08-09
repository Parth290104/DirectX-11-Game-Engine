#include "Window.h"
#include <sstream>
#include "Resource.h"

Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass() noexcept : hInst(GetModuleHandle(nullptr))
{
	WNDCLASSEX wc = {0};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMsgSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetInstance();
	wc.hIcon = static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32, 32, 0));
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = GetName();
	wc.hIconSm = static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 16, 16, 0));
	RegisterClassEx(&wc);
}

Window::WindowClass::~WindowClass()
{
	UnregisterClass(GetName(), GetInstance());
}

const char* Window::WindowClass::GetName() noexcept
{
	return wndClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
	return wndClass.hInst;
}

Window::Window(int width, int height, const char* name) : width(width), height(height)
{
	RECT wr{};
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top;
	if (AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE) == 0)
	{
		throw CHWND_LAST_EXCEPT();
	}

	// create window and get hWnd
	hWnd = CreateWindow(
		WindowClass::GetName(), name,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top,
		nullptr, nullptr, WindowClass::GetInstance(), this
	);

	// Check for error
	if (hWnd == nullptr)
	{
		throw CHWND_LAST_EXCEPT();
	}

	ShowWindow(hWnd, SW_SHOWDEFAULT);
}

Window::~Window()
{
	DestroyWindow(hWnd);
}

void Window::setTitle(const std::string& title) const
{
	if (SetWindowText(hWnd, title.c_str()) == 0)
	{
		throw CHWND_LAST_EXCEPT();
	}
}

std::optional<int> Window::ProcessMessages()
{
	MSG msg;

	// while queue has messages, remove and dispatch them (but do not block if there are no messages)
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		// check for quit because PeekMessage does not signal this via return value
		if (msg.message == WM_QUIT)
			return static_cast<int>(msg.wParam);

		// TranslateMessage will post auxiliary WM_CHAR messages from key msgs
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return {};
}

LRESULT CALLBACK Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// use create parameter passed in from CreateWindow() to store window class pointer at WinAPI-managed user data slot
	if (msg == WM_NCCREATE) // NCCREATE - non client create, sent prior to WM_CREATE, 
	// allows us to set up the window class pointer before any other messages are sent
	{
		// CREATESTRUCT -> structure containing the information about the window that is created
		// extract pointer to window class from creation data
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);

		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams); // lpCreateParams -> last param passed to CreateWindow

		// set WinAPI-managed user data to store pointer to window class
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));

		// set message proc to normal (non-setup) handler now that setup is finished 
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));

		// forward message to window class handler
		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}

	// if we get a message before the WM_NCCREATE message, handle with default handler
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// retrieve pointer to window class
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

	// forward message to window class handler
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	switch (msg)
	{
	// return 0, because we dont want the control flow to go to DefWindowProc, and the window must be destroyed by the destructor
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
		break;

	// clear keystate when window loses focus to prevent input getting locked in
	case WM_KILLFOCUS:
		keyboard.ClearState();
		break;

	// KEYBOARD MESSAGES
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN: // syskey commands need to be handled to track ALT key(VK_MENU) and F10
		// filter autorepeat key events
		if(!(lParam & 0x40000000) || keyboard.AutorepeatIsEnabled())
			keyboard.OnKeyPressed(static_cast<unsigned char>(wParam));
		break;

	case WM_KEYUP:
	case WM_SYSKEYUP:
		keyboard.OnKeyReleased(static_cast<unsigned char>(wParam));
		break;

	case WM_CHAR:
		keyboard.OnChar(static_cast<unsigned char>(wParam));
		break;
	// END KEYBOARD MESSAGES

	// START MOUSE MESSAGES
	case WM_MOUSEMOVE:
	{
		const POINTS points = MAKEPOINTS(lParam);
		if (points.x >= 0 && points.y < width && points.y >= 0 && points.y < height)
		{
			mouse.OnMouseMove(points.x, points.y);
			if (!mouse.IsInWindow())
			{
				SetCapture(hWnd);
				mouse.OnMouseEnter();
			}
		}
		
		else
		{
			if (wParam & (MK_LBUTTON | MK_RBUTTON))
			{
				mouse.OnMouseMove(points.x, points.y);
			}

			else
			{
				ReleaseCapture();
				mouse.OnMouseLeave();
			}
		}
		break;
	}

	case WM_LBUTTONDOWN:
	{
		const POINTS points = MAKEPOINTS(lParam);
		mouse.OnLeftPressed(points.x, points.y);
		break;
	}

	case WM_RBUTTONDOWN:
	{
		const POINTS points = MAKEPOINTS(lParam);
		mouse.OnRightPressed(points.x, points.y);
		break;
	}

	case WM_LBUTTONUP:
	{
		const POINTS points = MAKEPOINTS(lParam);
		mouse.OnLeftReleased(points.x, points.y);
		break;
	}

	case WM_RBUTTONUP:
	{
		const POINTS points = MAKEPOINTS(lParam);
		mouse.OnRightReleased(points.x, points.y);
		break;
	}

	case WM_MOUSEWHEEL:
	{
		const POINTS points = MAKEPOINTS(lParam);
		const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
		mouse.OnWheelDelta(points.x, points.y, delta);
		break;
	}
	// END MOUSE MESSGAES

	default:
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

Window::Exception::Exception(int lineNumber, const char* file, HRESULT hr) noexcept: ChilliException(lineNumber, file), hr(hr)
{

}

const char* Window::Exception::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code]" << GetErrorCode() << std::endl
		<< "[Description] " << GetErrorString() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();

	return whatBuffer.c_str();
}

const char* Window::Exception::GetType() const noexcept
{
	return "Chilli Window Exception";
}

std::string Window::Exception::TranslateErrorCode(HRESULT hr) noexcept
{
	char* pMessageBuffer = nullptr;
	DWORD nMessageLength = FormatMessage
	(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,
		hr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&pMessageBuffer),
		0,
		nullptr
	);

	if (nMessageLength == 0)
	{
		return "Unidentified error code";
	}

	std::string errorString = pMessageBuffer;
	LocalFree(pMessageBuffer);
	return errorString;
}

HRESULT Window::Exception::GetErrorCode() const noexcept
{
	return hr;
}

std::string Window::Exception::GetErrorString() const noexcept
{
	return TranslateErrorCode(hr);
}
