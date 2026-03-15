#include <windows.h>
#include <windowsx.h>
#include <iostream>
#include "../header/resource.h"

#define SCREEN_WIDTH GetSystemMetrics(SM_CXSCREEN)
#define SCREEN_HEIGHT GetSystemMetrics(SM_CYSCREEN)

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define PLATFORM_WIDTH 80
#define PLATFORM_HEIGHT 13
#define PLATFORM_SPEED 50

#define BALL_WIDTH 10
#define BALL_HEIGHT 10

static HWND hwnd;
bool Running;
RECT client;

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
    COLORREF color;
    public:
	RECT src = {0, 0, 0, 0};
	Platform(){};
	Platform(int x, int y, COLORREF color){
	    this->pos.x = x;
	    this->pos.y = y;
	    this->color = color;
	};
	~Platform(){};    
	virtual void draw() override {
	    HDC hdc = GetDC(hwnd);    
	    src.left = this->pos.x;
	    src.top = this->pos.y;
	    src.right = this->pos.x + PLATFORM_WIDTH - 1;
	    src.bottom = this->pos.y + PLATFORM_HEIGHT - 1;
	    DrawRect(hdc, this->src, this->color);    
	} 
	virtual void update() {	    
	    GetClientRect(hwnd, &client);	    
	    if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
		if (this->pos.x <= client.left) return;
		this->pos.x -= PLATFORM_SPEED;
		OffsetRect(&this->src, -PLATFORM_SPEED, 0);
		InvalidateRect(hwnd, NULL, TRUE);
	    } else if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
		if (this->pos.x >= client.right-PLATFORM_WIDTH) return;	  
		this->pos.x += PLATFORM_SPEED;
		OffsetRect(&this->src, PLATFORM_SPEED, 0);
		InvalidateRect(hwnd, NULL, TRUE);		
	    }
	    Sleep(100);
	}
};
class Ball : public Platform {
    COLORREF color;    
    public:
	int dx, dy = 0;
	
	Ball(){};
	Ball(int x, int y, COLORREF color) {
	    this->pos.x = x;
	    this->pos.y = y;
	    this->color = color;
	}
    	void draw() override {
	    HDC hdc = GetDC(hwnd);    
	    src.left = this->pos.x;
	    src.top = this->pos.y;
	    src.right = this->pos.x + BALL_WIDTH - 1;
	    src.bottom = this->pos.y + BALL_HEIGHT - 1;
	    DrawRect(hdc, this->src, this->color);   
	}
	void update() override {
	    if (this->pos.x <= 0 || this->pos.x >= client.right-BALL_WIDTH) {
		this->dx *= -1;
	    }
	    if (this->pos.y <= 0 || this->pos.y >= client.bottom - BALL_HEIGHT) {
		this->dy*=-1;
	    }
	    this->pos.x += this->dx;
	    this->pos.y += this->dy;
	    OffsetRect(&this->src, this->pos.x*this->dx, this->pos.y*this->dy);
	    InvalidateRect(hwnd, NULL, TRUE);
	}    
};
   
Platform platform(0, 0, RGB(170, 0, 170));
Ball ball(0, 0, RGB(255, 255, 255));

void init(void) {
    ShowCursor(FALSE);
    Running = true; 
    GetClientRect(hwnd, &client);
    platform.pos.x = (client.right-client.left)/2 - PLATFORM_WIDTH/2;
    platform.pos.y = (client.bottom-client.top)/2 - PLATFORM_HEIGHT/2 + 260;     
    ball.pos.x = (client.right-client.left)/2 - BALL_WIDTH/2;
    ball.pos.y = (client.bottom-client.top)/2 - BALL_HEIGHT/2 + 240;    
    ball.dx = 25;
    ball.dy = 25;
}
void draw() {
    platform.draw();
    ball.draw();
}
void update() {
    platform.update();
    ball.update();
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
        while(Running)
        {
	    if(PeekMessage(&Msg, 0, 0, 0, PM_REMOVE))
            {
                if(Msg.message == WM_QUIT) {
                    Running = false;
                }
                TranslateMessage(&Msg);
                DispatchMessageA(&Msg);
            } else {
          	update();
		draw();
	    }
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
		draw(); 	 
		EndPaint(hwnd, &ps);   	 
	    }
	    break;      
	case WM_CLOSE:
	    Running = false;
            DestroyWindow(hwnd);
            break;
	case WM_DESTROY:
	    Running = false;
            PostQuitMessage(0);
            break;
	default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}
