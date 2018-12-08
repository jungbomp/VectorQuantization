// MyCompression.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "MyCompression.h"

#define PI 3.14159265358979323846

#define MAX_VAL 65536

#define IMAGE_WIDTH 352
#define IMAGE_HEIGHT 288

#define MAX_LOADSTRING 100

// Global Variables:
MyImage			inImage, outImage;				// image objects
HINSTANCE		hInst;							// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// The title bar text

												// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
void 				ColorPixel(char* imgBuf, int w, int h, int x, int y);

// Main entry point for a windows application
int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow)
{
	MSG msg;
	HACCEL hAccelTable;

	// Read in a parameter from the command line
	char szImageName[1024] = { '\0', };
	int numOfVector = 0;
	int mode = 0;
	sscanf(lpCmdLine, "%s %d %d", szImageName, &numOfVector, &mode);

	// Create a separate console window to display output to stdout
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	printf("The first parameter was: %s\nThe second parameter was: %d\nThe third parameter was: %d\n", szImageName, numOfVector, mode);

	int szLength = strlen(szImageName);
	if (szLength < 5) {
		fprintf(stderr, "Please input image path.\n");
		return 0;
	}

	if (!(0 < mode && mode < 4)) {
		fprintf(stderr, "%d is invalid mode.\n");
		return 0;
	}

	if ((numOfVector - 1) & numOfVector) {
		fprintf(stderr, "Second parameter should be a power of 2.\n");
		return 0;
	}

	// Set up the images
	int w = IMAGE_WIDTH;
	int h = IMAGE_HEIGHT;
	int d = -1;
	if (!strcmp("raw", &szImageName[szLength - 3])) d = 8;
	if (!strcmp("rgb", &szImageName[szLength - 3])) d = 24;

	inImage.setWidth(w);
	inImage.setHeight(h);
	inImage.setDepth(d);

	if ('.' != szImageName[szLength - 4] || inImage.getDepth() < 0) {
		fprintf(stderr, "Invalied file format\n");
		return 0;
	}

	inImage.setImagePath(szImageName);
	inImage.ReadImage();

	int lengthOfVectors[3] = { 2, 4, 16 };
	int lengthOfVector = lengthOfVectors[mode - 1];
	byte* pData = inImage.getImageData();

	if (8 == d || numOfVector < MAX_VAL) {
		VectorClusteringCompressor vcc;
		if (!vcc.Encode(pData, w, h, d, numOfVector, lengthOfVector)) {

		}

		byte* pDest = new byte[inImage.getWidth() * inImage.getHeight() * (d / 8)];
		vcc.Decode(pDest);

		outImage = inImage;
		outImage.setImageData(pDest);
	}
	else {
		outImage = inImage;

		byte* pBlue = new byte[inImage.getWidth() * inImage.getHeight()];
		byte* pGreen = new byte[inImage.getWidth() * inImage.getHeight()];
		byte* pRed = new byte[inImage.getWidth() * inImage.getHeight()];
		byte* bgr[3] = { pBlue, pGreen, pRed };
		byte* pDest = new byte[inImage.getWidth() * inImage.getHeight() * 3];

		for (int i = 0, index = 0; i < inImage.getWidth() * inImage.getHeight() * 3; i += 3) {
			pBlue[index] = pData[i];
			pGreen[index] = pData[i + 1];
			pRed[index] = pData[i + 2];
			index++;
		}

		int numOfClusters[3] = { numOfVector, numOfVector, numOfVector };
		int exp = 0;
		int numOfCluster = numOfVector;
		while (1 < numOfCluster) {
			exp++;
			numOfCluster >>= 1;
		}

		numOfClusters[2] = exp / 3;
		numOfClusters[1] = numOfClusters[2] + (exp % 3 ? 1 : 0);
		numOfClusters[0] = exp - (numOfClusters[1] + numOfClusters[2]);

		numOfClusters[0] = (1 << numOfClusters[0]);
		numOfClusters[1] = (1 << numOfClusters[1]);
		numOfClusters[2] = (1 << numOfClusters[2]);


		VectorClusteringCompressor vcc[3];

		for (int i = 0; i < 3; i++) {
			if (!vcc[i].Encode(bgr[i], w, h, 8, numOfClusters[i], lengthOfVector)) {

			}

			vcc[i].Decode(bgr[i]);
		}

		for (int i = 0, index = 0; i <inImage.getWidth() * inImage.getHeight() * 3; i += 3) {
			pDest[i] = pBlue[index];
			pDest[i + 1] = pGreen[index];
			pDest[i + 2] = pRed[index];

			index++;
		}


		outImage.setImageData(pDest);
	}


    // Initialize global strings
    LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadString(hInstance, IDC_MYCOMPRESSION, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_MYCOMPRESSION);

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

// Colors a pixel at the given (x, y) position black.
void ColorPixel(char* imgBuf, int w, int h, int x, int y)
{
	imgBuf[(4 * y * w) + 4 * x] = 0;
	imgBuf[(4 * y * w) + 4 * x + 1] = 0;
	imgBuf[(4 * y * w) + 4 * x + 2] = 0;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MYCOMPRESSION));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCE(IDC_MYCOMPRESSION);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR szHello[MAX_LOADSTRING];
	LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);
	RECT rt;
	GetClientRect(hWnd, &rt);

    switch (message)
    {
		case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
				case IDM_ABOUT:
					DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
					break;
				case ID_MODIFY_IMAGE:
					outImage.Modify();
					InvalidateRect(hWnd, &rt, false);
					break;
				case IDM_EXIT:
					DestroyWindow(hWnd);
					break;
				default:
					return DefWindowProc(hWnd, message, wParam, lParam);
            }
        } break;

		case WM_PAINT:
		{
			hdc = BeginPaint(hWnd, &ps);
			// TO DO: Add any drawing code here...
			char text[1000];
			strcpy(text, "Original image (Left)  Image after modification (Right)\n");
			DrawText(hdc, text, strlen(text), &rt, DT_LEFT);
			strcpy(text, "\nUpdate program with your code to modify input image");
			DrawText(hdc, text, strlen(text), &rt, DT_LEFT);

			BITMAPINFO bmi;
			// CBitmap bitmap;
			memset(&bmi, 0, sizeof(bmi));
			bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
			bmi.bmiHeader.biWidth = inImage.getWidth();
			bmi.bmiHeader.biHeight = -inImage.getHeight();  // Use negative height.  DIB is top-down.
			bmi.bmiHeader.biPlanes = 1;
			bmi.bmiHeader.biBitCount = inImage.getDepth();
			bmi.bmiHeader.biCompression = BI_RGB;
			bmi.bmiHeader.biSizeImage = inImage.getWidth()*inImage.getHeight();

			if (8 == inImage.getDepth()) {
				for (int i = 0; i < 256; i++) {
					bmi.bmiColors[i].rgbBlue = i;
					bmi.bmiColors[i].rgbGreen = i;
					bmi.bmiColors[i].rgbRed = i;
					bmi.bmiColors[i].rgbReserved = 0;
				}
			}

			SetDIBitsToDevice(hdc,
				100, 100, inImage.getWidth(), inImage.getHeight(),
				0, 0, 0, inImage.getHeight(),
				inImage.getImageData(), &bmi, DIB_RGB_COLORS);

			SetDIBitsToDevice(hdc,
				inImage.getWidth() + 150, 100, outImage.getWidth(), outImage.getHeight(),
				0, 0, 0, outImage.getHeight(),
				outImage.getImageData(), &bmi, DIB_RGB_COLORS);

			EndPaint(hWnd, &ps);
		} break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}
