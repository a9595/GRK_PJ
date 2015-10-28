//////////////////////////////////////////////////////////////////////////
#include <windows.h> //Plik naglowkowy zawieraj?cy definicje funkcji, typy danych oraz makra WinAPI
#pragma comment(lib, "gdiplus.lib") // Adding gdiplus library

#include <windows.h> //Header file containing definitions of functions, data types, and WinAPI macros
#include <gdiplus.h> // GDIPlus header file

using namespace Gdiplus;	//We use GDI name space

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


struct MyPixel
{
	MyPixel()
	{
		mBlackness = 0;
	}

	int mBlackness;
};

struct MyStructure
{
	MyStructure()
	{
		mMyNumber = 100;
	}

	int mMyNumber;
};

const int mFrontBufferRowCount = 40, mFrontBufferColCount= 30;
MyPixel mFrontBuffer[mFrontBufferRowCount][mFrontBufferColCount];

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


	GdiplusShutdown(gdiplusToken);

	return (int)msg.wParam;
}

void DrawPixel(HDC hdc, int x, int y, int blackness)
{
	Graphics graphics(hdc);
	SolidBrush solidBrush(Color(blackness, 0, 0, 0));
	graphics.FillRectangle(&solidBrush, x, y, 19, 19);
}

void draw_pink_floyed(HDC hdc)
{
	Graphics graphics(hdc);
	Pen      pen(Color(255, 0, 0, 0)); //Constructor takes color in ARGB format

	for (int i = 0; i <= 40; i = i + 1)
	{
		graphics.DrawLine(&pen, 0, i * 20, 600, 600);
	}
}

void DrawGrid(HDC hdc) {
	
	MyPixel mFrontBuffer[40][30];
	mFrontBuffer[10][10].mBlackness = 255;

	Graphics graphics(hdc);
	Pen      pen(Color(255, 0, 0, 0)); //Constructor takes color in ARGB format
		
	
	for (int i = 0; i<=40; i = i + 1)
	{

		graphics.DrawLine(&pen, i*20, 0, i*20, 600); // draw  ||||||
		graphics.DrawLine(&pen, 0, i * 20, 800, i * 20); // draw  ===
	}

	
}



void draw_line(HDC hdc)
{
	Graphics graphics(hdc);
	Pen pen(Color(255, 0, 0, 0)); //Constructor takes color in ARGB format


	graphics.DrawLine(&pen, 0, 0, 800, 600); //arguments: Pen object, x1, y1, x2, y2


	SolidBrush solidBrush(Color(255, 0, 0, 0));
	graphics.FillRectangle(&solidBrush, 100, 100, 30, 10);


	///draw

	MyStructure structure1;
	structure1.mMyNumber = 30;

	MyStructure structTable[10];
	structTable[4].mMyNumber = 15;
//	int table[3][4];

	for (int i = 0; i < 10; i = i + 1)
	{
		graphics.DrawLine(&pen, i, 0, i, 600);
	}
}

void Present(HDC hdc)
{
	mFrontBuffer[20][20].mBlackness = 255;
	for (int i = 0; i < mFrontBufferRowCount; i++)
	{
		for (int j = 0; j < mFrontBufferColCount; j++)
		{
			DrawPixel(hdc, i*20, j*20, mFrontBuffer[i][j].mBlackness);
		}
	}
}
VOID OnPaint(HDC hdc)
{
	//draw_line(hdc);
	DrawGrid(hdc);

	//DrawPixel(hdc, 1, 1, 255);

	// draw pinkFloyed
	//draw_pink_floyed(hdc);


	Present(hdc);

}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	HDC          hdc;
	PAINTSTRUCT  ps;

	switch (msg)
	{
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		OnPaint(hdc);
		EndPaint(hwnd, &ps);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0); // This function adds messages WM_QUIT to the queue
		return 0;
	}

	// default handling of all other messages 
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

//////////////////////////////////////////////////////////////////////////