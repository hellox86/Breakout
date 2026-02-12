#include <windows.h>
#include "../header/resource.h"

#define SCREEN_WIDTH GetSystemMetrics(SM_CXSCREEN)
#define SCREEN_HEIGHT GetSystemMetrics(SM_CYSCREEN)
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
    const wchar_t CLASS_NAME[] = L"Breakout game"; // wchar_t нужен для UTF-16 в WinAPI

    WNDCLASSEX wc = { sizeof(WNDCLASSEX) };

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wc.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClassEx(&wc);
   
    HWND hwnd = CreateWindowEx(
        0,                    
        CLASS_NAME,
        L"Breakout game",    
        WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX,
        (SCREEN_WIDTH - WINDOW_WIDTH) / 2, (SCREEN_HEIGHT - WINDOW_HEIGHT) / 2, WINDOW_WIDTH, WINDOW_HEIGHT,
        NULL,           
        NULL,       
        hInstance,  
        NULL        
    );

    if (!hwnd) return 0;
   
    ShowWindow(hwnd, nCmdShow);

    // Run the message loop.
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        RECT textArea;
        LPCTSTR text = L"Hello, Breakout";
        const int len = lstrlen(text);
        GetClientRect(hwnd, &textArea);
        DrawText(hdc, text, len, &textArea, DT_CENTER | DT_VCENTER);
        EndPaint(hwnd, &ps);
    }
    return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

