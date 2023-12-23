// FileUnlocker.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Windows.h>
#include "stdafx.h"
#include "resource.h"
#include "FileUnlocker.h"
#include "Utils.h"


int WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nCmdShow
)
{
	INITCOMMONCONTROLSEX icex;
	icex.dwICC = ICC_BAR_CLASSES | ICC_LISTVIEW_CLASSES | ICC_NATIVEFNTCTL_CLASS | ICC_STANDARD_CLASSES;
	icex.dwSize = sizeof(icex);
	InitCommonControlsEx(&icex);

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	MSG msg;
	HACCEL hAccelTable;
	const char* szTitle1 = "FileUnlocker";

	LoadString(hInstance,	NULL, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDR_MENU1, szWindowClass, MAX_LOADSTRING);
	if (!MyRegisterClass(hInstance)) {
		MessageBoxA(NULL, "Failed!", "Failed!", MB_OK | MB_ICONINFORMATION);
		return FALSE;
	}

	if (!InitInstance(hInstance, nCmdShow))
	{
		MessageBoxA(NULL, "Failed!", "Failed!", MB_OK | MB_ICONINFORMATION);
		return FALSE;
	}
	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));
	//handle incoming messages
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
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	wcex.lpszClassName = L"FileProtector";
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));

	return RegisterClassEx(&wcex);
}
void UpdateListView(HWND hwndListView, const PathInfo& pathInfo)
{
	LVITEM lvItem;
	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = 0; // or the index at which you want to insert
	lvItem.iSubItem = 0; // subitem index
	lvItem.pszText = const_cast<LPTSTR>(pathInfo.name.c_str());

	if (ListView_InsertItem(hwndListView, &lvItem) != -1)
	{
		// Add the path to the ListView control in the next column
		lvItem.iSubItem = 1;
		lvItem.pszText = const_cast<LPTSTR>(pathInfo.path.c_str());

		if (ListView_SetItem(hwndListView, &lvItem))
		{
			// Successfully added the file/folder name and path to the ListView
		}
		else
		{
			MessageBox(NULL, _T("Failed To Add Path"), _T("Failed!"), MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
		}
	}
	else
	{
		MessageBox(NULL, _T("Failed To Add File/Folder Name"), _T("Failed!"), MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
	}
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;

	static HWND hwndBtnEnd;
	static HWND hwndBtnCreate;
	static HWND hwndListView;
	static HWND hwndListViewM;
	static HWND hwndLabelProcess;
	static HWND hwndStatusBar;

	PAINTSTRUCT ps;
	HDC hdc;
	RECT rect = { 0, 0, 0, 0 };

	switch (message)
	{
#pragma region Create
	case WM_CREATE:
	{
		GetClientRect(hWnd, &rect);
		//Creates Buttons
		hwndBtnCreate = CreateWindow(
			WC_BUTTON,
			_T("Unlock Files"),
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_TEXT | WS_TABSTOP,
			CBTN_MARGIN,											// x coord
			rect.bottom - 3 * CBTN_MARGIN - CBTN_Y,					// y coord
			CBTN_X, 												// cx
			CBTN_Y,													// cy
			hWnd, (HMENU)IDB_CREATE,
			hInst, NULL);

		hwndBtnEnd = CreateWindow(
			WC_BUTTON,
			_T("Unlock Folders"),
			WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_TEXT,
			rect.right - CBTN_MARGIN - CBTN_X,						// x coord
			rect.bottom - 3 * CBTN_MARGIN - CBTN_Y,					// y coord
			CBTN_X,													// cx
			CBTN_Y,													// cy
			hWnd,
			(HMENU)IDB_END,
			hInst, NULL);

		//hwndListView = CreateWindow(
		//	WC_LISTVIEW,
		//	_T(""),
		//	WS_CHILD | WS_VISIBLE | LVS_REPORT | WS_BORDER | WS_TABSTOP |
		//	LVS_SINGLESEL | LVS_SHOWSELALWAYS | WS_VSCROLL | WS_HSCROLL,
		//	CBTN_MARGIN,											// x coord
		//	CBTN_MARGIN,											// y coord
		//	rect.right - 2 * CBTN_MARGIN,							// cx
		//	rect.bottom - 7 * CBTN_MARGIN,							// cy
		//	hWnd,
		//	(HMENU)4,
		//	hInst, NULL);
		// Create the ListView control
		hwndListView = CreateWindow(
			WC_LISTVIEW,
			_T(""),
			WS_CHILD | WS_VISIBLE | LVS_REPORT | WS_BORDER | WS_TABSTOP |
			LVS_SINGLESEL | LVS_SHOWSELALWAYS | WS_VSCROLL | WS_HSCROLL,
			CBTN_MARGIN,                // x coord
			CBTN_MARGIN,                // y coord
			rect.right - 2 * CBTN_MARGIN,   // cx
			rect.bottom - 7 * CBTN_MARGIN,  // cy
			hWnd,
			(HMENU)4,
			hInst, NULL
		);

		// Add columns to the ListView control
		LVCOLUMN lvColumn = { 0 };
		lvColumn.mask = LVCF_TEXT | LVCF_WIDTH;
		lvColumn.cx = 300; // Adjust the column width as needed
		lvColumn.pszText = (LPWSTR)L"File Name / Folder Name";
		ListView_InsertColumn(hwndListView, 0, &lvColumn);

		lvColumn.cx = 400; // Adjust the column width as needed
		lvColumn.pszText = (LPWSTR)L"File Path / Folder Path";
		ListView_InsertColumn(hwndListView, 1, &lvColumn);

		ListView_SetExtendedListViewStyle(hwndListView,
			LVS_EX_DOUBLEBUFFER | LVS_EX_UNDERLINEHOT | LVS_EX_ONECLICKACTIVATE);

		hwndStatusBar = CreateWindow(
			STATUSCLASSNAME,
			_T("List"),
			SBARS_SIZEGRIP | WS_CHILD | WS_VISIBLE,
			0, 0, 0, 0,
			hWnd,
			(HMENU)NULL,
			hInst, NULL);

		EnumChildWindows(hWnd, SetChildFont, (LPARAM)GetMessageFont());
		break;
	}
#pragma endregion

	case WM_SIZE:
	{
		DWORD dwNewWidth = LOWORD(lParam);
		DWORD dwNewHeight = HIWORD(lParam);

		MoveWindow(
			hwndBtnCreate,
			CBTN_MARGIN,											// x coord
			dwNewHeight - 3 * CBTN_MARGIN - CBTN_Y,					// y coord
			CBTN_X, 												// cx
			CBTN_Y,													// cy
			TRUE);

		MoveWindow(
			hwndBtnEnd,
			dwNewWidth - CBTN_MARGIN - CBTN_X,						// x coord
			dwNewHeight - 3 * CBTN_MARGIN - CBTN_Y,					// y coord
			CBTN_X,													// cx
			CBTN_Y,													// cy
			TRUE);

		MoveWindow(
			hwndListView,
			CBTN_MARGIN,											// x coord
			CBTN_MARGIN,											// y coord
			dwNewWidth - 2 * CBTN_MARGIN,							// cx
			dwNewHeight - 7 * CBTN_MARGIN,							// cy
			TRUE);

		SendMessage(hwndStatusBar, WM_SIZE, lParam, wParam);
	}
	break;

	case WM_WINDOWPOSCHANGING:
	{
		PWINDOWPOS pWindowPos = (PWINDOWPOS)lParam;

		//if (pWindowPos->cx < 200 || pWindowPos->cy < 150)
		//pWindowPos->flags |= SWP_NOSIZE;

		if (pWindowPos->cx < 250)
			pWindowPos->cx = 250;

		if (pWindowPos->cy < 400)
			pWindowPos->cy = 400;
	}
	break;

#pragma region Command
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		switch (wmId)
		{
		case ID_ARCHIVE_EXIT:
			DestroyWindow(hWnd);
			break;
		case IDB_CREATE:
		{
			TCHAR filePath[MAX_PATH] = { 0 };

			OPENFILENAME openFileName = { 0 };
			openFileName.lStructSize = sizeof(OPENFILENAME);
			openFileName.hwndOwner = hWnd;
			openFileName.lpstrFile = filePath;
			openFileName.nMaxFile = MAX_PATH;
			openFileName.lpstrTitle = _T("Select a File");
			openFileName.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;

			if (GetOpenFileName(&openFileName))
			{
				// Extract the file name and path
				const TCHAR* fileName = PathFindFileName(filePath);
				const TCHAR* path = filePath;

				// Create a PathInfo object and store it in the vector
				PathInfo pathInfo;
				pathInfo.name = fileName;
				pathInfo.path = path;
				pathInfo.isFolder = false;

				pathList.push_back(pathInfo);

				// Update the ListView control
				UpdateListView(hwndListView, pathInfo);
				if (MessageBoxA(NULL, "Want To Delete Files?, continue?", "Warning!", MB_YESNO | MB_ICONHAND | MB_TOPMOST) == IDYES) {
					DeleteSelectedItems();
				}
				else {
					break;
				}
			}
			break;
		}
		case IDB_END:
		{
			BROWSEINFO browseInfo = { 0 };
			TCHAR folderPath[MAX_PATH] = { 0 };
			browseInfo.hwndOwner = hWnd;
			browseInfo.pidlRoot = NULL;
			browseInfo.pszDisplayName = folderPath;
			browseInfo.lpszTitle = _T("Select a Folder");
			browseInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

			LPITEMIDLIST pidl = SHBrowseForFolder(&browseInfo);
			if (pidl)
			{
				if (SHGetPathFromIDList(pidl, folderPath))
				{
					// Extract the folder name and path
					const TCHAR* folderName = PathFindFileName(folderPath);
					const TCHAR* path = folderPath;

					// Create a PathInfo object and store it in the vector
					PathInfo pathInfo;
					pathInfo.name = folderName;
					pathInfo.path = path;
					pathInfo.isFolder = true;

					pathList.push_back(pathInfo);

					// Update the ListView control
					UpdateListView(hwndListView, pathInfo);
				}
				CoTaskMemFree(pidl);
			}
			break;
		}
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
#pragma endregion

	case WM_NOTIFY:
		switch (((LPNMHDR)lParam)->code)
		{
		case NM_DBLCLK:
			if (((LPNMHDR)lParam)->hwndFrom == hwndListView)
			{
				//ShowModules(hwndListView);
			}
			break;
		}
		break;

	case WM_PAINT:
	{
		hdc = BeginPaint(hWnd, &ps);
		GetClientRect(hWnd, &rect);
		EndPaint(hWnd, &ps);

		break;
	}

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance;

	hWnd = CreateWindowEx(
#ifdef _DEBUG
		NULL,
#else
		//WS_EX_TOPMOST, Same Thing Though 
		NULL,
#endif
		L"FileProtector", L"FileUnlocker", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, 600, 500, NULL, NULL, hInst, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}
void DeleteSelectedItems()
{
	for (const PathInfo& pathInfo : pathList)
	{
		if (pathInfo.isFolder)
		{
			// Handle folder deletion
			// Convert std::wstring to LPCWSTR
			LPCWSTR lpcwstrFolderPath = pathInfo.path.c_str();
			if (RemoveDirectory(lpcwstrFolderPath))
			{
				// Folder deleted successfully
			}
			else
			{
				// Handle folder deletion failure
				DWORD error = GetLastError();
				// You can display an error message or log the error here.
			}
		}
		else
		{
			LPCWSTR lpcwstrFilePath = pathInfo.path.c_str();
			DWORD dwSession;
			WCHAR szSessionKey[CCH_RM_SESSION_KEY + 1] = { 0 };
			DWORD dwError = RmStartSession(&dwSession, 0, szSessionKey);

			if (dwError == ERROR_SUCCESS) {
				std::wstring wstrFilePath = pathInfo.path;

				const wchar_t* pszFilePath = wstrFilePath.c_str();

				dwError = RmRegisterResources(dwSession, 1, &pszFilePath, 0, NULL, 0, NULL);

				if (dwError == ERROR_SUCCESS || dwError == ERROR_RESOURCE_NOT_FOUND) {
					DWORD dwReason;
					UINT i;
					UINT nProcInfoNeeded;
					UINT nProcInfo = 10;
					RM_PROCESS_INFO rgpi[10];
					dwError = RmGetList(dwSession, &nProcInfoNeeded, &nProcInfo, rgpi, &dwReason);

					if (dwError == ERROR_SUCCESS) {
						bool isFileUsed = false;

						for (i = 0; i < nProcInfo; i++) {
							//std::wstring appTypeMsg = L"ApplicationType: " + std::to_wstring(rgpi[i].ApplicationType);
							//MessageBox(NULL, appTypeMsg.c_str(), L"Application Type", MB_OK | MB_ICONINFORMATION | MB_TOPMOST);

							std::wstring appNameMsg = L"AppName: " + std::wstring(rgpi[i].strAppName);
							MessageBox(NULL, appNameMsg.c_str(), L"App Name", MB_OK | MB_ICONINFORMATION | MB_TOPMOST);

							std::wstring procIdMsg = L"ProcessId: " + std::to_wstring(rgpi[i].Process.dwProcessId);
							MessageBox(NULL, procIdMsg.c_str(), L"Process ID", MB_OK | MB_ICONINFORMATION | MB_TOPMOST);

							HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_TERMINATE, FALSE, rgpi[i].Process.dwProcessId);
							if (hProcess) {
								FILETIME ftCreate, ftExit, ftKernel, ftUser;
								if (GetProcessTimes(hProcess, &ftCreate, &ftExit, &ftKernel, &ftUser) &&
									CompareFileTime(&rgpi[i].Process.ProcessStartTime, &ftCreate) == 0) {
									WCHAR sz[MAX_PATH];
									DWORD cch = MAX_PATH;
									if (QueryFullProcessImageNameW(hProcess, 0, sz, &cch) && cch <= MAX_PATH) {
										std::wstring pathMsg = L"Process Path: " + std::wstring(sz);
										MessageBox(NULL, pathMsg.c_str(), L"Process Path", MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
									}
								}
							}
							TerminateProcess(hProcess, 0);
							CloseHandle(hProcess);
							Sleep(1000);
							//DeleteFileHandle(wstrFilePath);
							RmEndSession(dwSession);
							break;
						}
					}
					else {
						// Failed to retrieve process list
						MessageBoxW(NULL, L"Failed to retrieve process list!", L"Error", MB_OK | MB_ICONSTOP | MB_TOPMOST);
					}
				}

				RmEndSession(dwSession);
			}
			// Handle file deletion
			if (DeleteFile(lpcwstrFilePath))
			{
				// File deleted successfully
			}
			else
			{
				// Handle file deletion failure
				DWORD error = GetLastError();
				// You can display an error message or log the error here.
			}
		}
	}

	// After deleting the selected items, you may want to update the ListView and pathList accordingly.
}
int UseStandard(const std::wstring& filePath) {
	// Prepare the command to be executed
	wchar_t command[512];
	swprintf(command, 512, L"cmd.exe /c takeown /f \"%s\" & icacls \"%s\" /grant *S-1-3-4:F /t /c /l", filePath.c_str(), filePath.c_str());

	// Create the process information structures
	STARTUPINFOW si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));
	si.cb = sizeof(si);

	// Create the process with the hidden console window
	//CREATE_NO_WINDOW
	if (CreateProcessW(NULL, &command[0], NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
		// Wait for the process to exit
		WaitForSingleObject(pi.hProcess, INFINITE);

		// Get the exit code
		DWORD exitCode = 0;
		GetExitCodeProcess(pi.hProcess, &exitCode);

		// Clean up process handles
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);

		// Check if the process was successful
		if (exitCode != 0) {
			return GetLastError();
			// Handle the error
		}
	}
	else {
		return GetLastError();
		// Handle the error
	}

	return ERROR_SUCCESS; // Ownership and permission changes successful
}
int TakeOWNDirectory(const std::wstring& filePath) {
	// Prepare the command to be executed
	std::wstring command = L"cmd.exe /c takeown /f \"" + filePath + L"\" /r /d y & icacls \"" + filePath + L"\" /grant *S-1-3-4:F /t /c /l";

	// Create the process information structures
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));
	si.cb = sizeof(si);

	// Create the process with the hidden console window
	//CREATE_NO_WINDOW
	if (CreateProcessW(NULL, &command[0], NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
		// Wait for the process to exit
		WaitForSingleObject(pi.hProcess, INFINITE);

		// Get the exit code
		DWORD exitCode = 0;
		GetExitCodeProcess(pi.hProcess, &exitCode);

		// Clean up process handles
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);

		// Check if the process was successful
		if (exitCode != 0) {
			return GetLastError();
			// Handle the error
		}
	}
	else {
		return GetLastError();
		// Handle the error
	}

	return ERROR_SUCCESS; // Ownership and permission changes successful
}
bool DeleteFileSp(const std::wstring& filePath)
{
	std::wstring extendedPath = L"\\\\?\\" + filePath;

	if (DeleteFileW(extendedPath.c_str()))
	{
		// Check if the file still exists
		if (GetFileAttributesW(extendedPath.c_str()) == INVALID_FILE_ATTRIBUTES)
		{
			// File deleted successfully
			return true;
		}
		else
		{
			// File still exists, consider it a failure to delete
			return false;
		}
	}
	else
	{
		// Failed to delete the file
		return false;
	}
}
int DeleteFileHandle(const std::wstring& filePath)
{
	UseStandard(filePath);
	HANDLE hFile = CreateFileW(filePath.c_str(), GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_FLAG_DELETE_ON_CLOSE, nullptr);
	if (hFile == INVALID_HANDLE_VALUE) {
		//Failed To Delete File
		return false;
	}
	else {
		CloseHandle(hFile);
		if (GetFileAttributesW(filePath.c_str()) == INVALID_FILE_ATTRIBUTES && GetLastError() == ERROR_FILE_NOT_FOUND) {
			//MessageBoxA(NULL, "File deleted successfully!", "", MB_OK | MB_ICONINFORMATION);
		}
		else {
			//MessageBoxA(NULL, "Failed to delete the file!", "Error", MB_OK | MB_ICONSTOP | MB_TOPMOST);
		}
	}
}