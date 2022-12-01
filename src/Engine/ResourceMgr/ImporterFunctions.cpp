#include <AEX.h>
#include "ResourceMgr.h"
#include "Engine/Platform/AEXSerialization.h"
#include "json/json.hpp"
#include <Audio/Audio.h>
#include <fstream>
#include "Engine/Graphics/AEXTextureAtlas.h"
#include "Animation/SpineComps.h"
#include "spine/TextureLoader.h"
#include "spine/Atlas.h"

#include "Animation/SpineComps.h"
#include "spine/TextureLoader.h"
#include "spine/Atlas.h"
namespace AEX
{
	//texture importer -------------------------------------------------------------------------

	IResource* TextureImporter::ImportFromFile(const char* filename, bool softLoad)
	{
		// create the resource
		TResource<Texture>* newRes = aexFactory->Create<TResource<Texture>>();

		if (!softLoad)
		{
			// create the texture
			Texture* newTexture = aexFactory->Create<Texture>();

			if (newTexture)
			{
				newTexture->LoadFromFile(filename);

				newRes->SetRawResource(newTexture, false);
			}
		}

		//return it
		return newRes;

	}
	const char* TextureImporter::GetResourceTypeName()
	{
		return Texture::TYPE().GetName().c_str();
	}




	//shader importer --------------------------------------------------------------------------

	IResource* ShaderImporter::ImportFromFile(const char* filename, bool softLoad)
	{
		// create the resource
		TResource<Shader>* newRes = aexFactory->Create<TResource<Shader>>();

		if (!softLoad)
		{
			// create the shader
			Shader* newShader = aexFactory->Create<Shader>();

			// load the new shader from file. 
			if (newShader)
			{
				FilePath fp(filename);

				//                                                                         if else
				Shader::EType shaderType = (fp.mExtension == ".frag") ? Shader::EType::ePixel : Shader::EType::eVertex;

				newShader->SetShaderType(shaderType, true);
				newShader->Load(filename);
				newRes->SetRawResource(newShader, false);
			}
		}

		// return it
		return newRes;
	}
	const char* ShaderImporter::GetResourceTypeName()
	{
		return Shader::TYPE().GetName().c_str();
	}




	//shader program importer ------------------------------------------------------------------

	IResource* ShaderProgramImporter::ImportFromFile(const char* filename, bool softLoad)
	{

		// create the resource
		TResource<ShaderProgram>* newRes = aexFactory->Create<TResource<ShaderProgram>>();

		if (!softLoad)
		{
			// create the shader
			ShaderProgram* newShaderProgram = aexFactory->Create<ShaderProgram>();

			// load the new shader from file. 
			if (newShaderProgram)
			{
				FilePath fp(filename);

				// use json to read the file and get the vertex shader name and the pixel shader name
				nlohmann::json file;

				ReadJsonFromFile(file, filename);

				std::string pixel = file["PixelShader"];
				std::string vertex = file["VertexShader"];

				auto pix = aexResources->LoadResourceType<Shader>(pixel.c_str());
				auto vert = aexResources->LoadResourceType<Shader>(vertex.c_str());

				newShaderProgram->SetShader(pix->get(), false);
				newShaderProgram->SetShader(vert->get(), true);

				// set to the resource
				newRes->SetRawResource(newShaderProgram, false);
			}
		}

		// return it
		return newRes;
	}
	const char* ShaderProgramImporter::GetResourceTypeName()
	{
		return ShaderProgram::TYPE().GetName().c_str();
	}




	//model importer --------------------------------------------------------------------------

	IResource* ModelImporter::ImportFromFile(const char* filename, bool softLoad)
	{
		// create the resource
		TResource<Model>* newRes = aexFactory->Create<TResource<Model>>();

		if (!softLoad)
		{
			// create the model
			Model* newModel = aexFactory->Create<Model>();

			// Create default model
			if (newModel)
			{
				// by default, we're going to make a quad
				// Create a quad by default
				f32 h = 0.5f;

				newModel->AddVertex(AEX::Vertex(AEX::AEVec2(-h, 1.0f), AEX::AEVec2(0, 1), AEX::Color(1, 0, 0)));
				newModel->AddVertex(AEX::Vertex(AEX::AEVec2(-h, 0.0f), AEX::AEVec2(0, 0), AEX::Color(1, 1, 0)));
				newModel->AddVertex(AEX::Vertex(AEX::AEVec2(h, 0.0f), AEX::AEVec2(1, 0), AEX::Color(0, 1, 0)));

				newModel->AddVertex(AEX::Vertex(AEX::AEVec2(-h, 1.0f), AEX::AEVec2(0, 1), AEX::Color(1, 0, 0)));
				newModel->AddVertex(AEX::Vertex(AEX::AEVec2(h, 0.0f), AEX::AEVec2(1, 0), AEX::Color(0, 1, 0)));
				newModel->AddVertex(AEX::Vertex(AEX::AEVec2(h, 1.0f), AEX::AEVec2(1, 1), AEX::Color(0, 0, 1)));
				newModel->UploadToGPU();

				newRes->SetRawResource(newModel, false);
			}
		}

		// return it
		return newRes;
	}
	const char* ModelImporter::GetResourceTypeName()
	{
		return Model::TYPE().GetName().c_str();
	}

	//sound importer --------------------------------------------------------------------------
	IResource* SoundImporter::ImportFromFile(const char* filename, bool softLoad)
	{
		TResource<Sound>* newSoundRes = new TResource<Sound>;
		if (!softLoad)
		{
			auto sound = aexAudio->CreateSound(filename);
			newSoundRes->SetRawResource(sound, false);
		}
		return newSoundRes;
	}
	const char* SoundImporter::GetResourceTypeName()
	{
		return Sound::TYPE().GetName().c_str();
	}


	//atlas importer --------------------------------------------------------------------------
	IResource* AtlasImporter::ImportFromFile(const char* filename, bool softLoad)
	{
		TResource<TextureAtlas>* newRes = new TResource<TextureAtlas>;
		if (!softLoad)
		{
			TextureAtlas* TexAtlas = aexFactory->Create<TextureAtlas>();

			if (TexAtlas)
			{
				TexAtlas->mInternalAtlas = new spine::Atlas(spine::String(filename), &spine::sTextureLoader, true);
				newRes->SetRawResource(TexAtlas, false);
			}
		}

		return newRes;
	}

	const char* AtlasImporter::GetResourceTypeName()
	{
		return "TextureAtlas";
	}

   	IResource* ArchetypeImporter::ImportFromFile(const char* filename, bool softLoad)
	{
		TResource<Archetype>* newArchRes = new TResource<Archetype>;
		if (!softLoad)
		{
			Archetype* prefab = aexFactory->Create<Archetype>();
			ReadJsonFromFile(prefab->prefab_json, filename);
			newArchRes->SetRawResource(prefab, false);
		}
		return newArchRes;
	}
	const char* ArchetypeImporter::GetResourceTypeName()
	{
		return Archetype::TYPE().GetName().c_str();
	}
}