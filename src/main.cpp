#include <windows.h>
#include <windowsx.h>
#include <iostream>
#include <random>

#include "../header/resource.h"
#include "../header/levels.h"

#define SCREEN_WIDTH GetSystemMetrics(SM_CXSCREEN)
#define SCREEN_HEIGHT GetSystemMetrics(SM_CYSCREEN)

#define WINDOW_WIDTH 1100
#define WINDOW_HEIGHT 768

#define PLATFORM_WIDTH 90
#define PLATFORM_HEIGHT 14
#define PLATFORM_SPEED 50

#define BALL_WIDTH 12
#define BALL_HEIGHT 12

#define BALL_SPEED 30
#define BALL_SPIN 0.8f

#define TILE_SPACINGX 8
#define TILE_SPACINGY 10

#define GRID_MARGINLR 40
#define GRID_MARGINTB 90

#define TILE_WIDTH (1080 - (GRID_MARGINLR<<1) - (TILE_SPACINGX*11))/12

#define TILE_HEIGHT 18

#pragma warning (disable: 26495)

HWND hwnd;
bool Running;
RECT client;


#define PI 3.14159

typedef struct PointF {
    float x, y;
};

float fRand(float fMin=0.f, float fMax=1.f)
{
    float f = (float)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

struct Entity {	
    COLORREF color;

    RECT src = {0, 0, 0, 0};
    POINT pos = {0, 0};
    virtual void draw() = 0;
    virtual void update() {};
};

void DrawRect(HDC hdc, RECT src, COLORREF color) {
    HBRUSH brush = CreateSolidBrush(color);

    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
    
    FillRect(hdc, &src, brush);
    ReleaseDC(NULL, hdc);
    SelectObject(hdc, oldBrush);
    DeleteObject(brush);
}
 
struct Platform : public Entity {
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
    void update() override {	    
	GetClientRect(hwnd, &client);	    
	if (GetAsyncKeyState(VK_LEFT) & 0x8000 || GetAsyncKeyState(0x41) & 0x8000) {
	    if (this->pos.x <= client.left) {
		OffsetRect(&this->src, 0, 0);
		InvalidateRect(hwnd, NULL, TRUE);
	    } else {
		this->pos.x -= PLATFORM_SPEED;
		OffsetRect(&this->src, -PLATFORM_SPEED, 0);
		InvalidateRect(hwnd, NULL, TRUE);
	    }
	} else if (GetAsyncKeyState(VK_RIGHT) & 0x8000 || GetAsyncKeyState(0x44) & 0x8000) {
	    if (this->pos.x >= client.right-PLATFORM_WIDTH) {
		OffsetRect(&this->src, 0, 0);
		InvalidateRect(hwnd, NULL, TRUE);		    
	    } else {
		this->pos.x += PLATFORM_SPEED;
		OffsetRect(&this->src, PLATFORM_SPEED, 0);
		InvalidateRect(hwnd, NULL, TRUE);		
	    }
	}
	Sleep(60);
    }
};

Platform platform (0, 0, RGB(170, 0, 170));
struct Tile : public Entity {
    void draw() override {
	HDC hdc = GetDC(hwnd);    
	this->src.left = this->pos.x;
	this->src.top = this->pos.y;
	this->src.right = this->pos.x + TILE_WIDTH - 1;
	this->src.bottom = this->pos.y + TILE_HEIGHT - 1;
	DrawRect(hdc, this->src, this->color);   
    }
};
COLORREF colors[5] = { RGB(170, 85, 0), RGB(170, 0, 0), RGB(255, 85, 85), RGB(0, 170, 0), RGB(255, 255, 85) };

struct Grid {
    Tile*** grid;
    Grid() {
	grid = new Tile**[10];
	for (int i = 0; i < 10; i++) {
	    grid[i] = new Tile*[12];
	    for (int j = 0; j < 12; j++) {
		grid[i][j] = new Tile;
	    }
	}
	
	int i, j;
	COLORREF currentColor;
	int iter = 0;
	  
	for (i = 0; i < 10; i++) {
	    if(!(i&1)) {
		currentColor = colors[iter];
		++iter;
	    }
	    for (j = 0; j < 12; j++) {
		grid[i][j]->pos.x = GRID_MARGINLR+(TILE_WIDTH+TILE_SPACINGX)*j; 
		grid[i][j]->pos.y = GRID_MARGINTB+(TILE_HEIGHT+TILE_SPACINGY)*i;     
		grid[i][j]->color = currentColor;
	    }	    
	} 	
    }
    void reset() {
	
    }
    void draw() {
	for (int i = 0; i < 10; i++) {
	    for (int j = 0; j < 12; j++) {		
		if(grid[i][j]) {
		    grid[i][j]->draw();	
		}

	    }
	}
    }
    ~Grid() {
	for (int i = 0; i < 10; i++) {
	    delete[] grid[i];
	}
	delete[] grid;
    }
};
      

Grid _grid;
RECT dst;

struct Ball : public Entity {    
    float dx, dy = 0;	
    PointF pos;
    
    Ball(float x, float y, COLORREF color) {
	this->pos.x = x;
	this->pos.y = y;
	this->color = color;
    }
    void draw() override {
	HDC hdc = GetDC(hwnd);    
	src.left = (int)this->pos.x;
	src.top = (int)this->pos.y;
	src.right = (int)this->pos.x + BALL_WIDTH - 1;
	src.bottom = (int)this->pos.y + BALL_HEIGHT - 1;
	DrawRect(hdc, this->src, this->color);   
    }
    
    void update() override {

	bool collision = IntersectRect(&dst, &this->src, &platform.src);

	if (collision) {
	    float angle = (float)(atan2(this->dy, this->dx));
	    angle+=(fRand()*PI/2-PI/4)*BALL_SPIN;
	    this->dx = BALL_SPEED*cos(angle);
	    this->dy = -BALL_SPEED*sin(angle);	    
	}
	
  	for (int i = 0; i < 10; i++) {
	    for (int j = 0; j < 12; j++) {
		if(_grid.grid[i][j]) {
		    collision = IntersectRect(&dst, &this->src, &_grid.grid[i][j]->src);
		    if (collision) {			    
			this->dy*=-1;
			_grid.grid[i][j] = NULL;		    
			break;
		    }
		}
	    }	    
	}	
	this->pos.x += this->dx;
	this->pos.y += this->dy;
	OffsetRect(&this->src, this->pos.x*this->dx, this->pos.y*this->dy);
	InvalidateRect(hwnd, NULL, TRUE);
    } 
};
Ball ball (0, 0, RGB(255, 255, 255));

void init(void) {
    srand(time(0));
    ShowCursor(FALSE);
    Running = true; 
    GetClientRect(hwnd, &client);    
    platform.pos.x = (client.right-client.left)/2 - PLATFORM_WIDTH/2;
    platform.pos.y = (client.bottom-client.top)/2 - PLATFORM_HEIGHT/2 + 340;     
    ball.pos.x = (client.right-client.left)/2 - BALL_WIDTH/2;
    ball.pos.y = (client.bottom-client.top)/2 - BALL_HEIGHT/2 + 320;    
    ball.dx = (float)BALL_SPEED;
    ball.dy = (float)BALL_SPEED;    
}
void draw() {
    _grid.draw();
    platform.draw();
    ball.draw();
}
void check_walls() {
    if (ball.pos.x <= 0 || ball.pos.x >= client.right-BALL_WIDTH) {
	ball.dx *= -1;
    }
    if (ball.pos.y <= 0) {
	ball.dy*=-1;
    }
    if (ball.pos.y >= client.bottom - BALL_HEIGHT) {
	Running = false;
	return;
    }
}

void update() {
    platform.update();
    check_walls();
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
            draw();
			update();
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
