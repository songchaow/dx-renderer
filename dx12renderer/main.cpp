// dx12renderer.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "dx12renderer.h"
#include "d3dbootstrap.h"
#include "ext/imgui/imgui.h"
#include "ext/imgui/imgui_impl_win32.h"
#include "ext/imgui/imgui_impl_dx12.h"
#include "engine/scene.h"
#include "engine/shader.h"
#include "main.h"
//#include "common/camera.h"

// Global Variables:
HINSTANCE hInst;                                // current instance
HWND hWindow;                                   // current window handle
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
WNDCLASSEXW windowClass;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);



int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_DX12RENDERER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    D3DRenderer renderer;

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return -1;
    }

    if(!renderer.d3d_init(hWindow))
        return -1;

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DX12RENDERER));

    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    // Main message loop:
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }
        renderer.render_frame();
    }

    // cleanup 
    WaitForLastSubmittedFrame();
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hWindow);
    ::UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);

    return (int) msg.wParam;
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

      HWND hwnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

      if (!hwnd)
      {
            return FALSE;
      }
      hWindow = hwnd;

      
      
      ShowWindow(hwnd, nCmdShow);
      UpdateWindow(hwnd);
      return TRUE;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    

    windowClass.cbSize = sizeof(WNDCLASSEX);

    windowClass.style          = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc    = WndProc;
    windowClass.cbClsExtra     = 0;
    windowClass.cbWndExtra     = 0;
    windowClass.hInstance      = hInstance;
    windowClass.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DX12RENDERER));
    windowClass.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    windowClass.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    windowClass.lpszMenuName   = MAKEINTRESOURCEW(IDC_DX12RENDERER);
    windowClass.lpszClassName  = szWindowClass;
    windowClass.hIconSm        = LoadIcon(windowClass.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&windowClass);
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
        return true;
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
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
