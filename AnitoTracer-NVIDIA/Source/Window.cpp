#include "Utils.h"
#include "Window.h"
#include <codecvt>
#include "Win32Application.h"

using namespace Microsoft::WRL;

Window::Window(const UINT width, const UINT height, const std::wstring name) :
	m_width(width),
	m_height(height),
	m_useWarpDevice(false),
	m_title(name)
{
	m_aspectRatio = static_cast<float>(width) / static_cast<float>(height);
}

// Helper function for setting the window's title text.
void Window::SetCustomWindowText(const LPCWSTR text) const
{
	const std::wstring windowText = m_title + L": " + text;
	SetWindowText(Win32Application::GetHwnd(), windowText.c_str());
}

void Window::msgBox(const std::string& msg)
{
	MessageBoxA(Win32Application::GetHwnd(), msg.c_str(), "Error", MB_OK);
}

void Window::d3dTraceHR(const std::string& msg, const HRESULT hr)
{
	char hr_msg[512];
	FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, hr, 0, hr_msg, NV_ARRAYSIZE(hr_msg), nullptr);

	const std::string error_msg = msg + ".\nError! " + hr_msg;
	msgBox(error_msg);
}

std::wstring string_to_wstring(const std::string& str)
{
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.length(), NULL, 0);
	std::wstring wstr(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.length(), &wstr[0], size_needed);
	return wstr;
}

std::string wstring_to_string(const std::wstring& wstr)
{
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.length(), NULL, 0, NULL, NULL);
	std::string str(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.length(), &str[0], size_needed, NULL, NULL);
	return str;
}

// Helper function for parsing any supplied command line args.
_Use_decl_annotations_

void Window::ParseCommandLineArgs(WCHAR* argv[], const int argc)
{
	for (int i = 1; i < argc; ++i)
	{
		if (_wcsnicmp(argv[i], L"-warp", wcslen(argv[i])) == 0 ||
			_wcsnicmp(argv[i], L"/warp", wcslen(argv[i])) == 0)
		{
			m_useWarpDevice = true;
			m_title = m_title + L" (WARP)";
		}
	}
}
