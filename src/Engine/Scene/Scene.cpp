#include <Debug/AEXDebug.h>
#include <Platform/AEXSerialization.h>
#include "Scene.h"
#include <Composition/TransformComponent.h>
namespace AEX
{
	// IComp class with Space defined
	//Space* IComp::GetOwnerSpace() { return mOwner->mOwnerSpace;	}

	Space::Space()
	{
		// for a space, the owner is actually itself
		mOwnerSpace = this;
	}

	// update the space
	void Space::Update()
	{
		// clean up any deleted objects (delayed deletion)
		DeleteDeadObjects();

		// update the hierachy
		UpdateHierarchy();
	}

	// direct object creation
	GameObject* Space::NewObject(const char* name)
	{
		// set only name and a Transform component
		GameObject* newObj = aexFactory->Create<GameObject>();
		newObj->SetName(name);
		newObj->mOwnerSpace = this;
		AddChild(newObj);
		return newObj;
	}
	void Space::DeleteObject(GameObject* obj)
	{
		obj->mbDestroyed = true;

		// remove first to prevent duplicates
		mDeletedObjects.remove(obj);

		// add to the list of dead objects
		mDeletedObjects.push_back(obj);
	}

	GameObject* Space::FindObjectByName(const char* name)
	{
		const std::vector<GameObject*>& children = GetChildren();
		for (auto tr : children)
			if (!strcmp(tr->GetName(), name))
				return tr;

		return nullptr;
	}
	GameObject* Space::FindObjectByTag(unsigned tag)
	{
		// return first object found with tag tag
		const std::vector<GameObject*>& children = GetChildren();
		for (auto child : children)
			if (child->mTag == tag)
				return child;
	}
	std::list<GameObject*> Space::FindObjectsByTag(unsigned tag)
	{
		// return all objects found with tag tag
		std::list<GameObject*> taggedObjs;
		const std::vector<GameObject*>& children = GetChildren();
		for (auto child : children)
			if (child->mTag == tag)
				taggedObjs.push_back(child);

		return taggedObjs;
	}
	// Transform Management
	void Space::UpdateHierarchy()
	{
		// update the transforms
		for (auto tr : mRootTransforms)
			tr->Update();
	}
	void Space::AddTransform(TransformComp* tr)
	{
		if (tr->Parent() == NULL)
			mRootTransforms.push_back(tr);
	}
	void Space::RemoveTransform(TransformComp* tr)
	{
		mRootTransforms.remove(tr);
	}

	// delete dead objects
	void Space::DeleteDeadObjects()
	{
		// todo
		for (auto obj : mDeletedObjects) {

			// remove the object from the hierarchy
			if (obj->mParent)
				obj->mParent->RemoveChild(obj);

			// remove the components from their systems. 
			obj->Shutdown();

			delete obj;
		}
		mDeletedObjects.clear();
	}

	// Loading/Saving
	void Space::StreamRead(const nlohmann::json& j)
	{
		GameObject::StreamRead(j);
		// read custom Space data (if there is any)
	}
	void Space::StreamWrite(nlohmann::json& j) const
	{
		GameObject::StreamWrite(j);

		// write custom Space data (if there is any)
	}


	/* -------------- SCENE CLASS -------------- */

	// system state functions
	bool Scene::Initialize()
	{
		CreateMainSpace();
		return mSpaces[0] != nullptr;
	}
	void Scene::Update()
	{
		// for (auto s: mSpaces)
		for (std::vector<Space*>::iterator it = mSpaces.begin(); it != mSpaces.end(); ++it)
			(*it)->Update();
	}
	void Scene::Shutdown()
	{
		DestroyAllSpaces();
	}
	Space* Scene::GetMainSpace()
	{
		if (!mSpaces.empty())
			return mSpaces[0];

		return nullptr;
	}
	Space* Scene::GetSpace(const char* spaceName)
	{
		// for (auto s: mSpaces)
		for (std::vector<Space*>::iterator it = mSpaces.begin(); it != mSpaces.end(); ++it)
			if ((*it)->mName == spaceName)
				return *it;

		return nullptr;
	}
	Space* Scene::NewSpace(const char* spaceName)
	{
		// check that Space isn't duplicated (by name)
		Space* newSpace = GetSpace(spaceName);
		if (newSpace)
		{
			// return existing Space
			DebugPrint("Error: Space name already exists, returning existing Space.\n");
			return newSpace;
		}

		// create a new Space
		newSpace = aexFactory->Create<Space>();
		newSpace->SetName(spaceName);

		// add to container
		mSpaces.push_back(newSpace);

		return newSpace;
	}



