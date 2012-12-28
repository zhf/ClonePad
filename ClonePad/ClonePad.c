// ClonePad.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "ClonePad.h"

#define MAX_LOADSTRING 100
//#define MAX_PATH 1024
#define IID_PPV_ARG(Type, Expr) IID_##Type, \
	reinterpret_cast<void**>(static_cast<Type **>(Expr))

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

HWND hMainWnd;
HWND hEditCtrl;
TCHAR szFileName[MAX_PATH] = TEXT("");
TCHAR szShortFileName[MAX_PATH] = TEXT("");
BOOL bTextIsUnicode;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
void				WhatError();
void				InitNewFile();

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_CLONEPAD, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CLONEPAD));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CLONEPAD));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)COLOR_APPWORKSPACE;//(HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_CLONEPAD);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   hMainWnd = hWnd;
   ShowWindow(hWnd, nCmdShow);
   InitNewFile();
   UpdateWindow(hWnd);


   return TRUE;
}

/* Requires Windows Vista or later */
DWORD OfferSaveDialog(HWND parentWin)
{
	int nButtonPressed                  = 0;
	TASKDIALOGCONFIG config             = {0};
	const TASKDIALOG_BUTTON buttons[]   = { 
		{ IDYES, TEXT("Save") },
		{ IDNO , TEXT("Don't Save")}
	};
	config.cbSize                       = sizeof(config);
	config.hInstance                    = hInst;
	config.dwCommonButtons              = TDCBF_CANCEL_BUTTON;
	config.pszMainIcon                  = NULL;
	config.pszMainInstruction           = TEXT("Do you want to save changes?");
	config.pszContent                   = NULL;
	config.pButtons                     = buttons;
	config.cButtons                     = ARRAYSIZE(buttons);
	config.hMainIcon					= NULL;
	config.dwFlags						= TDF_ALLOW_DIALOG_CANCELLATION | TDF_USE_HICON_MAIN;
	config.hwndParent					= parentWin;

	TaskDialogIndirect(&config, &nButtonPressed, NULL, NULL);
	return nButtonPressed;
}

void InitNewFile()
{
	SetWindowText(hMainWnd, TEXT("Untitled"));
	//WhatError();
	SetWindowText(hEditCtrl, NULL);
	//WhatError();
	_tcscpy_s(szShortFileName, TEXT(""));
	_tcscpy_s(szFileName, TEXT(""));
	bTextIsUnicode = TRUE;
	SendMessage(hEditCtrl, EM_SETMODIFY, (WPARAM)FALSE, 0);
}

//#pragma warning(disable : 4996)
BOOL ExecuteSaveDialog(LPOPENFILENAME lpofn)
{
	/* OPENFILENAME structure	*/
	(*lpofn).lStructSize = sizeof(*lpofn);
	(*lpofn).hwndOwner = hEditCtrl;
	(*lpofn).lpstrFilter = TEXT("Text\0*.txt\0UNICODE\0*.txt\0All\0*.*");
	(*lpofn).nFilterIndex = 1;
	(*lpofn).lpstrFile = szFileName;
	(*lpofn).nMaxFile = sizeof(szFileName);
	if (_tcscmp(szShortFileName, TEXT("")) == 0) _tcscpy_s(szShortFileName, sizeof(TEXT("*.txt")), TEXT("*.txt"));
	(*lpofn).lpstrFileTitle = szShortFileName;
	(*lpofn).nMaxFileTitle = sizeof(szShortFileName);
	(*lpofn).Flags = OFN_OVERWRITEPROMPT;
	(*lpofn).lpstrDefExt = TEXT("txt");
	return GetSaveFileName(lpofn);
}

BOOL ExecuteOpenDialog(LPOPENFILENAME lpofn)
{
	// Fill the structure
	(*lpofn).lStructSize = sizeof(*lpofn);
	(*lpofn).hwndOwner = hEditCtrl;
	(*lpofn).lpstrFilter = TEXT("Text\0*.txt\0All\0*.*\0");
	(*lpofn).nFilterIndex = 1;
	(*lpofn).lpstrFile = szFileName;
	(*lpofn).nMaxFile = sizeof(szFileName);
	(*lpofn).lpstrFileTitle = szShortFileName;
	(*lpofn).nMaxFileTitle = sizeof(szShortFileName);
	(*lpofn).Flags = OFN_CREATEPROMPT | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	return GetOpenFileName(lpofn);
}

HRESULT VistaOpenFileDialog()
{
    IFileDialog *pfd;
    
    // CoCreate the dialog object.
    HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, 
                                  NULL, 
                                  CLSCTX_INPROC_SERVER, 
                                  IID_PPV_ARG(IFileDialog, &pfd));
    
    if (SUCCEEDED(hr))
    {
        // Show the dialog
        hr = pfd->Show(hMainWnd);
        
        if (SUCCEEDED(hr))
        {
            // Obtain the result of the user's interaction with the dialog.
            IShellItem *psiResult;
            hr = pfd->GetResult(&psiResult);
            
            if (SUCCEEDED(hr))
            {
                // Do something with the result.
                psiResult->Release();
            }
        }
        pfd->Release();
    }
    return hr;
}

