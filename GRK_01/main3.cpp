//////////////////////////////////////////////////////////////////////////
#pragma comment(lib, "gdiplus.lib") // Adding gdiplus library

#include <windows.h> //Header file containing definitions of functions, data types, and WinAPI macros
#include <gdiplus.h> // GDIPlus header file
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>

using namespace Gdiplus;	//We use GDI name space
#define STEP 1.5;
#define PI 3.14

#pragma region Structures

struct MyPixel {
	int mBlackness = 0;
};

MyPixel mFrontBuffer[40][30];


struct Vector2D
{
	float vector[3];
	Vector2D() {
		vector[0] = 0;
		vector[1] = 0;
		vector[2] = 1;
	}
};

struct MyMatrix
{
	float array[3][3];
	void initialize() {
		for (auto i = 0; i < 3; i++)
		{
			for (auto j = 0; j < 3; j++)
			{
				array[i][j] = 0;
			}
		}
	}
	MyMatrix() {	
		initialize();
	}

	void Identity() {
		initialize();
		array[0][0] = 1;
		array[1][1] = 1;
		array[2][2] = 1;
	}

	void Translate(float x, float y) {
		Identity();
		array[0][2] = x;
		array[1][2] = y;

	}

	float DegToRad(float a) {
		return a * M_PI / 180;
	}

	void Rotate(float a) {
		Identity();
		float r = DegToRad(a);
		array[0][0] = cos(r);
		array[0][1] = -(sin(r));
		array[1][0] = sin(r);
		array[1][1] = cos(r);
	}

};


#pragma endregion Structures

#pragma region Matrix_functions
Vector2D Multiply(MyMatrix matrix, Vector2D vector) {
	Vector2D result;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			result.vector[i] += matrix.array[i][j] * vector.vector[j];
		}
	}
	return result;
}

MyMatrix MultiplyMatrix(MyMatrix matrixA, MyMatrix matrixB)
{
	MyMatrix result;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				result.array[i][j] += matrixA.array[i][k] * matrixB.array[k][j];
			}
		}
	}
	return result;
}

VOID DrawTranslatedPixel(int x, int y, MyMatrix transformation) {
	mFrontBuffer[x][y].mBlackness = 100;
	Vector2D vec;
	vec.vector[0] = x;
	vec.vector[1] = y;
	vec.vector[2] = 1;
	auto ret = Multiply(transformation, vec);
	mFrontBuffer[static_cast<int>(ret.vector[0])][static_cast<int>(ret.vector[1])].mBlackness = 255;
}
#pragma endregion Matrix_functions




#pragma region init functions
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
VOID DrawGrid(HDC hdc);
VOID Present(HDC hdc);
VOID DrawPixel(HDC hdc, int x, int y, int blackness);
VOID DrawPixelDiag(HDC hdc, int a, int b, int blackness);
VOID drwln(HDC hdc, int x, int y, int x1, int y1);
VOID brahLine(int x0, int y0, int x1, int y1);
VOID Bresenham(int x0, int y0, int x1, int y1);
VOID FloodFill(int x, int y, int borderColor, int fillColor);
VOID SimpleLine(int x0, int y0, int x1, int y1);
VOID DegToRad(float a);
VOID CircleBresenham(int xc, int yc, int R);
VOID Draw8Points(int xc, int yc, int a, int b);
#pragma endregion init functions

#pragma region init variables
bool left_click = false;
bool right_click = false;
int it = 1;
int x_coodrinate_right_button;
int y_coodrinate_right_button;
int x_coodrinate_left_button = 0;
int y_coordinate_left_button = 0;
#pragma endregion init variables




