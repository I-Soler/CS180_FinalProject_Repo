#pragma once
//#include <AEX.h>
//#include "Demos/Demos.h"
#include <Core/AEXCore.h>
#include <Platform/AEXFilePath.h>
#include <iostream>
#include <string>
#include <aexmath/AEXMath.h>

namespace AEX
{

	class IResource : public IBase
	{
		friend class ResourceManager;
		AEX_RTTI_DECL(IResource, IBase);

	public:
		virtual void SetRawResource(IBase* rawRes, bool freeCurrent) = 0;
		virtual IBase* GetRawResource() = 0;

	public:
		virtual ~IResource() {}
		virtual const char* GetResourceTypeName() = 0;
		const FilePath& GetFilePath() { return mFilepath; }
		s64 GetLastWriteTime() { return mLastWriteTime; }

	protected:
		FilePath	mFilepath;
		s64			mLastWriteTime;
	};

	template <typename RES_TYPE>
	class TResource : public IResource
	{
		friend class ResourceManager;
		AEX_RTTI_DECL(TResource<RES_TYPE>, IResource);
	private:
		RES_TYPE* mRawResource = nullptr;

	public:
		void SetRawResource(IBase* rawRes, bool freeCurrent)override
		{
			if (freeCurrent && mRawResource)
				delete mRawResource;

			mRawResource = (RES_TYPE*)rawRes;
		}
		RES_TYPE* GetRawResource() override
		{
			return dynamic_cast<RES_TYPE*>(mRawResource);
		}
	public:
		~TResource() override
		{
			if (mRawResource)
				delete mRawResource;
		}

		const char* GetResourceTypeName()override
		{
			return RES_TYPE::TYPE().GetName().c_str();
		}

		RES_TYPE* get() {
			return mRawResource;
		}
	};

	class IResourceImporter : public IBase
	{
		AEX_RTTI_DECL(IResourceImporter, IBase);

	public:

		IResourceImporter() {}
		virtual ~IResourceImporter() {}
		virtual IResource* ImportFromFile(const char* filename, bool softLoad = false) = 0;
		virtual const char* GetResourceTypeName() = 0;
	};

	class TextureImporter : public IResourceImporter
	{
		AEX_RTTI_DECL(TextureImporter, IResourceImporter);
		IResource* ImportFromFile(const char* filename, bool softLoad = false) override;
		const char* GetResourceTypeName() override;
	};

	class ShaderImporter : public IResourceImporter
	{
		AEX_RTTI_DECL(ShaderImporter, IResourceImporter);
		IResource* ImportFromFile(const char* filename, bool softLoad = false) override;
		const char* GetResourceTypeName() override;
	};

	class ShaderProgramImporter : public IResourceImporter // MAYBE WRONG UNTESTED !!!!!!
	{
		AEX_RTTI_DECL(ShaderProgramImporter, IResourceImporter);
		IResource* ImportFromFile(const char* filename, bool softLoad = false) override;
		const char* GetResourceTypeName() override;
	};

	class ModelImporter : public IResourceImporter // MAYBE WRONG UNTESTED !!!!!!
	{
		AEX_RTTI_DECL(ModelImporter, IResourceImporter);
		IResource* ImportFromFile(const char* filename, bool softLoad = false) override;
		const char* GetResourceTypeName() override;
	};

	class SoundImporter : public IResourceImporter
	{
		AEX_RTTI_DECL(SoundImporter, IResourceImporter);
		IResource* ImportFromFile(const char* filename, bool softLoad = false) override;
		const char* GetResourceTypeName() override;
	};

	class Archetype;	// declared below
	class ArchetypeImporter : public IResourceImporter
	{
		AEX_RTTI_DECL(Archetype, IResourceImporter);
		IResource* ImportFromFile(const char* filename, bool softLoad = false) override;
		const char* GetResourceTypeName() override;
	};


	class ResourceManager : public IBase
	{
		AEX_RTTI_DECL(ResourceManager, IBase);
		AEX_SINGLETON(ResourceManager) {}
	private:

