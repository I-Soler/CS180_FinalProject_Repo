#pragma once
#define WIN32_LEAN_AND_MEAN // exclude win32 features that are rarely used. 
#include <windows.h>
#include <stdio.h>

// Global Platform API
namespace AEX
{
    namespace Platform
    {
        #pragma region  // Console Management
        static HWND sConsoleWin;
        void ShowConsole()
        {
            if (sConsoleWin)
            {
                HWND consoleWin = GetConsoleWindow();
                ShowWindow(consoleWin, SW_SHOW);
            }

            else if (AllocConsole())
            {
                FILE* fp;
                // redirect standard output streams to go to the console output. 
                freopen_s(&fp, "CONOUT$", "wt", stdout);
                freopen_s(&fp, "CONOUT$", "wt", stderr);
                freopen_s(&fp, "CONOUT$", "rt", stdin);

                sConsoleWin = GetConsoleWindow();
            }
        }

        void DestroyConsole()
        {
            if (sConsoleWin) {

                FreeConsole();
                PostMessage(sConsoleWin, WM_CLOSE, 0, 0);
                sConsoleWin = NULL;
            }
        }

        void ResetConsole()
        {
            DestroyConsole();
            ShowConsole();
        }

        void HideConsole()
        {
            HWND consoleWin = GetConsoleWindow();
            ShowWindow(consoleWin, SW_HIDE);
        }
        #pragma endregion

        // screen size
        void GetScreenSize(int& sW, int& sH)
        {
            RECT rcWork;
            SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWork, 0);
            sW = rcWork.right - rcWork.left;
            sH = rcWork.bottom - rcWork.top;
        }
    }
}