int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG  msg;
	HWND hWnd;
	WNDCLASS wndClass;


	wndClass.style = CS_HREDRAW | CS_VREDRAW;	//Window style – will be painted when the window is moved or resolution changed   
	wndClass.lpfnWndProc = WndProc;								//Here we indicate the procedure that processes window messages 
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;							//Setting instances of our program in the window 
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);		// Loading from system resources 
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);			// Default cursor and icon
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);	// Color of the background of the window
	wndClass.lpszMenuName = NULL;									// We do not create menu
	wndClass.lpszClassName = TEXT("GettingStarted");				// Class name for the window, displayed in the window header

	RegisterClass(&wndClass);		//Registering window class in the system 


	hWnd = CreateWindow(
		TEXT("GettingStarted"),   // window class name
		TEXT("Getting Started"),  // window caption
		WS_OVERLAPPEDWINDOW,      // window style
		CW_USEDEFAULT,            // initial x position
		CW_USEDEFAULT,            // initial y position
		800,					  // initial x size
		600,					  // initial y size
		NULL,                     // parent window handle
		NULL,                     // window menu handle
		hInstance,                // program instance handle
		NULL);                    // creation parameters



	RECT rect = { 0, 0, 800, 600 }; //Creating rectangle 800x600
	AdjustWindowRect(&rect, GetWindowLong(hWnd, GWL_STYLE), FALSE); //Scaling of the window, to get 800x600px workspace
	SetWindowPos(hWnd, 0, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER | SWP_NOMOVE);

	GdiplusStartupInput gdiplusStartupInput; 	//Structure containing start parameters
	ULONG_PTR           gdiplusToken;	 	// Pointer to assing token

											// GDI initialization
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);


	ShowWindow(hWnd, nCmdShow);	//Displaying window
	UpdateWindow(hWnd);

	/*
	Below: message loop. We get messages
	from the system by means of GetMessage function
	and pass them to the procedure of the window.
	*/
	while (GetMessage(&msg, NULL, 0, 0)) {
		DispatchMessage(&msg);
	}

	return static_cast<int>(msg.wParam);
	GdiplusShutdown(gdiplusToken);
}

VOID OnPaint(HDC hdc)
{
	DrawGrid(hdc);
	
	if (left_click == true) {
		MyMatrix rotation;
		rotation.Rotate(15);
		MyMatrix translation;
		translation.Translate(5, 5);
		MyMatrix transformation = MultiplyMatrix(rotation, translation);
		DrawTranslatedPixel(x_coodrinate_left_button, y_coordinate_left_button, transformation);
		left_click = false;
		
	}
	if (right_click == true) {
		MyMatrix translation;
		translation.Translate(5, 5);
		MyMatrix rotation;
		rotation.Rotate(15);
		auto transformation = MultiplyMatrix(translation, rotation);
		DrawTranslatedPixel(x_coodrinate_left_button, y_coordinate_left_button, transformation);
		right_click = false;		
	}

	Present(hdc);

}

VOID Present(HDC hdc) {
	for (int i = 0; i < 40; i++)
	{
		for (int j = 0; j < 30; j++)
		{
			DrawPixel(hdc, i, j, mFrontBuffer[i][j].mBlackness);
		}
	}
}

VOID CircleBresenham(int xc, int yc, int R)
{
	for (int i = 0; i < 40; i++)
	{
		for (int j = 0; j < 30; j++)
		{
			mFrontBuffer[i][j].mBlackness = 0;
		}
	}
	int x = 0, y = R;
	int d = 1 - R;
	Draw8Points(xc, yc, x, y);
	while (x < y)
	{
		if (d < 0)
			d += 2 * x + 2;
		else
		{
			d += 2 * (x - y) + 5;
			y--;
		}
		x++;
		Draw8Points(xc, yc, x, y);
	}


}

VOID Draw8Points(int xc, int yc, int a, int b)
{
	mFrontBuffer[xc + a][yc + b].mBlackness = 255;
	mFrontBuffer[xc - a][yc + b].mBlackness = 255;
	mFrontBuffer[xc - a][yc - b].mBlackness = 255;
	mFrontBuffer[xc + a][yc - b].mBlackness = 255;
	mFrontBuffer[xc + b][yc + a].mBlackness = 255;
	mFrontBuffer[xc - b][yc + a].mBlackness = 255;
	mFrontBuffer[xc - b][yc - a].mBlackness = 255;
	mFrontBuffer[xc + b][yc - a].mBlackness = 255;
}

VOID SimpleLine(int x0, int y0, int x1, int y1) {
	if (x1 < x0)
	{
		std::swap(x0, x1);
		std::swap(y0, y1);
	}
	if (y1 < y0)
	{
		std::swap(x0, x1);
		std::swap(y0, y1);
	}
	auto slope = static_cast<float>(y1 - y0) / (x1 - x0);
	auto b = y0 - slope*x0;

	for (int i = x0; i < x1; i++)
	{
		int y = slope * i + b;
		mFrontBuffer[i][y].mBlackness = 255;
	}

}

