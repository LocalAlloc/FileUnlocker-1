#pragma once
#include <Windows.h>
#include "stdafx.h"
#include <vector>

TCHAR szTitle[MAX_LOADSTRING];				// The title bar text;
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
HINSTANCE hInst;
HWND hWnd;
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
void DeleteSelectedItems();
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
// Define a structure to represent file and folder paths
struct PathInfo {
    std::wstring name;  // File or folder name
    std::wstring path;  // Full path
    bool isFolder;      // True if it's a folder, false if it's a file
};

// Create a vector to store these PathInfo objects
std::vector<PathInfo> pathList;

HWND hwndListView; // Handle to the ListView control
