// ---------------------------------------------------------------------------
// Project Name		:	Alpha Engine
// File Name		:	AEInput.cpp
// Author			:	Sun Tjen Fam, Antoine Abi Chakra
// Creation Date	:	2008/0131
// Purpose			:	Input wrapper
// History			:
// - 2008/01/31		:	- initial implementation
// ---------------------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN // exclude win32 features that are rarely used. 
#include <windows.h>
#include "AEXWindow.h"
#include "AEXInput.h"
#include <iostream>

namespace AEX
{
	// ---------------------------------------------------------------------------
	Input::Input()
	{}

	// ---------------------------------------------------------------------------
	Input::~Input()
	{}
	
	// ---------------------------------------------------------------------------
	bool Input::Initialize()
	{
		std::memset(mKeyCurr, 0, sizeof(u8) * AEX_INPUT_KEY_NUM);
		std::memset(mKeyPrev, 0, sizeof(u8) * AEX_INPUT_KEY_NUM);

		std::memset(mMouseCurr, 0, sizeof(u8) * AEX_INPUT_MOUSE_NUM);
		std::memset(mMousePrev, 0, sizeof(u8) * AEX_INPUT_MOUSE_NUM);

		mAnyKey = false;
		mAnyKeyTriggered = false;
		mKeyPressed.clear();
		mKeyTriggered.clear();
		mMouseWheel = 0.0f;
		return true;
	}

	// ---------------------------------------------------------------------------
	namespace internals
	{
		bool mouseWheelThisFrame = false;
	} using namespace internals;
	bool Input::HandleWin32Message(u32 msg, void* _wParam, void* _lParam)
	{
		AE_UNUSED(_lParam);
		WPARAM wParam = (WPARAM)_wParam;

		switch (msg)
		{
			// Keyboard
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			if (wParam == VK_MENU)
			{
				mKeyCurr[Keys::Alt] = 1;
			}
			else
				mKeyCurr[wParam] = 1;
			break;

		case WM_KEYUP:
		case WM_SYSKEYUP:
			if (wParam == VK_MENU)
			{
				mKeyCurr[Keys::Alt] = 0;
			}
			else
				mKeyCurr[wParam] = 0;
			break;

			// Mouse Right Button		
		case WM_RBUTTONDOWN:
			mMouseCurr[Keys::MouseRight] = 1;
			break;

		case WM_RBUTTONUP:
			mMouseCurr[Keys::MouseRight] = 0;
			break;
			// Mouse Left Button
		case WM_LBUTTONDOWN:
			mMouseCurr[Keys::MouseLeft] = 1;
			break;

		case WM_LBUTTONUP:
			mMouseCurr[Keys::MouseLeft] = 0;
			break;
			// Mouse Left Button
		case WM_MBUTTONDOWN:
			mMouseCurr[Keys::MouseMiddle] = 1;
			break;

		case WM_MBUTTONUP:
			mMouseCurr[Keys::MouseMiddle] = 0;
			break;
			// not an input message, return false
		case WM_MOUSEWHEEL:
		{
			short  delta = GET_WHEEL_DELTA_WPARAM(wParam);
			mMouseWheel = (f32)delta / 120.0f;
			mouseWheelThisFrame = true;
			break;
		}
		default:
			return false;
		}
		return true;
	}

	// ---------------------------------------------------------------------------