VOID brahLine(int x0, int y0, int x1, int y1) {
	if (x1 < x0)
	{
		std::swap(x0, x1);
		std::swap(y0, y1);
	}
	if (y1 < y0)
	{
		std::swap(x0, x1);
		std::swap(y0, y1);
	}
	float error = 0;
	mFrontBuffer[x0][y0].mBlackness = 255;
	auto slope = static_cast<float>(y1 - y0) / (x1 - x0);

	for (auto i = x0; i < x1; i++)
	{
		error += slope;
		if (error < 0.5)
		{
			mFrontBuffer[i + 1][i].mBlackness = 255;
		}
		else {
			mFrontBuffer[i + 1][i + 1].mBlackness = 255;

		}
		error -= 1;
	}
}

VOID Bresenham(int x0, int y0, int x1, int y1) {
	int delta_x = x1 - x0;
	int delta_y = y1 - y0;
	int delta_a = 2 * delta_y;
	int delta_b = 2 * delta_y - 2 * delta_x;

	int d = 2 * delta_y - delta_x;
	int y = y0;
	mFrontBuffer[x0][y0].mBlackness = 255;
	for (int i = x0 + 1; i < x1; i++)
	{
		if (d < 0)
		{
			mFrontBuffer[i][y].mBlackness = 255;
			d += delta_a;
		}
		else {
			y += 1;
			mFrontBuffer[i][y].mBlackness = 255;
			d += delta_b;
		}
	}
	mFrontBuffer[x1][y1].mBlackness = 255;
}

VOID DrawGrid(HDC hdc)
{
	Graphics graphics(hdc);
	Pen pen(Color(255, 0, 0, 0));

	for (int i = 0; i < 40; i++)
	{
		graphics.DrawLine(&pen, i * 20, 0, i * 20, 600);
	}

	for (int i = 0; i < 30; i++)
	{
		graphics.DrawLine(&pen, 0, i * 20, 800, i * 20);
	}

}

VOID FloodFill(int x, int y, int borderColor, int fillColor) {

	if (!(x < 0 || x > 40) && !(y < 0 || y > 30)) {
		if (mFrontBuffer[x][y].mBlackness != borderColor && mFrontBuffer[x][y].mBlackness != fillColor) {
			mFrontBuffer[x][y].mBlackness = fillColor;
			FloodFill(x, y - 1, borderColor, fillColor);
			FloodFill(x + 1, y, borderColor, fillColor);
			FloodFill(x, y + 1, borderColor, fillColor);
			FloodFill(x - 1, y, borderColor, fillColor);
		}
	}
}

VOID DrawPixel(HDC hdc, int x, int y, int blackness) {
	Graphics graphics(hdc);
	SolidBrush solidBrush(Color(blackness, 0, 0, 0));
	graphics.FillRectangle(&solidBrush, x * 20, y * 20, 20, 20);
}

VOID DrawPixelDiag(HDC hdc, int a, int b, int blackness) {
	Graphics graphics(hdc);
	SolidBrush solidBrush(Color(blackness, 255, 255, 254));
	graphics.FillRectangle(&solidBrush, a * 20, b * 20, 20, 20);
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	HDC          hdc;
	PAINTSTRUCT  ps;
	//int x, y;

	switch (msg)
	{
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		OnPaint(hdc);
		EndPaint(hwnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

		
	case WM_LBUTTONDOWN:
		hdc = BeginPaint(hwnd, &ps);
		x_coodrinate_left_button = LOWORD(lParam) / 20;
		y_coordinate_left_button = HIWORD(lParam) / 20;
		left_click = true;
		RedrawWindow(hwnd, nullptr, nullptr, RDW_ERASE | RDW_INVALIDATE);
		break;
	case WM_RBUTTONDOWN:
		hdc = BeginPaint(hwnd, &ps);
		x_coodrinate_right_button = LOWORD(lParam) / 20;
		y_coodrinate_right_button = HIWORD(lParam) / 20;
		right_click = true;
		RedrawWindow(hwnd, nullptr, nullptr, RDW_ERASE | RDW_INVALIDATE);
		break;
	}


	return DefWindowProc(hwnd, msg, wParam, lParam);
}


