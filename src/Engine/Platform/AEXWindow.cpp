// ----------------------------------------------------------------------------
// File: WindowClass.cpp
// Project: Thomas Komair
// Purpose: Defines a class that wraps around a Win32 window.
// Author: Alpha Engine
// Copyright DigiPen Institute of Technology, 2013. All rights reserved
// ----------------------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN // exclude win32 features that are rarely used. 
#include <windows.h>
#include <shellapi.h>
#include "..\Debug\AEXDebug.h"
#include "AEXWindow.h"
#include "AEXInput.h"
#include <strsafe.h>		// WIN32 DEBUG
#include "ResourceMgr/ResourceMgr.h"
namespace AEX
{
	static HWND sWinHandle;           // handle to the WIN32 window
	static HINSTANCE sInstance;    // handle to the module associated with the window. 
	static WNDCLASSEX sWinClass;        // window class for gam199 windows
	static LPCSTR sWinClassName = "AEX Window";

    LRESULT CALLBACK ProxyWinProc(
        _In_ HWND   hWnd,
        _In_ UINT   message,
        _In_ WPARAM wParam,
        _In_ LPARAM lParam
    );

    // Register Our Window Class
    bool CreateWin32WindowClass()
    {
        sInstance = GetModuleHandle(NULL);

        // Register my window class
        // create a WNDCLASSEX structure and populate it
        sWinClass.cbSize = sizeof(WNDCLASSEX);
        sWinClass.style = CS_HREDRAW | CS_VREDRAW;
        sWinClass.lpfnWndProc = ProxyWinProc;
        sWinClass.cbClsExtra = 0;
        sWinClass.cbWndExtra = 0;
        sWinClass.hInstance = sInstance;
        sWinClass.hIcon = LoadIcon(sInstance, IDI_APPLICATION);
        sWinClass.hCursor = LoadCursor(NULL, IDC_ARROW);
        sWinClass.hbrBackground = (HBRUSH)(COLOR_GRAYTEXT + 1);
        sWinClass.lpszMenuName = NULL;
        sWinClass.lpszClassName = sWinClassName;
        sWinClass.hIconSm = LoadIcon(sWinClass.hInstance, IDI_APPLICATION);


        // register class 
        if (!RegisterClassEx(&sWinClass))
        {
            // dialog for error
            MessageBox(NULL,
                "Call to RegisterClassEx failed!",
                "Windows Desktop Guided Tour",
                NULL);
            return false;
        }
        return true;
    }
    bool Window::Initialize()
    {
        if (!CreateWin32WindowClass())return false;

        sWinHandle = CreateWindow(
            sWinClassName,            // the name of the window class application
            "Crazy Bubbles",          // the title of our window
            mWindowStyle,             // the type or style of the window -> title bar + borders
            10, 10,                   // position of the window
            1280, 720,                // size of the window. 
            NULL,                     // parent window (null because we have no parent). 
            NULL,                     // handle to the menu (null becaue we have none). 
            sInstance,                // instance of the program associated with the window.  
            NULL                      // to be passed to the window procedured. 
        );

        mPosX = mPosY = 10;
        SetSize(1920, 1080);

        if (!sWinHandle)
        {
            MessageBox(NULL,
                "Call to CreateWindow failed!",
                "Windows Desktop Guided Tour",
                NULL);
            mExists = false;
            return false;
        }
        mExists = true;

        // we accept drag drop
        DragAcceptFiles(
            sWinHandle,
            TRUE
        );

        // show the window
        ShowWindow(sWinHandle, SW_SHOW);
        UpdateWindow(sWinHandle);
        return true;
    }
    Window::Window()
        : mWindowStyle(WS_OVERLAPPEDWINDOW)
    {
    }
    void* Window::GetHandle()
    {
        return sWinHandle;
    }
    bool Window::Exists()
    {
        return mExists;
    }

    int Window::GetPosX() { return mPosX; }
    int Window::GetPosY() { return mPosY; }
    int Window::GetWidth() { return mWidth; }
    int Window::GetHeight() { return mHeight; }
    bool Window::GetFullscreen()
    {
        return this->mbFullscreen;
    }
    std::string Window::GetTitle() { return mTitle; }
    void Window::GetCurrentMonitorRect(int& left, int& top, int& right, int& bottom)
    {
        // Get monitor info
        HMONITOR currentMonitor = MonitorFromWindow(sWinHandle, MONITOR_DEFAULTTONEAREST);
        MONITORINFO info; info.cbSize = sizeof(MONITORINFO);
        GetMonitorInfo(currentMonitor, &info);

        // copy the data over from the monitor work area rectangle
        left = info.rcWork.left;
        top = info.rcWork.top;
        right = info.rcWork.right;
        bottom = info.rcWork.bottom;
    }

