#include <windows.h>
#include "../header/resource.h"
#define SCREEN_WIDTH GetSystemMetrics(SM_CXSCREEN)
#define SCREEN_HEIGHT GetSystemMetrics(SM_CYSCREEN)
#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 600

bool running = false;

typedef struct {
    int x, y;    
} Point;

class Entity {
    public:	
	Point pos;
	
	virtual void draw(HDC hdc) = 0;
};
RECT getCenteredRect(int x, int y, int w, int h) {
    RECT tmp;
    
    tmp.left = x-(w/2);
    tmp.right = x+(w/2);
    tmp.top = y-(h/2);
    tmp.bottom = y+(h/2);
    return tmp;
}
void DrawRect(HDC hdc, int x, int y, int w, int h, COLORREF color) {
    HBRUSH brush = CreateSolidBrush(color);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);

    RECT src = getCenteredRect(x, y, w, h);
    FillRect(hdc, &src, brush);

    SelectObject(hdc, oldBrush);
    DeleteObject(brush);
}

class Platform : public Entity {
    public:
	Platform(int x, int y) {
            this->pos.x = x;
	    this->pos.y = y;
	}
	void draw(HDC hdc) override {
            DrawRect(hdc, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 + 200, 1100, 3, RGB(170, 170, 170));
	}
	void move() { 
	    
	}
};
class Ball : public Entity {
    public:
	void draw(HDC hdc) override {

	}
};

Platform platform(1, 1);

void initGame(HDC hdc) {
    platform.draw(hdc);
    
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wc = { sizeof(WNDCLASSEX) };
    HWND hwnd;
  
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

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    running = true;
    
    while(running) {
 	MSG Msg;
	
 	while (PeekMessage(&Msg, 0, 0, 0, PM_REMOVE))
	{
	    if(Msg.message == WM_QUIT) {
		running = false;
	    }
        TranslateMessage(&Msg);
        DispatchMessageA(&Msg);
	} 
    }
    return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_PAINT:
    {
        HDC          hdc;
        PAINTSTRUCT  ps;
        hdc = BeginPaint(hwnd, &ps);
        initGame(hdc);
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