void SaveFile(BOOL fileIsUnicode)
{
	HANDLE hFile = CreateFile(szFileName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		WhatError();
		return;
	}
	int cTxtLen = GetWindowTextLength(hEditCtrl); 
	DWORD dwSize = fileIsUnicode ? (cTxtLen + 1) * 2 : (cTxtLen + 1);
	PBYTE lpBuffer= (PBYTE)malloc(dwSize);
	GetWindowText(hEditCtrl, (LPTSTR)lpBuffer, dwSize);

	DWORD NumberOfBytesWritten;
	if (fileIsUnicode)
	{
		// Write unicode signature
		WORD bom = 0xfeff;
		WriteFile(hFile, &bom, sizeof(bom), &NumberOfBytesWritten, NULL);
		// Write the content
		WriteFile(hFile, lpBuffer, dwSize, &NumberOfBytesWritten, NULL);
	}
	else
	{
		LPSTR lpAnsiCharStr = (LPSTR)malloc(dwSize); // TODO: Calc the size accurately
		// Convert unicode to ansi
		WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)lpBuffer, cTxtLen, lpAnsiCharStr, dwSize, NULL, NULL);
		// Write the content
		WriteFile(hFile, lpAnsiCharStr, dwSize, &NumberOfBytesWritten, NULL);
		free(lpAnsiCharStr);
	}
	CloseHandle(hFile);
	free(lpBuffer);
}