	void Input::Update()
	{
		int i;		// counter for loop
		POINT mp;	// to store the mouse position

		//Get the mouse position in screen coordinate
		GetCursorPos(&mp);

		// Get the mouse position in screen coordinate

		ScreenToClient((HWND)aexWindow->GetHandle(), &mp);

		mMousePrevPos = mMousePos;
		mMousePos.x = (float)mp.x;
		mMousePos.y = (float)mp.y;

		f32 win_width = (f32)aexWindow->GetWidth();
		f32 win_height = (f32)aexWindow->GetHeight();

		/* Step 3:  Compute whether the mouse position is inside the window*/
		mMouseInWindow = !(mMousePos.x < 0 || mMousePos.x >  win_width ||
			mMousePos.y < 0 || mMousePos.y >  win_height);

		/* Step 4: Compute the mouse position in centered window coordinate */

		mMousePos.x = mMousePos.x - win_width / 2;
		mMousePos.y = -mMousePos.y + win_height / 2;

		//printf("Mouse pos: %i, %i = %f, %f\n", mp.x, mp.y, mMousePos.x, mMousePos.y);

		// compute mouse difference
		mMouseDiff.x = mMousePrevPos.x - mMousePos.x;
		mMouseDiff.y = mMousePrevPos.y - mMousePos.y;

		// reset the mouse wheel
		if (mouseWheelThisFrame)
			mouseWheelThisFrame = false;
		else
			mMouseWheel = 0.0f;


		// Clear the keys that were pressed last frame
		mKeyPressed.clear();
		mKeyTriggered.clear();
		mAnyKey = false;
		mAnyKeyTriggered = false;

		// Determine whether a key was triggered or not
		for (i = 0; i < AEX_INPUT_KEY_NUM; ++i)
		{
			if (mKeyCurr[i])
			{
				// any keys has been pressed
				mAnyKey = true;
				mKeyPressed.push_back((u8)i);

				if (!mKeyPrev[i])
				{
					mAnyKeyTriggered = true;
					mKeyTriggered.push_back((u8)i);
					mKeyCurr[i] = 2;
				}
				else
					mKeyCurr[i] = 1;
			}
			mKeyPrev[i] = mKeyCurr[i];
		}

		//Determine whether a mouse button was triggered or not
		for (i = 0; i < AEX_INPUT_MOUSE_NUM; ++i)
		{
			if (mMouseCurr[i])
			{
				if (!mMousePrev[i])
				{
					mMouseCurr[i] = 2;
				}
				else
					mMouseCurr[i] = 1;
			}
			mMousePrev[i] = mMouseCurr[i];
		}
	}

	// ---------------------------------------------------------------------------

	const std::vector<u8> & Input::AllKeyPressed()
	{
		return mKeyPressed;
	}

	// ---------------------------------------------------------------------------

	const std::vector<u8> & Input::AllKeyTriggered()
	{
		return mKeyTriggered;
	}


	// ---------------------------------------------------------------------------

	bool Input::AnyKey()
	{
		return mAnyKey;
	}

	// ---------------------------------------------------------------------------

	bool Input::AnyKeyTriggered()
	{
		return mAnyKeyTriggered;
	}

	// ---------------------------------------------------------------------------

	bool Input::KeyPressed(int key)
	{
		if (key > AEX_INPUT_KEY_NUM - 1)
			return 0;

		if (key == Keys::Any)
			return mAnyKey;

		return mKeyCurr[key] > 0;
	}

	// ---------------------------------------------------------------------------

	bool Input::KeyTriggered(int key)
	{
		if (key > AEX_INPUT_KEY_NUM - 1)
			return 0;

		if (key == Keys::Any)
			return mAnyKey;
		return mKeyCurr[key] == 2;
	}

	// ---------------------------------------------------------------------------

	bool Input::MousePressed(int button)
	{
		if (button > Keys::MouseMiddle)
			return 0;

		return mMouseCurr[button] > 0;
	}

	// ---------------------------------------------------------------------------

	bool Input::MouseTriggered(int button)
	{
		if (button > Keys::MouseMiddle)
			return 0;

		return mMouseCurr[button] == 2;
	}

	// ---------------------------------------------------------------------------


	bool Input::MouseReleased(int button)
	{
		if (button > Keys::MouseMiddle)
			return 0;

		std::cout << mMousePrev[button] << std::endl;
		return (mMousePrev[button] != 0 && mMouseCurr[button] == 0);
	}

	// ---------------------------------------------------------------------------

	AEVec2 Input::GetMousePos()
	{
		return mMousePos;
	}

	// ---------------------------------------------------------------------------

	AEVec2 Input::GetPrevMousePos()
	{
		return mMousePrevPos;
	}

	// ---------------------------------------------------------------------------

	AEVec2 Input::GetMouseMovement()
	{
		return mMouseDiff;
	}

	// ---------------------------------------------------------------------------

	bool	  Input::MouseInWindow()
	{
		return mMouseInWindow;
	}

	// ---------------------------------------------------------------------------
	f32		Input::GetMouseWheel()
	{
		return mMouseWheel;
	}
}