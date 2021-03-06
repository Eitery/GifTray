// window.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "window.h"

#include <objidl.h>
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")
#include <Gdipluscolor.h>

#define MAX_LOADSTRING 100

HINSTANCE hInst;
WCHAR szWindowClass[MAX_LOADSTRING]; 

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	LoadStringW(hInstance, IDC_WINDOW, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOW));

	MSG msg;

	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOW));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = NULL;
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance;
	auto width = GetSystemMetrics(SM_CXSCREEN);
	auto height = GetSystemMetrics(SM_CYSCREEN);

	HWND hWnd = CreateWindowEx(WS_EX_LAYERED, szWindowClass, 0, WS_DLGFRAME, 0, 0, width, height, NULL, NULL, hInstance, NULL);

	SetTimer(hWnd, 100500, 25, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static bool mouseState = false;
	static POINT startPoint;
	switch (message)
	{
	case WM_TIMER:
	{
		if (mouseState)
		{
			POINT point;
			if (GetCursorPos(&point))
			{
				auto width = abs(startPoint.x - point.x);
				auto height = abs(startPoint.y - point.y);

				auto x = min(startPoint.x, point.x);
				auto y = min(startPoint.y, point.y);

				Bitmap b(width, height);
				Gdiplus::Graphics *g = Gdiplus::Graphics::FromImage(&b);
				Gdiplus::Pen pen(Color(0, 0, 0, 0), 1);
				Gdiplus::Rect rect(x, y, width, height);
				g->DrawRectangle(&pen, rect);

				HDC screenDC = GetDC(NULL);

				UpdateLayeredWindow(hWnd, NULL, NULL, NULL, NULL, NULL, RGB(255, 255, 255), NULL, ULW_COLORKEY);



				/********************************************
				*   step 1.
				*   Using Gdiplus to load the image
				********************************************/
				RECT wndRect;
				::GetWindowRect(hWnd, &wndRect);
				SIZE wndSize = { wndRect.right - wndRect.left,wndRect.bottom - wndRect.top };
				HDC hdc = ::GetDC(hWnd);
				HDC memDC = ::CreateCompatibleDC(hdc);
				HBITMAP memBitmap = ::CreateCompatibleBitmap(hdc, wndSize.cx, wndSize.cy);
				::SelectObject(memDC, b.GetHBITMAP();

				BrandImageIStream.Seek({ 0,0 }, SEEK_SET, nullptr);
				Gdiplus::Image image(&BrandImageIStream);
				Gdiplus::Graphics graphics(memDC);
				graphics.DrawImage(&image, 0, 0, wndSize.cx, wndSize.cy);
				/********************************************
				*   step 2.
				*   Get "UpdateLayeredWindow" function's
				*   proc address.
				********************************************/
				HMODULE hUser32 = ::LoadLibrary("User32.dll");
				if (!hUser32)
				{
					return FALSE;
				}
				UPDATELAYEREDWINDOWFUNCTION UpdateLayeredWindow = (UPDATELAYEREDWINDOWFUNCTION)::GetProcAddress(hUser32, "UpdateLayeredWindow");
				if (!UpdateLayeredWindow)
				{
					return FALSE;
				}
				// get screen dc 
				HDC screenDC = GetDC(NULL);
				POINT ptSrc = { 0,0 };

				/*********************************************
				*   step 3.
				*   Use UpdateLayeredWindow to Draw the Window
				*
				*********************************************/
				BLENDFUNCTION blendFunction;
				blendFunction.AlphaFormat = AC_SRC_ALPHA;
				blendFunction.BlendFlags = 0;
				blendFunction.BlendOp = AC_SRC_OVER;
				blendFunction.SourceConstantAlpha = 255;
				UpdateLayeredWindow(hwnd, screenDC, &ptSrc, &wndSize, memDC, &ptSrc, 0, &blendFunction, 2);

				::DeleteDC(memDC);
				::DeleteObject(memBitmap);
			}
		}
		break;
	}
	case WM_PAINT:
		break;
	case WM_LBUTTONDOWN:
		if (GetCursorPos(&startPoint))
		{
			mouseState = true;
		}
		break;
	case WM_LBUTTONUP:
		mouseState = false;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
