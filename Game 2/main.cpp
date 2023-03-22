#include <iostream>
#include "Graphics.h"
#include "scripts.h"

//window params
long screenright = 800;
long screenbottom = 600;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		if ((lParam >> 30) & 0x1) break;
		ic->receive_keydown(wParam);
		break;
	case WM_KEYUP:
		ic->receive_keyup(wParam);
		break;
	default:
		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLn, int nCmdShow) {

	//create windowclass struct
	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX); wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc; wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW); wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszClassName = L"WindowClass";
	RegisterClassEx(&wc);

	//adjust client size, create window
	RECT rect = { 0, 0, screenright, screenbottom };
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
	HWND hWnd = CreateWindowEx(NULL, L"WindowClass", L"Game", WS_OVERLAPPEDWINDOW,
		100, 100, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, hInstance, NULL);

	// initialization and setup
	Graphics::init(hWnd);
	setup();

	//show window
	ShowWindow(hWnd, nCmdShow);

	//main loop
	MSG msg = {};
	msg.message = WM_NULL;
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		Graphics::rtarget->BeginDraw();
		Graphics::rtarget->Clear(D2D1::ColorF(0.3f, 0.3f, 0.3f, 1));
		run();
		Graphics::rtarget->EndDraw();
	}
}