// ----------------------------------------------------------------------------
// File: WindowClass.h
// Project: Thomas Komair
// Purpose: Defines a class that wraps around a Win32 window.
// Author: Alpha Engine
// Copyright DigiPen Institute of Technology, 2013. All rights reserved
// ----------------------------------------------------------------------------

#ifndef AEX_WINDOW_H_
#define AEX_WINDOW_H_

#include "..\Core\AEXCore.h"
#include <string>
namespace AEX
{
	class Window : public IBase
	{
		AEX_RTTI_DECL(Window, IBase);
		AEX_SINGLETON(Window);

		// Fwd declaration of default window procedure
		friend int  AEDefWinProc(void* hWnd, u32 message, void * wParam, void *	lParam );

	public:

		// Initialize the window (actually creates it)
		bool Initialize();

		// getters
		void* GetHandle();
		bool Exists();
		int GetPosX();
		int GetPosY();
		int GetWidth();
		int GetHeight();
		bool GetFullscreen();
		std::string GetTitle();
		void GetCurrentMonitorRect(int& left, int& top, int& right, int& bottom);   // Current monitor

		// setters
		void SetPosition(int x, int y);
		void SetPosX(int x);
		void SetPosY(int y);
		void SetSize(int w, int h);
		void SetWidth(int w);
		void SetHeight(int h);
		void SetTitle(const char* title);
		void SetFullscreen(bool enabled);
		void Close();

		// update to read all messages
		void Update();

	private:
		long mWindowStyle;;
		int mPosX, mPosY;
		int mWidth, mHeight;               // client area size of the window
		int mMetricWidth, mMetricHeight;   // metric size of the window
		std::string mTitle;
		bool mbFullscreen = false;
		bool mExists = true;
	};
}
#define aexWindow AEX::Singletons::Get<AEX::Window>()
// ----------------------------------------------------------------------------
#endif