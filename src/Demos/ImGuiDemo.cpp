#include <AEX.h>
#include "Demos.h"
#include <iostream>
#include <Extern/imgui/imgui.h>

using namespace AEX;
static bool showImGuiDemoWindow = false;
// ----------------------------------------------------------------------------
// GAMESTATE FUNCTIONS
void GUIDemo::Initialize()
{
	showImGuiDemoWindow = false;
}
void GUIDemo::Update()
{	
	if (ImGui::Begin("My Window"))
	{
		static bool framerate = true;

		ImGui::Checkbox("Show Demo Window", &showImGuiDemoWindow);
		ImGui::Separator();
		if (ImGui::CollapsingHeader("Frame Rate")) {
			if (framerate)
				ImGui::Text("FPS: %.2f", aexTime->GetFrameRate());
			else
				ImGui::Text("Frame Time: %.3f", aexTime->GetFrameTime());

			if (ImGui::Button("Switch"))framerate = !framerate;
		}
	}
	ImGui::End();

	if (showImGuiDemoWindow)
		ImGui::ShowDemoWindow();
}