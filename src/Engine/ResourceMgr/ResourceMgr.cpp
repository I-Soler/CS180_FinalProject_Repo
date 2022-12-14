#include <AEX.h>
#include "ResourceMgr.h"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <sstream>
#include <Extern/imgui/imgui.h>

namespace AEX
{
	bool ResourceManager::Initialize()
	{
		// register known importers
		mImporters[".png"] = new TextureImporter;
		mImporters[".jpg"] = new TextureImporter;
		mImporters[".vert"] = new ShaderImporter;
		mImporters[".frag"] = new ShaderImporter;
		mImporters[".shader"] = new ShaderProgramImporter;
		mImporters[".model"] = new ModelImporter;
		mImporters[".mp3"] = new SoundImporter;
		mImporters[".arch"] = new ArchetypeImporter;
		mImporters[".atlas"] = new AtlasImporter;

		// return success
		return true;
	}

	//free specific type of resource
	void ResourceManager::FreeResource(IResource* res)
	{
		auto it = mAllResources["Texture"].find(res->GetName());

		delete it->second;
		mAllResources["Texture"].erase(it);
	}

	//free specific file (shader / texture) by name
	void ResourceManager::FreeResource(const std::string& name)
	{
		FilePath fp(name.c_str());
		std::string a;

		if (fp.mExtension == ".png" || fp.mExtension == ".jpg")
		{
			a = "Texture";
		}

		else if (fp.mExtension == ".model")
		{
			a = "Model";
		}

		else if (fp.mExtension == ".frag" || fp.mExtension == ".vert")
		{
			a = "Shader";
		}

		else if (fp.mExtension == ".shader")
		{
			a = "ShaderProgram";
		}
		else if (fp.mExtension == ".mp3")
		{
			a = "Sound";
		}
		else if (fp.mExtension == ".atlas")
		{
			a = "TextureAtlas";
		}

		auto it = mAllResources[a].find(name);

		if (it != mAllResources[a].end())
		{
			delete it->second;
			mAllResources[a].erase(it);
		}

		std::cout << a << " " << "not found to delete" << std::endl;
	}

	void ResourceManager::FreeAllResources()
	{
		// free all resources
		for (auto it = mAllResources.begin(); it != mAllResources.end(); it++)
		{
			for (auto it2 = it->second.begin(); it2 != it->second.end(); it2++)
			{
				delete it2->second;
			}
		}

		//clear the map
		mAllResources.clear();
	}

	void ResourceManager::Shutdown() {


		// free all resources
		FreeAllResources();
		// free all importers
		for (auto imp : mImporters)
			delete imp.second;
		mImporters.clear();
	}

	void ResourceManager::Update() {

		// check for last modified times. 
	}


	// load folder /// loads data folder (usually)
	void ResourceManager::LoadFolder(const char* folderPath, bool softLoad, bool forceReload)
	{
		std::vector<std::thread> STDthread_IDs;	/* array of ID of each thread    */
		bool multithreaded = false;

		const std::filesystem::path data{ folderPath };

		// directory_iterator can be iterated using a range-for loop
		for (auto const& dir_entry : std::filesystem::directory_iterator{ data })
		{
			if (dir_entry.is_directory())
			{
				std::string po = dir_entry.path().u8string();

				if (po.c_str() == "Scenes")
				{
					continue;
				}

				LoadFolder(po.c_str());
				//std::cout << dir_entry.path() << '\n';
			}
			else
			{
				std::string pe = dir_entry.path().u8string();

				if (dir_entry.path().extension() == ".json")
				{
					continue;
				}

				LoadResource(pe.c_str(), softLoad, forceReload);

				//std::cout << dir_entry.path() << '\n';
			}
		}
	}

	void ResourceManager::RegisterImporter(const char* extension, IResourceImporter* importer) {

		// add the importer to the map but avoid duplicates
		auto importerIt = mImporters.find(extension);

		// importer exists -> free the current one
		if (importerIt != mImporters.end()) {
			delete importerIt->second;
		}

		// add the importer
		mImporters[extension] = importer;
	}

	// resource loading
	IResource* ResourceManager::LoadResource(const char* filename, bool softLoad, bool forceReload)
	{
		// get the filepaht

		FilePath fp(filename);

		// get the importer corresponding to the extension
		auto importerIt = mImporters.find(fp.mExtension);

		if (importerIt == mImporters.end()) {
			DebugPrint("ResourceManager Loading Error: %s . Extension not recognized", (fp.mFilename + fp.mExtension).c_str());
			return nullptr;
		}

		// importer is known get pointer to it and load the resource
		auto importer = importerIt->second;


		// now that we know the importer, we have a way to look for it by its type
		auto resTypename = importer->GetResourceTypeName();
		auto& resMap = mAllResources[resTypename];

		// see if the resource exists (find by name)
		auto resName = fp.mFilename + fp.mExtension;
		auto resIt = resMap.find(resName);

		IResource* resource = nullptr;

		// resource exists -> replace the raw resource
		// with the new loaded resource
		if (resIt != resMap.end())
		{
			// set the resource to return
			resource = resIt->second;

			if (forceReload)
			{
				IResource* newRes = importer->ImportFromFile(filename, softLoad);

				// set the new resource using the newly created resource
				resource->SetRawResource(newRes->GetRawResource(), true);

				// delete the resource loaded by the importer
				// but first make sure that we don't delete it by setting thre raw 
				// resource to null
				newRes->SetRawResource(nullptr, false);
				delete newRes;
			}
		}

		// resource doesn't exists yet, simply register it on our map
		else
		{
			// store the resource so we can return it after.
			resource = importer->ImportFromFile(filename, softLoad);

			// register it
			resMap[resName] = resource;
		}

		// set the resource metada
		resource->SetName(resName.c_str());
		resource->mFilepath = fp;
		resource->mLastWriteTime = fp.GetLastWriteTime();

		// TODO:
		// broadcast a resource loaded event. 

		// return the resource
		return resource;
	}

	const std::map <std::string, std::map<std::string, IResource*>>& ResourceManager::GetAllResources()
	{
		return mAllResources;
	}

	//check for mouse input and storing all files dragged and dropped into a list of strings
	void ResourceManager::DragDrop(std::list<std::string> files, bool softLoad)
	{
		// sanity check
		if (!files.empty())
		{
			if (mouse)
			{
				drag = true;
			}
			else
			{
				drag = false;
			}

			dragdropfiles = files;
		}
		return;
	}

}


