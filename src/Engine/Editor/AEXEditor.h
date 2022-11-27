#pragma once
#include "..\Core\AEXCore.h"
#include "ResourceMgr/ResourceMgr.h"
#include <Platform/AEXTime.h>

namespace AEX
{
	class Editor : public IBase
	{
		AEX_RTTI_DECL(Editor, IBase);
		AEX_SINGLETON(Editor) {}

	public:
		bool Initialize();
		void Update();

		void Menu();
		void ImGuizmo();
		void ObjectManager();
		void Inspector();
		void Assets();

		void InitEditorCamera();
		void UpdateEditorCamera();

		template<typename T>
		bool getResource(TResource<T>** selectedResource);

		void ShowObjects(const std::vector<GameObject*> &ObjList, bool indent);

		bool editing = false;
		AEXTimer timer;


		std::map<unsigned, std::string> tags =
		{
			{0, "Default"},
			{1, "Player"},
			{2, "Enemy"}
		};
	};
}

#define aexEditor AEX::Singletons::Get<AEX::Editor>()

// todo(put this in an inl file)
//#include Editor.inl

namespace AEX
{
	template<typename T>
	bool Editor::getResource(TResource<T>** selectedResource)
	{
		if (!selectedResource)
			return false;

		std::string resourceTypeName = T::TYPE().GetName();

		auto resMgr = aexResources;
		const auto& allResources = resMgr->GetAllResources();
		auto it = allResources.find(resourceTypeName);
		if (it == allResources.end())
		{
			ImGui::Text("No Resources of types %s", resourceTypeName);
			return false;
		}

		// Display loaded resources
		for (auto resIt : it->second)
		{
			// determine if selected
			bool selected = false;

			if (resIt.second == *selectedResource)
				selected = true;

			if (ImGui::Selectable(resIt.second->GetName(), &selected))
			{
				// the internal resource is null-> get it to force a reload in case
				// the internal resource was soft loaded. 
				if (resIt.second->GetRawResource() == nullptr) {
					resMgr->GetResource<T>(resIt.first.c_str());
				}

				*selectedResource = dynamic_cast<TResource<T>*>(resIt.second);
				return true;
			}
		}
		return false;
	}
}

