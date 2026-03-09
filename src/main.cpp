#include <windows.h>
#include <windowsx.h>
#include <iostream>
// 767
#include "../header/resource.h"
#define SCREEN_WIDTH GetSystemMetrics(SM_CXSCREEN)
#define SCREEN_HEIGHT GetSystemMetrics(SM_CYSCREEN)

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 600

#define PLATFORM_WIDTH 100
#define PLATFORM_HEIGHT 4

static HWND hwnd;

typedef struct {
    int x, y;    
} Point;

class Entity {
    public:	
	Point pos = {0, 0};
	virtual void draw(HDC hdc) = 0;
};
    
void DrawRect(HDC hdc, RECT src, COLORREF color) {
    HBRUSH brush = CreateSolidBrush(color);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);

    FillRect(hdc, &src, brush);

    SelectObject(hdc, oldBrush);
    DeleteObject(brush);
}
POINT mouseStartMove;

class Platform : public Entity {
    public:
	RECT pl;
	Platform(int x, int y) {
            this->pos.x = x;
	    this->pos.y = y;
	}
	void draw(HDC hdc) override {
	    this->pl.left = this->pos.x;
	    this->pl.right = this->pos.x+PLATFORM_WIDTH-1;
	    this->pl.top = this->pos.y;
	    this->pl.bottom = this->pos.y+PLATFORM_HEIGHT-1;
	     
	    DrawRect(hdc, this->pl, RGB(170, 170, 170));
	}	
};
class Ball : public Entity {
    public:
	void draw(HDC hdc) override {

	}
};
   
Platform platform(0, 0);

void initGame(void) {
    RECT client;
    GetClientRect(hwnd, &client);
    mouseStartMove.x = platform.pos.x = (client.right-client.left)/2 - PLATFORM_WIDTH/2;
    mouseStartMove.y = platform.pos.y = (client.bottom-client.top)/2 - PLATFORM_HEIGHT/2 + 200;     
}
BOOL bIsDragging = FALSE;

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
    ShowCursor(FALSE);
    if (hwnd == NULL)
    {
        MessageBoxA(NULL, "Window Creation Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    initGame();
		
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
		platform.draw(hdc);
		char buf[256];
		snprintf(buf, sizeof(buf), "x: %d, y: %d\n", platform.pos.x, platform.pos.y);
		
		OutputDebugStringA(buf);
		  
		EndPaint(hwnd, &ps);   	 
	    }
	    break;
 	case WM_LBUTTONDOWN:
	    {
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);
		POINT pt = { xPos, yPos };

		if (PtInRect(&platform.pl, pt))
		{
		    bIsDragging = TRUE;
		    SetCapture(hwnd);
		    mouseStartMove = pt;
		}
	    }
	    break;
 
	case WM_MOUSEMOVE:
	    {
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);
		int dx = xPos-mouseStartMove.x;
		int dy = yPos-mouseStartMove.y;
		OffsetRect(&platform.pl, dx, mouseStartMove.y);

		platform.pos.x = xPos;
 
		mouseStartMove.x = xPos;
		mouseStartMove.y = yPos;

		InvalidateRect(hwnd, NULL, TRUE);
		
	    }
	    break;
	case WM_LBUTTONUP:
	    {
		if (bIsDragging)
		{
		    bIsDragging = FALSE;
		    ReleaseCapture();
		}
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
