#include <windows.h>
#include <windowsx.h>
#include <iostream>
#include "../header/resource.h"
#define SCREEN_WIDTH GetSystemMetrics(SM_CXSCREEN)
#define SCREEN_HEIGHT GetSystemMetrics(SM_CYSCREEN)

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 600

#define PLATFORM_WIDTH 100
#define PLATFORM_HEIGHT 4

static HWND hwnd;

class Entity {
    public:	
	POINT pos = {0, 0};
	virtual void draw() = 0;
};
    
void DrawRect(HDC hdc, RECT src, COLORREF color) {
    HBRUSH brush = CreateSolidBrush(color);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);

    FillRect(hdc, &src, brush);
    ReleaseDC(NULL, hdc);
    SelectObject(hdc, oldBrush);
    DeleteObject(brush);
}

class Platform : public Entity {
    RECT src = {0, 0, 0, 0};
    COLORREF color;
    public:
	Platform(){};
	Platform(int x, int y, COLORREF color){
	    this->pos.x = x;
	    this->pos.y = y;
	    this->color = color;
	};
	~Platform(){};    
	void draw() override {
	    HDC hdc = GetDC(hwnd);
	    src.left = this->pos.x;
	    src.top = this->pos.y;
	    src.right = this->pos.x + PLATFORM_WIDTH - 1;
	    src.bottom = this->pos.y + PLATFORM_HEIGHT - 1;
	    DrawRect(hdc, this->src, this->color); 
	}
	void move() {
	    
	}
};
class Ball : public Entity {
    public:
	void draw() override {
	    
	}
};
   
Platform platform(0, 0, RGB(37, 150, 190));

void init(void) {
    ShowCursor(FALSE);
    
    RECT client;
    GetClientRect(hwnd, &client);
    platform.pos.x = (client.right-client.left)/2 - PLATFORM_WIDTH/2;
    platform.pos.y = (client.bottom-client.top)/2 - PLATFORM_HEIGHT/2 + 200;     
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wc = { sizeof(WNDCLASSEX) };
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
    wc.lpszMenuName = NULL;
    wc.lpszClassName = L"Breakout";

    if (!RegisterClassEx(&wc))
    {
        MessageBoxA(NULL, "Window Registration Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    hwnd = CreateWindowExA(
        WS_EX_CLIENTEDGE,
        "Breakout",
        "Breakout",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
        (SCREEN_WIDTH - WINDOW_WIDTH) / 2, (SCREEN_HEIGHT - WINDOW_HEIGHT) / 2, WINDOW_WIDTH, WINDOW_HEIGHT,
        NULL, NULL, hInstance, NULL);

    if (hwnd == NULL)
    {
        MessageBoxA(NULL, "Window Creation Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    init();
		
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    
    MSG Msg;	
    while (GetMessage(&Msg, NULL, 0, 0))
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    } 
    
    return Msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
	case WM_PAINT:
	    {
		HDC hdc;
		PAINTSTRUCT  ps;
		hdc = BeginPaint(hwnd, &ps);
		platform.draw();
		EndPaint(hwnd, &ps);   	 
	    }
	    break;
	case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
	case WM_DESTROY:
            PostQuitMessage(0);
            break;
	default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}
