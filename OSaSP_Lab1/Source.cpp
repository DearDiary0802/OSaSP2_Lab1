#ifndef UNICODE
#define UNICODE
#endif // UNICODE

#include <Windows.h>

#pragma comment(lib, "Msimg32.lib")

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void DrawPicture(HANDLE& hBmp, BITMAP& bitmap, POINT& bitmapCoordinates);

const int BITMAP_SIZE = 200;
HDC hdcBack;
HANDLE hbmBack;
RECT rcClient;

void InitializeBackBuffer(HWND hWnd, int width, int height)
{
	HDC hdcWindow;
	hdcWindow = GetDC(hWnd);
	hdcBack = CreateCompatibleDC(hdcWindow);
	hbmBack = CreateCompatibleBitmap(hdcWindow, width, height);
	ReleaseDC(hWnd, hdcWindow);

	SaveDC(hdcBack);
	SelectObject(hdcBack, hbmBack);
}

void FinalizeBackBuffer()
{
	if (hdcBack)
	{
		RestoreDC(hdcBack, -1);
		DeleteObject(hbmBack);
		DeleteDC(hdcBack);
		hdcBack = 0;
	}
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
{
	WNDCLASSEX wcex;
	HWND hWnd;
	MSG msg;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_VREDRAW | CS_HREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursorW(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"FirstLab";
	wcex.hIconSm = wcex.hIcon;

	if (!RegisterClassEx(&wcex))
	{
		return 0;
	}

	hWnd = CreateWindowEx(0, L"FirstLab", L"First lab", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;

	static HDC hDC;
	static BITMAP bitmap;
	static HANDLE hBmp;

	static POINT bitmapCoordinates;
	static bool isMouseActive;
	static POINT mouseCoordOnBmp;
	static int offset;

	static UINT_PTR timer;

	RECT clientRect;
	GetClientRect(hWnd, &clientRect);

	switch (uMsg)
	{
	case WM_CREATE:
	{
		hBmp = LoadImage(NULL, L"C:\\Users\\User\\Desktop\\ÎÑèÑÏ_÷2\\OSaSP_Lab1\\cat1.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		GetObject(hBmp, sizeof(bitmap), &bitmap);

		bitmapCoordinates = { 0, 0 };
		isMouseActive = false;
		offset = 5;
		break;
	}
	case WM_DESTROY:
	{
		FinalizeBackBuffer();
		PostQuitMessage(0);
		break;
	}
	case WM_PAINT:
	{
		DrawPicture(hBmp, bitmap, bitmapCoordinates);

		hDC = BeginPaint(hWnd, &ps);
		BitBlt(hDC, 0, 0, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, hdcBack, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_SIZE:
	{
		GetClientRect(hWnd, &rcClient);
		FinalizeBackBuffer();
		InitializeBackBuffer(hWnd, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	}
	case WM_KEYDOWN:
	{
		bool isMoveKey = false;
		switch (wParam)
		{
		case VK_LEFT:
		case 0x41:
			bitmapCoordinates.x - offset < clientRect.left ? bitmapCoordinates.x += offset : bitmapCoordinates.x -= offset;
			isMoveKey = true;
			break;
		case VK_RIGHT:
		case 0x44:
			bitmapCoordinates.x + offset > clientRect.right - BITMAP_SIZE ? bitmapCoordinates.x -= offset : bitmapCoordinates.x += offset;
			isMoveKey = true;
			break;
		case VK_UP:
		case 0x57:
			bitmapCoordinates.y - offset < clientRect.top ? bitmapCoordinates.y += offset : bitmapCoordinates.y -= offset;
			isMoveKey = true;
			break;
		case VK_DOWN:
		case 0x53:
			bitmapCoordinates.y + offset > clientRect.bottom - BITMAP_SIZE ? bitmapCoordinates.y -= offset : bitmapCoordinates.y += offset;
			isMoveKey = true;
			break;
		}
		if (isMoveKey && !timer)
			timer = SetTimer(hWnd, 0, 1 * 1000, NULL);
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	}
	case WM_TIMER:
	{
		offset += 5;
		break;
	}
	case WM_KEYUP:
	{
		switch (wParam)
		{
		case VK_LEFT:
		case 0x41:
		case VK_RIGHT:
		case 0x44:
		case VK_UP:
		case VK_DOWN:
		case 0x57:
		case 0x53:
			KillTimer(hWnd, 0);
			timer = NULL;
			offset = 5;
		}
		break;
	}
	case WM_MOUSEHWHEEL:
	{
		if (isMouseActive)
			break;
		int direction = GET_WHEEL_DELTA_WPARAM(wParam);
		(bitmapCoordinates.x - (direction / 25) >= clientRect.left) && (bitmapCoordinates.x - (direction / 25) <= clientRect.right - BITMAP_SIZE) ? bitmapCoordinates.x -= (direction / 25) : bitmapCoordinates.x += (direction / 25);
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	}
	case WM_MOUSEWHEEL:
	{
		if (isMouseActive)
			break;
		int direction = GET_WHEEL_DELTA_WPARAM(wParam);
		if (LOWORD(wParam) & MK_SHIFT)
		{
			(bitmapCoordinates.x - (direction / 25) >= clientRect.left) && (bitmapCoordinates.x - (direction / 25) <= clientRect.right - BITMAP_SIZE) ? bitmapCoordinates.x -= (direction / 25) : bitmapCoordinates.x += (direction / 25);
		}
		else
		{
			(bitmapCoordinates.y + (direction / 25) >= clientRect.top) && (bitmapCoordinates.y + (direction / 25) <= clientRect.bottom - BITMAP_SIZE) ? bitmapCoordinates.y += (direction / 25) : bitmapCoordinates.y -= (direction / 25);
		}
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	}
	case WM_LBUTTONDOWN:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		mouseCoordOnBmp.x = x - bitmapCoordinates.x;
		mouseCoordOnBmp.y = y - bitmapCoordinates.y;
		if ((mouseCoordOnBmp.x >= 0) && (mouseCoordOnBmp.x <= BITMAP_SIZE) && (mouseCoordOnBmp.y >= 0) && (mouseCoordOnBmp.y <= BITMAP_SIZE))
			isMouseActive = true;
		break;
	}
	case WM_MOUSEMOVE:
	{
		if (isMouseActive)
		{
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);
			if ((x - mouseCoordOnBmp.x >= clientRect.left) && (x - mouseCoordOnBmp.x <= clientRect.right - BITMAP_SIZE))
				bitmapCoordinates.x = x - mouseCoordOnBmp.x;
			else
				if (x - mouseCoordOnBmp.x < clientRect.left)
					bitmapCoordinates.x = 0;
				else
					bitmapCoordinates.x = clientRect.right - BITMAP_SIZE;

			if ((y - mouseCoordOnBmp.y >= clientRect.top) && (y - mouseCoordOnBmp.y <= clientRect.bottom - BITMAP_SIZE))
				bitmapCoordinates.y = y - mouseCoordOnBmp.y;
			else
				if (y - mouseCoordOnBmp.y < clientRect.top)
					bitmapCoordinates.y = 0;
				else
					bitmapCoordinates.y = clientRect.bottom - BITMAP_SIZE;
			InvalidateRect(hWnd, NULL, FALSE);
		}
		break;
	}
	case WM_LBUTTONUP:
	{
		isMouseActive = false;
		break;
	}
	case WM_GETMINMAXINFO:
	{
		MINMAXINFO* pInfo = (MINMAXINFO*)lParam;
		POINT min = { bitmapCoordinates.x + BITMAP_SIZE + 50, bitmapCoordinates.y + BITMAP_SIZE + 50 };
		pInfo->ptMinTrackSize = min;
		break;
	}
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void DrawPicture(HANDLE& hBmp, BITMAP& bitmap, POINT& bitmapCoordinates)
{
	SaveDC(hdcBack);
	FillRect(hdcBack, &rcClient, (HBRUSH)(COLOR_WINDOW + 1));
	
	HDC hMemDC = CreateCompatibleDC(hdcBack);
	HBITMAP hOldBmp = (HBITMAP)SelectObject(hMemDC, hBmp);
	if (hOldBmp)
	{
		SetMapMode(hMemDC, GetMapMode(hdcBack));
		TransparentBlt(hdcBack, bitmapCoordinates.x, bitmapCoordinates.y, BITMAP_SIZE, BITMAP_SIZE, hMemDC, 0, 0, bitmap.bmWidth, bitmap.bmHeight, RGB(0, 255, 0));
		SelectObject(hMemDC, hOldBmp);
	}
	DeleteDC(hMemDC);

	RestoreDC(hdcBack, -1);
}
