#include <Windows.h>
#include <tchar.h>
#include <commctrl.h>

#include "resource.h"
#include "MessageProcessor.h"

// Libs
#pragma comment(lib, "comctl32.lib")

#define WM_TASKBAR_CREATE RegisterWindowMessage(_T("TaskbarCreated"))
#define	WM_USER_SHELLICON WM_USER + 1

NOTIFYICONDATA notifyIconData;
HINSTANCE hInst;
HWND hWnd;
MessageProcessor processor;

LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	if (Message == WM_TASKBAR_CREATE) 
	{
		if (!Shell_NotifyIcon(NIM_ADD, &notifyIconData)) {
			MessageBox(NULL, "Systray Icon Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
			DestroyWindow(hWnd);
			return -1;
		}
	}

	switch (Message)
	{
	case WM_DESTROY:
		Shell_NotifyIcon(NIM_DELETE, &notifyIconData);
		PostQuitMessage(0);
		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);	// Destroy Window
		break;
	default:
		return processor.ProcessMessage(hwnd, Message, wParam, lParam);
	}
	return 0;		// Return 0 = Message successfully proccessed
}

bool RegisterWindowClass(HINSTANCE instanceHandle)
{
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = instanceHandle;
	wc.hIcon = LoadIcon(instanceHandle, (LPCTSTR)MAKEINTRESOURCE(IDI_ICON1));
	wc.hCursor = LoadCursor(instanceHandle, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "Ftor app";
	wc.hIconSm = LoadIcon(instanceHandle, (LPCTSTR)MAKEINTRESOURCE(IDI_ICON1));

	auto result = RegisterClassEx(&wc);
	if (!result) {
		MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return false;
	}

	return true;
}

bool DisplayTrayIcon(HWND hWnd)
{
	NOTIFYICONDATA notifyIconData;
	notifyIconData.cbSize = sizeof(NOTIFYICONDATA);
	notifyIconData.hWnd = (HWND)hWnd;
	notifyIconData.uID = 5;
	notifyIconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	strcpy_s(notifyIconData.szTip, 21, "Ftor Tip");
	notifyIconData.hIcon = NULL;
	notifyIconData.uCallbackMessage = WM_USER_SHELLICON;

	// Display tray icon
	if (!Shell_NotifyIcon(NIM_ADD, &notifyIconData)) {
		MessageBox(NULL, "Systray Icon Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return false;
	}

	return true;
}

int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow)
 {
	HANDLE hMutexInstance;
	hMutexInstance = CreateMutex(NULL, FALSE, _T("GifTray-{96387D4C-6CA0-4E7A-AE72-23B44B338779}"));

	if (GetLastError() == ERROR_ALREADY_EXISTS || GetLastError() == ERROR_ACCESS_DENIED)
		return 0;
	
	if (RegisterWindowClass(hInstance))
	{
		hWnd = CreateWindowEx(WS_EX_CLIENTEDGE, "Ftor app", "Ftor ditch", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

		auto result = GetLastError();

		if (hWnd != NULL)
		{
			if (DisplayTrayIcon(hWnd))
			{
				MSG message;
				while (GetMessage(&message, NULL, 0, 0))
				{
					TranslateMessage(&message);
					DispatchMessage(&message);
				}
				return message.wParam;
			}
		}
		MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
	}
	return 0;
}