#include <Graphics/AEXShader.h>
#include <Graphics/AEXTexture.h>
#include <Graphics/AEXTextureAtlas.h>
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
		if (mpTexture == nullptr && mpTexAtlas == nullptr)
			mpTexture = aexResources->GetResource<Texture>("Default.png");
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
				{
					mpTexAtlas = nullptr;

					auto sz = AEVec2(mpTexture->get()->GetWidth(), mpTexture->get()->GetHeight());
					transform->SetWorldScale(sz);
					return true;
				}
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
			
			if (ImGui::CollapsingHeader("Set Atlas"))	// Needs to check if the resource can be set
			{
				TResource<TextureAtlas>* temp = nullptr;
				
				if (aexEditor->getResource<TextureAtlas>(&mpTexAtlas))
				{
					mpTexture = nullptr;
				}	
				if (mpTexAtlas && mpTexAtlas->get())
				{
					if (ImGui::CollapsingHeader("Choose Region"))
					{
						auto regionNames = mpTexAtlas->get()->GetAllRegionNames();
						for (auto& rn : regionNames) {
							if (ImGui::Selectable(rn.c_str())) {
								mAtlasRegionName = rn;

								auto sz = mpTexAtlas->get()->GetRegionSize(rn);
								transform->SetWorldScale(sz);

								break;
							}
						}
					}
				}

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
		{
			tmp = j["textureatlas"];
			if (tmp != "null") mpTexAtlas = aexResources->GetResource<TextureAtlas>(tmp.c_str());
		}

		{
			tmp = j["atlasregion"];
			if (tmp != "null") mAtlasRegionName = tmp.c_str();
		}



		j["modulationColor"] >> mModulationColor;
	}

	void Renderable::StreamWrite(nlohmann::json& j) const
	{
		j["model"] = mpModel ? mpModel->GetName() : "null";
		j["texture"] = mpTexture ? mpTexture->GetName() : "null";
		j["shader"] = mpShader ? mpShader->GetName() : "null";
		j["modulationColor"] << mModulationColor;
		j["textureatlas"] = mpTexAtlas ? mpTexAtlas->GetName() : "null";
		j["atlasregion"] =  mAtlasRegionName;

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

			transform->mLocal.mTranslation.z = -transform->mLocal.mTranslation.y;
			// pass model to world matrix to the shader
			auto modelMtx = const_cast<Transform*>(&transform->mWorld)->GetMatrix44();
			mpShader->get()->SetShaderUniform("mtxModel", &modelMtx);
			mpShader->get()->SetShaderUniform("uModColor", &mModulationColor);

			AEMtx33 id;
			mpShader->get()->SetShaderUniform("mtxTexTransform", &id);

			int texture_unit = 0;
			mpShader->get()->SetShaderUniform("ts_diffuse", &texture_unit);

			// bind the texture if we have one
			if (mpTexture) {
				mpTexture->get()->Bind();
			}
			else if (mpTexAtlas)
			{
				auto texRes = mpTexAtlas->get()->GetTextureFromRegionName(mAtlasRegionName);
				if(texRes)
					texRes->get()->Bind();

				int texture_unit = 0;
				mpShader->get()->SetShaderUniform("ts_diffuse", &texture_unit);

				AEVec2 uvMin, uvMax;
				mpTexAtlas->get()->GetRegionMinMax(mAtlasRegionName, uvMin, uvMax);

				AEMtx33 texTransform = AEMtx33::Translate(uvMin.x, uvMin.y) * AEMtx33::Scale(uvMax.x - uvMin.x, uvMax.y - uvMin.y);
				mpShader->get()->SetShaderUniform("mtxTexTransform", &texTransform);
			}
			else
			{
				AEMtx33 id;
				mpShader->get()->SetShaderUniform("mtxTexTransform", &id);

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