	void Scene::DestroySpace(const char* spaceName)
	{
		DestroySpace(GetSpace(spaceName));
		/*FOR_EACH(it, mSpaces)
			if ((*it)->mName == spaceName)
			{
				delete(*it);
				mSpaces.erase(it);
				return;
			}*/
	}
	void Scene::Reset()
	{
		DestroyAllSpaces();
		CreateMainSpace();
	}

	std::vector<Space*> Scene::GetAllSpaces()
	{
		return mSpaces;
	}

	// Space's world transform update
	void Scene::UpdateAllSpaceHierarchies()
	{
		// for (auto s: mSpaces)
		for (std::vector<Space*>::iterator it = mSpaces.begin(); it != mSpaces.end(); ++it)
			(*it)->UpdateHierarchy();
	}

	void Scene::DeleteAllSpaceDeadObjects()
	{
		for (std::vector<Space*>::iterator it = mSpaces.begin(); it != mSpaces.end(); ++it)
			(*it)->DeleteDeadObjects();
	}

	// wrappers for object creation (always call the Space)
	GameObject* Scene::NewObject(const char* objName)
	{
		return mSpaces[0]->NewObject(objName);
	}
	GameObject* Scene::NewObject(const char* spaceName, const char* objName)
	{
		if (auto space = GetSpace(spaceName))
			return space->NewObject(spaceName);
		else {
			DebugErrorIf(space == nullptr, "Scene::NewObject(): Space %s doesn't exist", spaceName);
			return nullptr;
		}
	}

	GameObject* Scene::FindObjectByNameInSpace(Space* space, const char* name)
	{
		return space->FindObjectByName(name);
	}

	GameObject* Scene::FindObjectByNameInAnySpace(const char* name)
	{
		// for (auto s: mSpaces)
		for (std::vector<Space*>::iterator it = mSpaces.begin(); it != mSpaces.end(); ++it)
			if ((*it)->FindObjectByName(name) != nullptr)
				return (*it)->FindObjectByName(name);

		return nullptr;
	}

	// Loading/Saving
	void Scene::StreamRead(const nlohmann::json& j)
	{
		DestroyAllSpaces();

		mName = j["name"].get<std::string>();
		const nlohmann::json& spaceJson = j["spaces"];

		for (auto& tmp : spaceJson)
		{
			auto space = NewSpace("default");
			space->StreamRead(tmp);
			space->OnCreate();
		}

		//InitializeSpaces();
	}
	void Scene::StreamWrite(nlohmann::json& j) const
	{
		j["name"] = mName;
		nlohmann::json & spaceJson = j["spaces"];

		for (auto s : mSpaces)
		{
			nlohmann::json tmp;
			s->StreamWrite(tmp);
			spaceJson.push_back(tmp);
		}
	}
	void Scene::LoadFile(const char* fileName, bool initSpaces)
	{
		nlohmann::json j;
		ReadJsonFromFile(j, fileName);

		DestroyAllSpaces();
		StreamRead(j);

		if (initSpaces)
			InitializeSpaces();
	}
	void Scene::SaveFile(const char* fileName)
	{
		nlohmann::json j;
		StreamWrite(j);
		SaveJsonToFile(j, fileName);
	}

	// private members
	void Scene::DestroyAllSpaces()
	{
		// for (auto s: mSpaces)
		for (std::vector<Space*>::iterator it = mSpaces.begin(); it != mSpaces.end(); ++it)
		{
			(*it)->Shutdown();
			delete* it;
		}

			mSpaces.clear();
	}
	void Scene::DestroySpace(Space* space)
	{
		if (!space)
			return;

		FOR_EACH(it, mSpaces)
			if (*it == space)
			{
				// Cosa de Alex, no se si esta bien o no
				// Objects assigned to this space will become default
				auto objList = space->GetChildren();
				for (auto& obj : objList)
				{
					Space* OldSpace = obj->mOwnerSpace;
					OldSpace->RemoveChild(obj);		// Remove object from old space

					GetMainSpace()->AddChild(obj);	// Add object to the new space
				}

				delete(*it);
				mSpaces.erase(it);
				return;
			}
	}
	void Scene::InitializeSpaces()
	{
		// for (auto s: mSpaces)
		for (std::vector<Space*>::iterator it = mSpaces.begin(); it != mSpaces.end(); ++it)
		{
			(*it)->OnCreate();
		}
		for (std::vector<Space*>::iterator it = mSpaces.begin(); it != mSpaces.end(); ++it)
		{
			//(*it)->OnCreate();
			(*it)->Initialize();
		}
	}
	void Scene::CreateMainSpace()
	{
		if (!mSpaces.empty())
		{
			DebugPrint("AEXScene::CreateMainSpace: Main Space already exists!\n");
			return;
		}
		NewSpace("Main");
	}
}