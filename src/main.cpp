#include <windows.h>
#include <iostream>


#include "../header/resource.h"
#define SCREEN_WIDTH GetSystemMetrics(SM_CXSCREEN)
#define SCREEN_HEIGHT GetSystemMetrics(SM_CYSCREEN)
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define START_PLATFORM_X WINDOW_WIDTH/2
#define START_PLATFORM_Y WINDOW_HEIGHT/2;

#define INFO(s) { MessageBoxA(NULL, s, "INFO", 0); }
void init() {
    // platform
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
            MoveToEx(hdc, WINDOW_WIDTH / 2-100, WINDOW_HEIGHT / 2 + 100, NULL);
            POINT pt;
            
            bool currentPos = GetCurrentPositionEx(hdc, &pt);
            int platform_size = 200;
            for (int layer = 0; layer < 10; layer++) {
                for (int i = 0; i < platform_size; i++) {
                    SetPixel(hdc, pt.x + i, pt.y, RGB(128, 128, 128));
                }
               pt.y += 1;

            }
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

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{    
    WNDCLASSEX wc;
	
    memset(&wc, 0, sizeof(WNDCLASSEX));
	HWND hwnd;
    MSG Msg;
    
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
    wc.lpszMenuName = NULL;
    wc.lpszClassName = TEXT("breakout game");

    if (!RegisterClassEx(&wc))
    {
        MessageBoxA(NULL, "Window Registration Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    

    hwnd = CreateWindowExA(
        WS_EX_CLIENTEDGE,
        "breakout game",
        "breakout game",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
        (SCREEN_WIDTH-WINDOW_WIDTH)/2, (SCREEN_HEIGHT-WINDOW_HEIGHT)/2, WINDOW_WIDTH, WINDOW_HEIGHT,
        NULL, NULL, hInstance, NULL);

    if (hwnd == NULL)
    {
        MessageBoxA(NULL, "Window Creation Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    while (GetMessage(&Msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return Msg.wParam;