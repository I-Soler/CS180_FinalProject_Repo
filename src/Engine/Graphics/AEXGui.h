#pragma once
#include <Core/AEXCore.h>
namespace AEX {
	class GUI : public IBase
	{
		AEX_RTTI_DECL(GUI, IBase);
		AEX_SINGLETON(GUI) {}
	public:
		bool Initialize();
		void Shutdown();

		// This function should be called before calling any ImGui functions
		// it marks the start of a new frame of GUI and therefore should be placed 
		// at the beginning of the frame, after input has updated but before Logic updates. 
		// Note that NewFrame shouldn't be called anymore until EndFrame is called. Otherwise, 
		// ImGui will throw an error. 
		void NewFrame();

		// This function marks the end of the frame for ImGui. It will draw the GUI resulting
		// from all the ImGui calls that were made since NewFrame was last called. 
		// Because it will draw to the framebuffer, this function should be called just before
		// the graphics engine passes the framebuffer to the video display (i.e in aexGraphics->Present())
		void EndFrame();
	};
}
#define aexGui AEX::Singletons::Get<AEX::GUI>()