void LoadFile()
{
	HANDLE hFile = CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_HIDDEN,NULL);

	if(hFile == INVALID_HANDLE_VALUE)
	{
		WhatError();
		return;
	}

	DWORD dwFileSize = GetFileSize(hFile,NULL);

	if(dwFileSize > 1000000)
	{
		MessageBox(hEditCtrl,TEXT("File greater than 1 MB is not supported."),TEXT("Size limit"),0);
		CloseHandle(hFile);
		return;
	}

	PBYTE lpBuffer = (PBYTE)malloc(dwFileSize + 2);
	DWORD NumberOfBytesWritten;
	DWORD bResult = ReadFile(hFile,lpBuffer,dwFileSize,&NumberOfBytesWritten,NULL);
	CloseHandle(hFile);
	if (bResult == FALSE)
	{
		WhatError();
		return;
	}
	
	lpBuffer[dwFileSize] = NULL;
	lpBuffer[dwFileSize+1] = NULL;
	/* Test for type of charater system */
	int unicodeTest = IS_TEXT_UNICODE_SIGNATURE | IS_TEXT_UNICODE_REVERSE_SIGNATURE ;
	bTextIsUnicode = IsTextUnicode(lpBuffer,dwFileSize,&unicodeTest);
	if( bTextIsUnicode )
		{
			if(unicodeTest & IS_TEXT_UNICODE_REVERSE_SIGNATURE)
			{
				for(unsigned int i = 0; i < dwFileSize; i+=2)
				{
					BYTE tempByte = lpBuffer[i];
					lpBuffer[i] = lpBuffer[i+1];
					lpBuffer[i+1] = tempByte;
				}
			}

			SetWindowText(hEditCtrl,(LPTSTR)lpBuffer+1);
	}
	else
	{
		// Convert unicode buffer to a new multibyte buffer
		size_t cb = dwFileSize * 2 + 2;
		LPWSTR lpWideCharStr = (LPWSTR)malloc(cb);
		// TODO: Ask for encoding
		MultiByteToWideChar(/*CP_ACP*/936, MB_PRECOMPOSED, (LPCSTR)lpBuffer, -1, lpWideCharStr, cb);
		SetWindowText(hEditCtrl,lpWideCharStr);
		free(lpWideCharStr);
	}
	free(lpBuffer);
	SetWindowText(hMainWnd, szShortFileName);
}
//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	//PAINTSTRUCT ps;
	//HDC hdc;
	static HWND hEdit;
	static CHOOSEFONT cf;
	static LOGFONT lf; 
	static HFONT font;
	static OPENFILENAME ofn;
	//static TCHAR filename[266];
	static HANDLE hFile; 
	static BOOL fsave,unicode;
	RECT rect; 
	int nButtonHit = -1;
	time_t now;
	TCHAR *szNow;

	switch (message)
	{
	case WM_CREATE:
		hEdit = CreateWindow(TEXT("EDIT"), NULL, 
			WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL,
			0, 0, CW_USEDEFAULT, CW_USEDEFAULT, hWnd, NULL, hInst, 0);
		ShowWindow(hEdit, SW_SHOWMAXIMIZED);
		SetFocus(hEdit);
		hEditCtrl = hEdit;
		return 0;

	case WM_SIZE:
		MoveWindow(hEdit, 0, 0, LOWORD(lParam),HIWORD(lParam),TRUE);
		return 0;

	case WM_SETFOCUS:
		SetFocus(hEdit);
		return 0;

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			//DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			//ShellAbout(hWnd, szTitle, TEXT("<<A Notepad Clone>>"), LoadIcon(hInst, MAKEINTRESOURCE(IDI_CLONEPAD)));
			VistaOpenFileDialog();
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case ID_FILE_NEW:
			OutputDebugString(TEXT("FILE_NEW"));
			if (GetWindowTextLength(hEdit) == 0) break;
			nButtonHit = OfferSaveDialog(hWnd);
			switch (nButtonHit)
			{
			case IDCANCEL:
				break; // Do nothing
			case IDYES:
				// Do save ...
				InitNewFile();
				break;
			case IDNO:
				// Clear the edit;
				InitNewFile();
				break;
			}
			break;
		case ID_FILE_OPEN:
			OutputDebugString(TEXT("FILE_NEW"));
			if (ExecuteOpenDialog(&ofn) != FALSE)
			{
				_tcscpy_s(szFileName, ofn.lpstrFile);
				_tcscpy_s(szShortFileName, ofn.lpstrFileTitle);
			}
			LoadFile();
			break;
		case ID_FILE_SAVEAS:
			if (ExecuteSaveDialog(&ofn) != FALSE)
			{
				_tcscpy_s(szFileName, ofn.lpstrFile);
				_tcscpy_s(szShortFileName, ofn.lpstrFileTitle);
				SaveFile(bTextIsUnicode);
				SetWindowText(hWnd, szShortFileName);
			}
			break;
		case ID_EDIT_UNDO:
			OutputDebugString(TEXT("EDIT_UNDO"));
			if (SendMessage(hEdit, EM_CANUNDO, 0, 0)) SendMessage(hEdit, WM_UNDO, 0, 0);
			break;
		case ID_EDIT_CUT:
			OutputDebugString(TEXT("EDIT_CUT"));
			SendMessage(hEdit, WM_CUT, 0, 0);
			break;
		case ID_EDIT_COPY:
			OutputDebugString(TEXT("EDIT_COPY"));
			SendMessage(hEdit, WM_COPY, 0, 0);
			break;
		case ID_EDIT_PASTE:
			OutputDebugString(TEXT("EDIT_PASTE"));
			SendMessage(hEdit, WM_PASTE, 0, 0);
			break;
		case ID_EDIT_DELETE:
			OutputDebugString(TEXT("EDIT_DELETE"));
			SendMessage(hEdit, WM_CLEAR, 0, 0);
			break;
		case ID_EDIT_SELECTALL:
			OutputDebugString(TEXT("EDIT_SELECTALTEXT("));
			SendMessage(hEdit, EM_SETSEL, 0, -1);
			break;
		case ID_FORMAT_FONT:
			OutputDebugString(TEXT("FORMAT_FONT"));
			/*	FONT STRUCTURE	*/
			cf.lStructSize = sizeof(CHOOSEFONT);
			cf.hwndOwner = hEdit;
			cf.lpLogFont = &lf;
			cf.Flags = CF_EFFECTS | CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS;
			
			if(ChooseFont(&cf))
			{
				DeleteObject(font);
				font = CreateFontIndirect(&lf); 
				SendMessage(hEdit, WM_SETFONT, (WPARAM)font, 0);
				GetClientRect(hEdit,&rect);
				InvalidateRect(hEdit,&rect,0);
			}
			break;
		case ID_EDIT_TIME:
			time(&now);
			szNow =  _tctime(&now);
			SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szNow);
			break;
		case ID_FORMAT_WORDWRAP:
			OutputDebugString(TEXT("FORMAT_WORDWRAP"));
			/*
			SendMessage(hEdit, 
                        EM_SETWORDBREAKPROC, 
                        (WPARAM) 0, 
                        (LPARAM) (EDITWORDBREAKPROC) WordBreakProc); 

            SendMessage(hEdit, 
                        EM_FMTLINES, 
                        (WPARAM) TRUE, 
                        (LPARAM) 0); 

            SendMessage(hEdit, 
                        EM_SETSEL, 
                        0, -1);             // select all text 

            SendMessage(hwndEdit, WM_CUT, 0, 0); 

            SendMessage(hwndEdit, WM_PASTE, 0, 0); 
			*/
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

// Message handler for about box.
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

void WhatError()
{ /* Code from MSDN */
	LPVOID lpvbuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | /* LocalAlloc used and saves pointer in 5 argument.*/
    FORMAT_MESSAGE_FROM_SYSTEM,NULL,GetLastError(),
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
    (LPTSTR)&lpvbuf,0,NULL);
	MessageBox(NULL,(LPCTSTR)lpvbuf,TEXT("INFORMATION"), MB_OK | MB_ICONINFORMATION );
	LocalFree(lpvbuf);
}

;