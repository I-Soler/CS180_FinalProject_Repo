// ---------------------------------------------------------------------------
// Project Name		:	Alpha Engine
// File Name		:	AEInput.h
// Author			:	Sun Tjen Fam
// Creation Date	:	2008/01/31
// Purpose			:	declaration for input stuff
// History			:
// - 2008/01/31		:	- initial implementation
// ---------------------------------------------------------------------------

#ifndef AEX_INPUT_H
#define AEX_INPUT_H
// ---------------------------------------------------------------------------
// Defines/Enums


#define AEX_INPUT_KEY_NUM	256
#define AEX_INPUT_MOUSE_NUM 3

#include <Core\AEXCore.h>
#include <aexmath\AEXVec2.h>
namespace AEX
{
	namespace Keys {
		enum KeyCodes
		{
			Any					= -1,
			MouseLeft			= 0,
			MouseRight			= 1,
			MouseMiddle			= 2,
			Control				= 17,
			Shift				= 16,
			Clear				= 0x0C,
			Return				= 0x0D,
			BackSpace			= 8,
			Tab					= 9,
			PageUp				= 0x21,
			PageDown			= 0x22,
			End					= 0x23,
			Home				= 0x24,
			Left				= 0x25,
			Up					= 0x26,
			Right				= 0x27,
			Down				= 0x28,
			Insert				= 0x2D,
			Alt					= 10,
			Escape				= 0x1B,
			Space				= 32,
			Delete				= 0x2E,
			Numpad0				= 0x60,
			Numpad1				= 0x61,
			Numpad2				= 0x62,
			Numpad3				= 0x63,
			Numpad4				= 0x64,
			Numpad5				= 0x65,
			Numpad6				= 0x66,
			Numpad7				= 0x67,
			Numpad8				= 0x68,
			Numpad9				= 0x69,
			Multiply			= 0x6A,
			Add					= 0x6B,
			Separator			= 0x6C,
			Subtract			= 0x6D,
			Decimal				= 0x6E,
			Divide				= 0x6F,
			F1					= 0x70,
			F2					= 0x71,
			F3					= 0x72,
			F4					= 0x73,
			F5					= 0x74,
			F6					= 0x75,
			F7					= 0x76,
			F8					= 0x77,
			F9					= 0x78,
			F10					= 0x79,
			F11					= 0x7A,
			F12					= 0x7B,
			Plus				=     0xBB,   // '+' any country
			Comma				=     0xBC,   // ',' any country
			Minus				=     0xBD,   // '-' any country
			Period				=     0xBE,   // '.' any country
		};

	}
	// ---------------------------------------------------------------------------
	// class definitions
	class  Input : public IBase
	{
		AEX_RTTI_DECL(Input, IBase);
		AEX_SINGLETON(Input);

	public:
		
	public:

		// ISystem
		virtual bool Initialize(); 
		virtual void Update();
		virtual ~Input();

		// Input
		bool HandleWin32Message(u32 msg, void* wParam, void* lParam);
		bool AnyKey();
		bool AnyKeyTriggered();
		bool KeyPressed(int key);
		bool KeyTriggered(int key);
		bool MousePressed(int button);
		bool MouseTriggered(int button);
		bool MouseReleased(int button);

		AEVec2 GetMousePos();
		AEVec2 GetPrevMousePos();
		AEVec2 GetMouseMovement();
		f32		GetMouseWheel();
		bool MouseInWindow();

		const std::vector<u8> & AllKeyPressed();
		const std::vector<u8> & AllKeyTriggered();

	private:

		u8	mKeyCurr[AEX_INPUT_KEY_NUM];
		u8	mKeyPrev[AEX_INPUT_KEY_NUM];
		u8	mMouseCurr[AEX_INPUT_MOUSE_NUM];
		u8	mMousePrev[AEX_INPUT_MOUSE_NUM];
		f32 mMouseWheel;
		AEVec2 mMousePos;			// Mouse Position in Centered Coordinates
		AEVec2 mMousePrevPos;		// Previous Mouse Position in Centered Coordinates
		AEVec2 mMouseDiff;		// Mouse movement this update: Prev - Curr
		bool		 mMouseInWindow;	// Specifies whether the mouse is contained insisde the window


		bool				mAnyKey;	 // Any key is pressed. 
		bool				mAnyKeyTriggered;	 // Any key is pressed. 
		std::vector<u8>	mKeyPressed; // keys pressed last frame.
		std::vector<u8>	mKeyTriggered; // keys pressed last frame.
	};
}

// Easy access to singleton
#define aexInput (AEX::Singletons::Get<AEX::Input>())
// ---------------------------------------------------------------------------

#endif // AE_INPUT_H

