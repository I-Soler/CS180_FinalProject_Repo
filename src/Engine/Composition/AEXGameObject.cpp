#include <Core/AEXCore.h>
#include <Composition/AEXComposition.h>
#include <Debug/AEXDebug.h>

namespace AEX
{
	GameObject::GameObject() {}
	GameObject::~GameObject() {
		// Shutdown();

		// todo: delete all components
		for (auto& comp : mComponents)
			delete comp;

		// todo: delete all components
		for (auto& obj : mChildren)
			delete obj;
	}

	// system state functions
	void GameObject::OnCreate() {
		for (auto& comp : mComponents) {
			comp->OnCreate();
			comp->AddToSystem();
		}

		for (auto& child : mChildren)
			child->OnCreate();
	}
	void GameObject::Initialize() {
		for (auto& comp : mComponents)
			comp->Initialize();

		for (auto& child : mChildren)
			child->Initialize();
	}
	void GameObject::Shutdown() {
		for (auto& comp : mComponents) {
			comp->Shutdown();
		}

		for (auto& child : mChildren)
			child->Shutdown();
	}

	// inherited from IBase
	bool GameObject::SetEnabled(bool enabled) {
		mEnabled = enabled;

		for (auto& comp : mComponents)
			comp->SetEnabled(enabled);

		for (auto& child : mChildren)
			child->SetEnabled(enabled);

		return mEnabled;
	}

	// Hierarchy management
	void GameObject::AddChild(GameObject* obj)
	{
		// never add an object twice
		for (std::vector<GameObject*>::iterator it = mChildren.begin(); it != mChildren.end(); ++it)
			if (*it == obj)
				return;

		// if the object already has a parent, remove that object from it
		if (obj->mParent)
			obj->mParent->RemoveChild(obj);

		// set the parent to this
		obj->mParent = this;
		obj->mOwnerSpace = mOwnerSpace;

		// add the object to our list
		mChildren.push_back(obj);
	}
	void GameObject::RemoveChild(GameObject* obj)
	{
		if (obj->mParent == this)
			// FOR_EACH(it, mChildren)
			for (std::vector<GameObject*>::iterator it = mChildren.begin(); it != mChildren.end(); ++it)
				if (*it == obj)
				{
					mChildren.erase(it);
					break;
				}

		obj->mParent = nullptr;
		//obj->mOwnerSpace = nullptr;
	}
	void GameObject::SetParent(GameObject* parent)
	{
		if (parent)
			parent->AddChild(this);
	}

	// Loading/Saving
	void GameObject::StreamRead(const nlohmann::json& j)
	{
		// read name and ID
		mUID = j["uid"];
		mName = j["name"].get<std::string>();
		mTag = j["tag"];

		// read the components
		const nlohmann::json& comps = j["components"];
		for (auto& compJson : comps) {

			// read the type
			std::string compType = compJson["_type"].get<std::string>();

			// ask the factory to build the components
			if (auto comp = aexFactory->Create(compType.c_str()))
			{
				AddComp((IComp*)comp);
				comp->StreamRead(compJson);
			}
			else
				DebugPrint("Unknown Component %s", compType.c_str());
		}

		// TODO
		// read the children 
		const nlohmann::json& children = j["children"];
		for (auto& childJson : children) {

			GameObject* child = aexFactory->Create<GameObject>();
			child->StreamRead(childJson);
			AddChild(child);
			//child->OnCreate();
		}
	}
	void GameObject::StreamWrite(nlohmann::json& j) const
	{
		// save name
		j["name"] = mName;
		j["uid"] = mUID;
		j["tag"] = mTag;

		// save components (as an array)
		nlohmann::json& comps = j["components"];
		for (u32 i = 0; i < mComponents.size(); ++i)
		{
			IComp* comp = mComponents[i];

			nlohmann::json compJson;		// tmp json object
			compJson["_type"] = comp->GetType().GetName();
			comp->StreamWrite(compJson);	// write to tmp
			comps.push_back(compJson);		// add tmp
		}

		// save the children objects
		nlohmann::json& children = j["children"];
		for (u32 i = 0; i < mChildren.size(); ++i)
		{
			GameObject* child = mChildren[i];

			nlohmann::json childJson;		// tmp json object
			child->StreamWrite(childJson);	// write to tmp
			children.push_back(childJson);		// add tmp
		}
	}

	// Component Management
	IComp* GameObject::AddComp(IComp* comp)
	{
		//sanity check avoid duplicates
		//for (auto& Comp : mComponents)
		//	if (Comp->mUID == comp->mUID)
		//		return mComponents.back(); //this return does not make sense
	
		comp->mOwner = this;
		mComponents.push_back(comp);

		return mComponents.back();
	}
	void   GameObject::RemoveComp(IComp* comp) {

		// sanity check: make sure that it's our component
		if (comp->GetOwner() != this) return; // todo: throw error!

		// look for it in the array
		for (u32 i = 0; i < mComponents.size(); ++i)
		{
			if (mComponents[i] == comp) // found it!
			{
				// notify component that it's about to be destroyed
				comp->Shutdown(); 
				delete comp;// free memory
				// remove from component list
				mComponents.erase(mComponents.begin() + i);
				break;
			}
		}

	}
	IComp* GameObject::GetComp(const char* compTypeStr) {
		// FIND -> WARNING IT'S COSTLY
		for (u32 i = 0; i < mComponents.size(); ++i)
		{
			IComp* comp = mComponents[i];

			// found by type name!
			if (comp->GetType().GetName() == compTypeStr)
			{
				return comp;
			}
		}
		return nullptr;
	}

	GameObject* GameObject::clone()
	{

		GameObject* obj = mOwnerSpace->NewObject("default");
		nlohmann::json j;

		this->StreamWrite(j);
		obj->StreamRead(j);

		mParent->AddChild(obj);
		//obj->mOwnerSpace = mOwnerSpace;

		// Set cloned name
		std::string c = GetName();
		c += " clone";
		obj->SetName(c.c_str());

		obj->OnCreate();

		return obj;
	}
}