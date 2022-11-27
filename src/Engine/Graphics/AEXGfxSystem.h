/// ---------------------------------------------------------------------------
/// \file			AEXGfxSystem.h
/// \author			Thomas Komair
/// \brief			Declare the interface for the AEX graphics engine
/// ---------------------------------------------------------------------------
#ifndef AEX_GRAPHICS_SYSTEM_H_
#define AEX_GRAPHICS_SYSTEM_H_

// ---------------------------------------------------------------------------
// INCLUDES & DEPENDENCIES
#include "..\Core\AEXCore.h"
#include <aexmath\AEXMathDefs.h>
#include <aexmath\AEXVec2.h>	
#include <aexmath\AEXVec3.h>	
#include "AEXColor.h"		

namespace AEX
{
	// Forward Declarations of Graphics related classes
	class Renderable;
	class Camera;
	class Texture;
	class Shader;
	class ShaderProgram;
	class Model;
	struct AEMtx44;

	// ---------------------------------------------------------------------------
	// CLASS: Graphics: Main system for the graphics engine
	class  Graphics : public IBase
	{
		AEX_RTTI_DECL(Graphics, IBase);
		AEX_SINGLETON(Graphics);
	public:

		// ISystem Inherited
		virtual bool Initialize();
		virtual ~Graphics();

		void SetViewport(s32 left, s32 bottom, s32 right, s32 top);
		void SetClearColor(const Color & col);
		void ClearFrameBuffer();
		void Render(); // main render step. called from main engine
		void Present();

		// Rendererer porpoerties
		bool GetDepthTestEnabled();
		void SetDepthTestEnabled(bool enabled);
		bool GetVSyncEnabled();
		void SetVSyncEnabled(bool enabled);

		// Debug Draw
		void DrawLine(AEVec2 p0, AEVec2 p1, Color col = AEX::Color());
		void DrawLine(float x0, float y0, float x1, float y1, AEX::Color col = AEX::Color());
		void DrawRect(float x, float y, float w, float h, Color col = AEX::Color());
		void DrawOrientedRect(float x, float y, float w, float h, float angle, Color col = AEX::Color());
		void DrawCircle(f32 cX, f32 cY, f32 radius, Color col = AEX::Color(), f32 angle_start = 0, f32 angle_end = TWO_PI);
		void RenderLineData(bool clear = true, AEMtx44* matViewProj = nullptr);

		// Component Management
		void AddCamera(Camera* cam);
		void RemoveCamera(Camera* cam);
		void SortCameras();
		Camera* GetCurrentCamera();
		void AddRenderable(Renderable* render);
		void RemoveRenderable(Renderable* render);
		void ClearRenderables();
		void ClearCameras();
		void ClearAllComponents();



		#pragma region// Basic Resource Management
		//
		// TODO: Move this to resource management system.
		//
		Model *			LoadModel(const char * modelFile);
		Texture *		LoadTexture(const char * textureFile);
		Shader *		LoadShader(const char * shaderFile);
		ShaderProgram * LoadShaderProgram(const char * shaderFile, Shader* pix, Shader * vert);

		#pragma endregion

	private:

		Camera* mCurrentCamera = nullptr;
		std::list<Camera*> mCameras;
		std::list<Renderable*> mRenderables;

		bool InitializeOpenGL();
		void TerminateOpenGL();

		bool						mbVSyncEnabled = true;
	};
}
// Easy access to singleton
#define aexGraphics AEX::Singletons::Get<AEX::Graphics>()

// ---------------------------------------------------------------------------
#endif