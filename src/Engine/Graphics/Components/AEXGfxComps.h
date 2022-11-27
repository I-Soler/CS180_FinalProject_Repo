#pragma once
#include <Composition/AEXComponent.h>
#include <aexmath/AEXVec2.h>
#include <aexmath/AEXVec3.h>
#include <aexmath/AEXVec4.h>
#include <aexmath/AEXMtx33.h>
#include <aexmath/AEXMtx44.h>
#include <Graphics/AEXColor.h>

namespace AEX {

	template <typename T> class TResource;
	class TransformComp;
	class Camera : public IComp {
		AEX_RTTI_DECL(Camera, IComp);
	public: 

		// size of the the view volume (width, height) for the orthographic projection
		//How much of the space can we see (zoom in, zoom out)
		AEVec2 mViewRectangle;

		// Viewport Data
		//Area of the framebuffer in which we are going to draw 
		AEVec4 mViewport;				// viewport dimensions: to use with glViewport glScissor when drawing the camera
										// x = left, y = bottom, z = right, w = top
		bool mbClearViewport = true;	// should clear the viewport or not?
		Color mViewportColor;			// if so, use this color to clear the viewport. 

		// Draw Order: Graphics System draws in ascending order 
		int DrawOrder = 0;	

	protected:
		// transform pointer
		TransformComp* transform = nullptr;
	public:
		Camera();
		void AddToSystem()override;
		void RemoveFromSystem() override;
		void OnCreate()override;
		void Shutdown()override;
		bool Edit()override;

		// get view matrix
		AEMtx33 GetViewMtx33();
		AEMtx44 GetViewMtx44();
		AEMtx33 GetInvViewMtx33();
		AEMtx44 GetInvViewMtx44();

		// get projection matrix
		AEMtx44 GetProjectionMtx44();
		AEMtx44 GetInvProjectionMtx44();

		// get model view projection matrix;
		AEMtx44 GetViewProjMtx44();
		AEMtx44 GetInvViewProjMtx44();

		void ViewToWorld(AEVec2& MousePos);

		void StreamRead(const nlohmann::json& j) override;
		void StreamWrite(nlohmann::json& j) const override;

	};

	class Model;
	class Texture;
	class ShaderProgram;
	class Renderable : public IComp {
		AEX_RTTI_DECL(Renderable, IComp);
		TResource<Model>* mpModel = nullptr;				// required: model containing the indexed vertex data (see class Vertex).
		TResource<ShaderProgram>* mpShader = nullptr;		// required: shader used to draw the model. 
		TransformComp* transform = nullptr;		// required: transform to compute the model-to-world matrix (identity if null). 
		TResource<Texture>* mpTexture = nullptr;			// (optional): texture image to apply to the basic shader. (if null, uses only modulation color). 
		Color mModulationColor					// modulation color. 
			= Color(1, 1, 1, 1);

		// IComp API
		void OnCreate()override;
		void Initialize()override;
		void AddToSystem()override;
		void RemoveFromSystem()override;
		void Shutdown()override;
		bool Edit()override;
		void StreamRead(const nlohmann::json& j) override;
		void StreamWrite(nlohmann::json& j) const override;
		
		/* RENDERABLE BASE API (OVERLOAD THOSE). 
		*  Each step of the Renderable Render function is customizable, Render is implemented as:
		*  void Render(){
		*		BindResources(true);
		*		PreDraw();	// TODO: change this to an event
		*		DrawMesh();
		*		PostDraw();	// TODO: change this to an event
		*		BindResources(false);
		*/
		virtual void Render(); 
		virtual void BindResources(bool bind);
		virtual void DrawMesh();
		virtual void PreDraw();	 
		virtual void PostDraw(); 

		// AABB compute, it uses the AABB in model space. 
		// it can be transformed by an optional parameter
		AEVec4 mModelAABB; // Cached AABB in model space. 
		AEVec4 ComputeAABB(AEMtx33 * mtx = nullptr);

		bool ValidateResources() { return mpModel && mpShader; }

	public:
		void SetModColor(const Color&);
		void SetTexture(TResource<Texture>*);
	};
}