    // setters
    void Window::SetPosition(int x, int y)
    {
        mPosX = x;
        mPosY = y;
        UINT flags = SWP_NOZORDER | SWP_NOSIZE;
        SetWindowPos(sWinHandle, NULL, mPosX, mPosY, 0, 0, flags);
    }
    void Window::SetPosX(int x)
    {
        SetPosition(x, mPosY);
    }
    void Window::SetPosY(int y)
    {
        SetPosition(mPosX, y);
    }
    void Window::SetSize(int w, int h)
    {
        // Update the client size
        mWidth = w;
        mHeight = h;
        RECT metricRect = { 0, 0, w, h };

        // adjust the rectangle to fit the desired client area
        AdjustWindowRect(&metricRect, mWindowStyle, FALSE);

        // Update the metric size
        mMetricWidth = metricRect.right - metricRect.left;
        mMetricHeight = metricRect.bottom - metricRect.top;

        UINT flags = SWP_NOZORDER | SWP_NOMOVE;
        SetWindowPos(sWinHandle, NULL, 0, 0, mMetricWidth, mMetricHeight, flags);
    }
    void Window::SetWidth(int w)
    {
        SetSize(w, mHeight);
    }
    void Window::SetHeight(int h)
    {
        SetSize(mWidth, h);
    }
    void Window::SetTitle(const char* title)
    {
        mTitle = title;
        SetWindowText(sWinHandle, title);
    }
    void Window::SetFullscreen(bool enabled)
    {
        if (mbFullscreen == enabled)
            return;

        mbFullscreen = enabled;

        if (mbFullscreen) {

            int sW, sH, left, top, bottom, right;
            GetCurrentMonitorRect(left, top, right, bottom);
            sW = right - left;
            sH = bottom - top;

            // change the style of our window
            mWindowStyle = WS_POPUP;
            SetWindowLongPtr(sWinHandle, GWL_STYLE, mWindowStyle);
            UINT flags = SWP_FRAMECHANGED | SWP_SHOWWINDOW;
            //SetWindowPos(sWinHandle, NULL, 0, 0, sW, sH, flags);
            SetWindowPos(sWinHandle, NULL, left, top, sW, sH, flags);
        }
        else
        {
            mWindowStyle = WS_OVERLAPPEDWINDOW;
            SetWindowLongPtr(sWinHandle, GWL_STYLE, mWindowStyle);
            UINT flags = SWP_FRAMECHANGED | SWP_SHOWWINDOW;
            SetWindowPos(sWinHandle, NULL, mPosX, mPosY, mWidth, mHeight, flags);
        }
    }

    void Window::Close()
    {
        mExists = false;
    }

    void Window::Update()
    {
        MSG msg;
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                mExists = false;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }


	// ----------------------------------------------------------------------------
	// Window procedure call back. 

	/*
        ProxyWinProc:
        The reason we're using this function to call AEDefWinProc is so that
        we don't have to include <Windows.h> in AEWindow.h. Win32 is a huge API
        and since we'll likely include AEXWindow.h everywhere else in our engine, 
        we'll end up polluting the global namespace and making the compilation times greater. 

        AEDefWinProc only takes C++ types, while ProxyWinProc takes types declared in 
        <Windows.h>. 

        ProxyWinProc call AEDefWinProc and returns whatever it returns. 
    */
	int  AEDefWinProc(void* _hWnd, u32		_msg, void* _wp, void* _lp)
	{
        HWND hWin = (HWND)_hWnd;
        UINT msg = _msg;
        WPARAM wp = (WPARAM)_wp;
        LPARAM lp = (LPARAM)_lp;

		bool dodefault = false;

		switch (msg)
		{
		case WM_DESTROY:
            PostQuitMessage(0); break;
			break;
		case WM_CREATE:
			return 0;
			// called any time the window is resized
		case WM_SIZE:
		{
			u32 w = LOWORD(lp);
			u32 h = HIWORD(lp);
            aexWindow->mWidth = w;
            aexWindow->mHeight = h;
            RECT r = { 0,0,w,h };
            ::AdjustWindowRect(&r, aexWindow->mWindowStyle, FALSE);
            aexWindow->mMetricHeight = r.bottom - r.top;
            aexWindow->mMetricWidth = r.right - r.left;

		}break;

        case WM_DROPFILES:
        {
            u32 fileCount = DragQueryFile((HDROP)_wp, -1, 0, 0);
            char buffer[512];
            std::list<std::string> files;
            for (u32 i = 0; i < fileCount; ++i)
            {
                DragQueryFile((HDROP)_wp, i, buffer, 512);
                files.push_back(buffer);
            }
            // do something with this.... 
            // trigger an event with that data (when event system is done) -----------------------------------------------------------------------------------------------------
            aexResources->DragDrop(files);

        }   break;

		default:
			dodefault = true;
			break;
		}

		// Handle input messages
		if (aexInput->HandleWin32Message((u32)msg, (void*)wp, (void*)lp))
			return 0;

		// no one has handled the message, do default
		if (dodefault)
			return DefWindowProc(hWin, msg, wp, lp);

		// it was handled
		return 0;
	}
    LRESULT CALLBACK ProxyWinProc(HWND hWin, UINT msg, WPARAM wp, LPARAM lp)
    {
        return AEDefWinProc((void*)hWin, u32(msg), (void*)wp, (void*)lp);
    }
}