		// map of all resources stored as
		// mAllResources["ResourceTypeName"]["ResourceName"] = IResource*
		std::map < std::string, std::map<std::string, IResource*>> mAllResources;

		// map of all resources, stored as
		// mAllResources["file extension"] = IResourceImporter *
		std::map < std::string, IResourceImporter*> mImporters;

	public:

		bool hovered = false;
		AEVec2 wPos;
		AEVec2 wScale;
		bool mouse = false;
		bool drag = false;
		std::list<std::string> dragdropfiles;

		bool Initialize();

		//free specific type of resource
		void ResourceManager::FreeResource(IResource* res);

		//free specific file (shader / texture) by name
		void ResourceManager::FreeResource(const std::string& name);

		void ResourceManager::FreeAllResources();

		void Shutdown();

		void Update();

		// load folder /// loads data folder (usually)
		void LoadFolder(const char* folderPath, bool softload = false, bool forceReload = false);

		void RegisterImporter(const char* extension, IResourceImporter* importer);

		template <typename IMPORTER_TYPE>
		void RegisterImporter(const char* extension);

		// resource loading
		IResource* LoadResource(const char* filename, bool softLoad, bool forceReload = false);

		//void Save(Scene*);

		// this function is here for convenience
		// when loading a resource from code (when we usually know the type of 
		// the resource we want to load
		template<typename RES_TYPE>
		TResource<RES_TYPE>* LoadResourceType(const char* filename, bool softLoad = false, bool forceReload = false);

		// getting a resource happens always from code where we know the resouce type
		// we want. So we use only a template. 
		template <typename RES_TYPE>
		TResource<RES_TYPE>* GetResource(const char* resName, bool loadIfNull = true);

		const std::map < std::string, std::map<std::string, IResource*>>& GetAllResources();

		//check for mouse input and storing all files dragged and dropped into a list of strings
		void DragDrop(std::list<std::string> files, bool softLoad = true);

		//ImGui window to show the files in data
		void ShowFiles(const char* folderPath);

		//loads the files and if a new one is dropped also loads it
		void LoadFiles(const char* folderPath);
	};


	template <typename IMPORTER_TYPE>
	void ResourceManager::RegisterImporter(const char* extension)
	{
		RegisterImporter(extension, new IMPORTER_TYPE);
	}

	// this function is here for convenience
	// when loading a resource from code (when we usually know the type of 
	// the resource we want to load
	template<typename RES_TYPE>
	TResource<RES_TYPE>* ResourceManager::LoadResourceType(const char* filename, bool softLoad, bool forceReload)
	{
		return dynamic_cast<TResource<RES_TYPE>*>(LoadResource(filename, softLoad, forceReload));
	}

	// getting a resource happens always from code where we know the resouce type
	// we want. So we use only a template. 
	template <typename RES_TYPE>
	TResource<RES_TYPE>* ResourceManager::GetResource(const char* resName, bool loadIfNull) {

		// get the type in the map using RTTI
		auto resTypename = RES_TYPE::TYPE().GetName();

		// see if the the resource exists
		auto& resMap = mAllResources[resTypename];
		auto resIt = resMap.find(resName);

		// resource to return
		TResource<RES_TYPE>* resource = nullptr;

		// resource exists
		if (resIt != resMap.end())
		{
			resource = dynamic_cast<TResource<RES_TYPE>*>(resIt->second);

			if ((resource->GetRawResource() == nullptr && loadIfNull == true) && FilePath::Exists(resource->mFilepath.mFullPath.c_str()))
			{
				LoadResource(resource->mFilepath.mFullPath.c_str(), false, true);
			}

		}
		// resource doesn't exist -> create it
		// and register it.
		else
		{
			// new resource but the RAW resource is null
			resource = new TResource<RES_TYPE>;

			// set its name
			resource->SetName(resName);

			resMap[resName] = resource;
		}

		// return the resource
		return resource;
	}

	/* prefabs interface */
	class Archetype : public IBase
	{
		AEX_RTTI_DECL(Archetype, IBase);
	public:
		nlohmann::json prefab_json;
	};
}
#define aexResources (AEX::Singletons::Get<AEX::ResourceManager>())
