#include <AEX.h>
#include "Demos.h"
#include <iostream>
#include "Physics/RigidbodyComponent.h"

using namespace AEX;

// HELPER FUNCTIONS DECLARATIONS
void DrawSprite(AEVec3 pos, AEVec2 scale, f32 rot, Texture * tex);

// ----------------------------------------------------------------------------
// GAMESTATE FUNCTIONS
void DemoGameState::Initialize()
{
}
void DemoGameState::LoadResources()
{
	// Images
	aexGraphics->LoadTexture(".\\data\\Images\\Default.png");

	// Models
	aexGraphics->LoadModel(".\\data\\Models\\Quad.model");
}
void DemoGameState::Update()
{
	// get main window dimensions
	auto mainWin = aexWindow;
	auto winWH = AEVec2(f32(mainWin->GetWidth()), f32(mainWin->GetHeight()));

	// control the engine
	if (aexInput->KeyTriggered('B'))
		aexTime->LockFrameRate(!aexTime->FrameRateLocked());
	if (aexInput->KeyTriggered('V'))
		aexGraphics->SetVSyncEnabled(!aexGraphics->GetVSyncEnabled());
	if (aexInput->KeyPressed(Keys::Plus))
		aexTime->SetMaxFrameRate(aexTime->GetMaxFrameRate() + 1.0);
	if (aexInput->KeyPressed(Keys::Minus))
		aexTime->SetMaxFrameRate(aexTime->GetMaxFrameRate() - 1.0);
	if (aexInput->KeyTriggered('F'))
		aexWindow->SetFullscreen(!aexWindow->GetFullscreen());

	f32 fps = (f32)aexTime->GetFrameRate();
	std::string wintitle = "Simple Demo - FPS: "; wintitle += std::to_string(fps);
	if (aexTime->FrameRateLocked())	wintitle += "(LOCKED)";
	wintitle += " - VSYNC: ";	wintitle +=	aexGraphics->GetVSyncEnabled() ? "ON" : "OFF";
	wintitle += " - Controls: FPS: 'B', '+/-'. VSYNC: 'V'";
	aexWindow->SetTitle(wintitle.c_str());

}
void DemoGameState::Render()
{
	auto mainWin = aexWindow;
	auto winWH = AEVec2(f32(mainWin->GetWidth()), f32(mainWin->GetHeight()));

	aexGraphics->SetViewport(0, 0, s32(winWH.x), s32(winWH.y));
	aexGraphics->SetClearColor(Color(1,0,0,1));
	aexGraphics->ClearFrameBuffer();
}

// ----------------------------------------------------------------------------
// HELPER FUNCTIONS
void DrawSprite(AEVec3 pos, AEVec2 scale, f32 rot, Texture * tex)
{
	// sanity check -> only draw if we have a quad object created
	if (auto quad = aexGraphics->GetModel("Quad.model"))
	{
		auto mainWin = aexWindow;
		auto winWH = AEVec2(f32(mainWin->GetWidth()), f32(mainWin->GetHeight()));

		// compute the model to world matrix
		AEMtx44 model = AEMtx44::Translate(pos.x, pos.y, pos.z) * AEMtx44::RotateXYZ(0, 0, rot) * AEMtx44::Scale(scale.x, scale.y, 1.0f);

		// compute the world to cam matrix (camera is centered at (0,0, 20))
		AEMtx44 cam = AEMtx44::Translate(0, 0, -20);

		// compute the cam to screen matrix
		AEMtx44 proj = AEMtx44::OrthoProjGL(winWH.x, winWH.y, 0.01f, 1000.0f);

		// upload to shader
		auto shader = aexGraphics->GetShaderProgram("TextureMap.shader");
		if (shader)
		{
			shader->Bind();
			shader->SetShaderUniform("mtxViewProj", &(proj * cam));
			shader->SetShaderUniform("mtxModel", &model);
		}

		// set texture if it exists and set it in the shader
		if (tex) {
			tex->Bind();
			int texUnit = 0;
			shader->SetShaderUniform("ts_diffuse", &texUnit);
		}

		// draw the quad
		quad->Draw();

		// unbind everything
		if (shader)shader->Unbind();
		if (tex)tex->Unbind();
		
	}
}