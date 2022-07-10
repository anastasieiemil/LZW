#include "stdafx.h"
#include "lzw.h"
#define MAX_LOADSTRING 100
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
HWND caseta, caseta1;									//sa introduc adresa fisierul ce va fi comprimat
char address[MAX_PATH] = "";
wchar_t adress[100];
FILE* input = NULL;
FILE* output = NULL;
//se reapeleaza
// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	MSG msg;
	HACCEL hAccelTable;
	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_LZW, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LZW));

	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LZW));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(3);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_LZW);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;
	hInst = hInstance; // Store instance handle in our global variable

	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, 800, 430, NULL, NULL, hInstance, NULL);
	CreateWindow(TEXT("button"), TEXT("Compress"),	//Buton de compresie	
		WS_VISIBLE | WS_CHILD,
		120, 150, 100, 100,
		hWnd, (HMENU)1, NULL, NULL);

	CreateWindow(TEXT("button"), TEXT("Expand"),		//Buton de expansiune
		WS_VISIBLE | WS_CHILD,
		340, 150, 100, 100,
		hWnd, (HMENU)2, NULL, NULL);

	caseta = CreateWindow(TEXT("edit"), TEXT(""),		//Path catre fisierul sursa
		WS_BORDER | WS_VISIBLE | WS_CHILD,
		60, 45, 500, 30, hWnd, NULL, NULL, NULL);
	CreateWindow(TEXT("button"), TEXT("Browse"),			//Buton ce verifica Pathul
		WS_VISIBLE | WS_CHILD,
		580, 40, 80, 40,
		hWnd, (HMENU)5, NULL, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

bool eroare(HWND hWnd, char* adresa)
{
	if (strstr(adresa, ".eb") == NULL) {
		::MessageBox(hWnd, TEXT("Nu puteti decomprima un fiseier care nu este comprimat!"), TEXT("Eroare"), MB_OK | MB_ICONERROR);
		return false;
	}
	return true;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	switch (message)
	{
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		switch (wmId)
		{
		case IDM_ABOUT:
			::MessageBox(hWnd, _T("@Powerd by Anastasei Emil & Beblea Marian."), _T("About"), MB_OK);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case ID_COMPRESS:
			if (input) {
				SwitchAddress(address);
				Compress(input, address, hWnd);
				::MessageBox(hWnd, _T("Compression finalized!"), _T("Compression"), MB_OK | MB_ICONINFORMATION);
				input = NULL;
			}
			else
				::MessageBox(hWnd, _T("Choose a file"), _T(""), MB_OK | MB_ICONEXCLAMATION);
			break;
		case ID_EXPAND:
			if (!input) {
				::MessageBox(hWnd, _T("Choose a file"), _T(""), MB_OK | MB_ICONEXCLAMATION);
				break;
			}
			if (eroare(hWnd, address)) {
				Uncompress(input, "Uncompressed.txt");
				::MessageBox(hWnd, _T("Uncompression finalized!"), _T("Uncompress"), MB_OK | MB_ICONINFORMATION);//
			}
			break;
		case ID_OPEN:
			input = InOpen(hWnd, caseta, address);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
