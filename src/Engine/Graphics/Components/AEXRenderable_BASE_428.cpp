#include <Graphics/AEXShader.h>
#include <Graphics/AEXTexture.h>
#include <Graphics/AEXModel.h>
#include <Graphics/AEXGfxSystem.h>
#include <../Engine/Composition/TransformComponent.h>
#include <Composition/AEXComposition.h>
#include <Debug/AEXDebug.h>
#include "AEXGfxComps.h"
#include <Platform/AEXSerialization.h>
#include <ResourceMgr/ResourceMgr.h>
#include "imgui/imgui.h"
#include "Editor/AEXEditor.h"

namespace AEX {
	// IComp API


	void Renderable::OnCreate(){
		// assert we have a transform
		transform = GetOwner()->GetComp<TransformComp>();
		// get the transform.
		DebugAssert(transform != nullptr, "Renderable depends on TransformComp");
	}
	void Renderable::AddToSystem(){
		aexGraphics->AddRenderable(this);
	}
	void Renderable::RemoveFromSystem(){
		aexGraphics->RemoveRenderable(this);
	}
	void Renderable::Initialize(){

		if(mpTexture == nullptr)
			mpTexture = aexResources->GetResource<Texture>("Default.jpg");
		if (mpShader == nullptr)
			mpShader = aexResources->GetResource<ShaderProgram>("TextureMap.shader");
		if (mpModel == nullptr)
			mpModel = aexResources->GetResource<Model>("Quad.model");
	}
	void Renderable::Shutdown()
	{
		RemoveFromSystem();
	}
	bool Renderable::Edit()
	{
		if (ImGui::CollapsingHeader("Renderable"))
		{
			ImGui::ColorEdit4("Coor", &mModulationColor.x);
			if(ImGui::CollapsingHeader("Set texture"))	// Needs to check if the resource can be set
			{
				if (aexEditor->getResource<Texture>(&mpTexture))
					return true;
			}
			if (ImGui::CollapsingHeader("Set Shader"))	// Needs to check if the resource can be set
			{
				if (aexEditor->getResource<ShaderProgram>(&mpShader))
					return true;
			}
			if (ImGui::CollapsingHeader("Set Model"))	// Needs to check if the resource can be set
			{
				if (aexEditor->getResource<Model>(&mpModel))
					return true;
			}
		}
		return false;
	}

	void Renderable::StreamRead(const nlohmann::json& j)
	{
		std::string tmp;

		// load model
		{
			tmp = j["model"];
			if (tmp != "null")mpModel = aexResources->GetResource<Model>(tmp.c_str());
		}
		// load texture
		{
			tmp = j["texture"];
			if (tmp != "null")mpTexture = aexResources->GetResource<Texture>(tmp.c_str());
		}
		// load shader
		{
			tmp = j["shader"];
			if (tmp != "null")mpShader = aexResources->GetResource<ShaderProgram>(tmp.c_str());
		}

		j["modulationColor"] >> mModulationColor;
	}

	void Renderable::StreamWrite(nlohmann::json& j) const
	{
		j["model"] = mpModel ? mpModel->GetName() : "null";
		j["texture"] = mpTexture ? mpTexture->GetName() : "null";
		j["shader"] = mpShader ? mpShader->GetName() : "null";
		j["modulationColor"] << mModulationColor;
	}

	/* RENDERABLE BASE API (OVERLOAD THOSE).
	*  Each step of the Renderable Render function is customizable, Render is implemented as:
	*  void Render(){
	*		BindResources(true){}
	*		PreDraw(){}	// TODO: change this to an event
	*		DrawMesh(){}
	*		PostDraw(){}	// TODO: change this to an event
	*		BindResources(false){}
	*/
	void Renderable::Render(){
		BindResources(true);
		PreDraw();	
		DrawMesh();
		PostDraw(); 
		BindResources(false);
	}
	void Renderable::BindResources(bool bind) {

		// sanity check-> we have the minimum resources to draw:
		if (!ValidateResources()) return;

		// bind step
		if (bind) {

			// bind model and shaders
			mpModel->get()->Bind();
			mpShader->get()->Bind();

			/*if (transform->mWorld.mTranslation.y < 0)
				transform->mWorld.mTranslation.z = abs(transform->mWorld.mTranslation.y);
			else
				transform->mWorld.mTranslation.z = -1;*/

			transform->mLocal.mTranslation.z = -transform->mLocal.mTranslation.y;
			// pass model to world matrix to the shader
			auto modelMtx = const_cast<Transform*>(&transform->mWorld)->GetMatrix44();
			mpShader->get()->SetShaderUniform("mtxModel", &modelMtx);
			mpShader->get()->SetShaderUniform("uModColor", &mModulationColor);

			// bind the texture if we have one
			if (mpTexture) {
				int texture_unit = 0;
				mpTexture->get()->Bind();
				mpShader->get()->SetShaderUniform("ts_diffuse", &texture_unit);
			}
		}
		// unbind step
		else {
			mpModel->get()->Unbind();
			mpShader->get()->Unbind();
			if (mpTexture)mpTexture->get()->Unbind();
		}
	}
	void Renderable::DrawMesh(){
		if(mpModel && mpModel->get())
		// call draw on the model.
		mpModel->get()->Draw();
	}
	void Renderable::PreDraw(){
		// nothing for now. overrid this for custom render. 
	}
	void Renderable::PostDraw(){
		// nothing for now. override this for custom render.
	}

	void Renderable::SetModColor(const Color& c)
	{
		mModulationColor = c;
	}

	// AABB compute, it uses the AABB in model space. 
	// it can be transformed by an optional parameter
	AEVec4 Renderable::ComputeAABB(AEMtx33* mtx){
		return AEVec4();
	}
}