#include <AEX.h>
#include <Extern/imgui/imgui.h>
#include <Extern/imgui/ImGuizmo.h>
#include "AEXEditor.h"
#include "Collisions/Collisions.h"
#include <Platform/AEXSerialization.h>	// save prefabs
#include <Platform/AEXInput.h>

namespace AEX
{
	static bool showImGuiDemoWindow = false;
	static int GameObjCounter = 0;					// for creating new unnamed objects
	static std::vector<GameObject*> SelectedObjs;	// Handle selecting objects

	//ImGuizmo
	static ImGuizmo::OPERATION sCurrentGizmoOperation(ImGuizmo::OPERATION::TRANSLATE);
	static ImGuizmo::MODE sCurrentGizmoMode(ImGuizmo::MODE::WORLD);

	bool Editor::Initialize()
	{
		aexScene->LoadFile("Default Scene", false);

		InitEditorCamera();

		timer.Start();
		return true;
	}
	void Editor::Update()
	{
		if (aexInput->KeyTriggered(Keys::F5))
			SetEnabled(!Enabled());

		// Auto save every 5 mins
		if (timer.GetTimeSinceStart() >= 60 * 5)
		{
			aexScene->SaveFile("Auto saved scene");
			timer.Reset();
		}

		Menu();
		Space* myScene = aexScene->GetSpace("Editor Space");
		GameObject* camera = myScene->FindObjectByName("Editor Camera");
		if (Enabled())
		{
			camera->SetEnabled(true);
			UpdateEditorCamera();

			ObjectManager();
			Inspector();
			Assets();
			ImGuizmo();
			aexResources->ShowFiles("data");
		}
		else
			camera->SetEnabled(false);
	}
	void Editor::Menu()
	{
		// Load and save scene
		ImGui::BeginMainMenuBar();
		if (ImGui::BeginMenu("Load and Save"))
		{
			ImGui::Checkbox("Show Demo Window", &showImGuiDemoWindow);

			static char label[64] = "EditorFileTest";		// enter name to save file
			ImGui::InputText("Enter File Name", label, 30);

			if (ImGui::Button("Save"))
				aexScene->SaveFile(label);

			if (ImGui::Button("Load"))			// load file using OpenSaveFileDlg
			{
				OpenSaveFileDlg dlg;
				if (dlg.Open("Select Scene File"))
				{
					std::string filename = dlg.GetFiles()[0];
					aexScene->LoadFile(filename.c_str());
					SelectedObjs.clear();	// Avoid bug

					InitEditorCamera();	// Init editor camera in the new scene
				}
			}
			ImGui::EndMenu();
		}

		// Start, pause and continue
		if (ImGui::Button("Start"))
		{
			if (!editing)
				aexScene->SaveFile("TemporalSave");

			aexScene->InitializeSpaces();
			aexCollision->Initialize();	// PrevFrameContacts must be deleted when restarted, ask Thomas if this is ok

			editing = true;
			SetEnabled(false);
		}

		if (ImGui::Button("Pause"))
			SetEnabled(true);


		if (ImGui::Button("Stop") && editing)
		{
			aexScene->LoadFile("TemporalSave", false);
			editing = false;
			SetEnabled(true);
			SelectedObjs.clear();	// No objects showing on the inspector, otherwise it breaks 
		}

		// Change Guizmo mode
		if (ImGui::BeginMenu("Change ImGuizmo type"))
		{
			static int e = 0;
			if (ImGui::RadioButton("Translate", &e, 0))
				sCurrentGizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
			if (ImGui::RadioButton("Scale", &e, 1))
				sCurrentGizmoOperation = ImGuizmo::OPERATION::SCALE;
			if (ImGui::RadioButton("Rotate", &e, 2))
				sCurrentGizmoOperation = ImGuizmo::OPERATION::ROTATE;
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Spaces"))
		{
			static char Spacelabel[20] = "Default";
			ImGui::InputText("", Spacelabel, IM_ARRAYSIZE(Spacelabel));

			if (ImGui::Button("Add Space"))
				aexScene->NewSpace(Spacelabel);

			ImGui::SameLine();
			if (ImGui::Button("Delete Space"))
			{
				// Some spaces should be non deletable
				if (strcmp(Spacelabel, "Main") != 0 && strcmp(Spacelabel, "Editor Space") != 0)
					aexScene->DestroySpace(Spacelabel);
				else
					std::cout << "cant delete those spaces" << std::endl;
			}

			if (ImGui::CollapsingHeader("List of spaces"))
			{
				for (auto& it : aexScene->GetAllSpaces())
					ImGui::Text(it->GetName());
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();

		if (showImGuiDemoWindow)
			ImGui::ShowDemoWindow();
	}

	void Editor::ObjectManager()
	{
		if (ImGui::Begin("Object Manager"))
		{
			// Create default Object 
			if (ImGui::Button("Create Object"))
			{
				Space* mainSp = aexScene->GetMainSpace();				// Get space where object will be added

				std::string objStr("New Object ");
				objStr += std::to_string(GameObjCounter);				// Avoid object having the same name
				GameObjCounter++;

				GameObject* Obj = mainSp->NewObject(objStr.c_str());	// create the object

				// Every object must have a Transform and a Renderable by default
				TransformComp* tr = aexFactory->Create<TransformComp>();
				tr->mLocal.mScale = AEVec2(50, 50);
				Obj->AddComp(tr);	Obj->NewComp<Renderable>();
				Obj->OnCreate(); Obj->Initialize();
			}

			ImGui::SameLine();

			// Delete selected objects
			if (ImGui::Button("Delete Object") && SelectedObjs.size())
			{
				for (int it = 0; it < SelectedObjs.size(); it++)
				{
					Space* objSpace = SelectedObjs[it]->mOwnerSpace;	// get space of object to be deleted
					objSpace->DeleteObject(SelectedObjs[it]);
				}
				SelectedObjs.clear();
			}

			// Clone selected objects
			if (ImGui::Button("Clone") && SelectedObjs.size())
			{
				for (int it = 0; it < SelectedObjs.size(); it++)
				{
					GameObject* obj = SelectedObjs[it]->clone();
				}
			}
			ImGui::SameLine();

			// Add a child to selected objects
			if (ImGui::Button("Create child") && SelectedObjs.size())
			{
				for (int it = 0; it < SelectedObjs.size(); it++)
				{
					std::string objStr("Child");
					objStr += std::to_string(GameObjCounter);	// Avoid object having the same name
					GameObjCounter++;

					Space* objSpace = SelectedObjs[it]->mOwnerSpace;			// get space from the parent
					GameObject* Obj = objSpace->NewObject(objStr.c_str());	// create child in that space
					SelectedObjs[it]->AddChild(Obj);

					// Every object must have a Transform and a Renderable by default
					TransformComp* tr = aexFactory->Create<TransformComp>();
					tr->mLocal.mScale = AEVec2(1, 1);
					Obj->AddComp(tr);
					Obj->AddComp(aexFactory->Create<Renderable>());
					Obj->OnCreate(); Obj->Initialize();
				}
			}

			// Show all exisiting objects + children in the scene
			for (auto& it : aexScene->GetAllSpaces())
			{
				if (strcmp(it->GetName(), "Editor Space") == 0) continue;		// don't show editor camera
				const std::vector<GameObject*>& ObjList = it->GetChildren();	// Gets childrens
				ShowObjects(ObjList, false);
			}
		}
		ImGui::End();
	}

	void Editor::Inspector()
	{
		if (ImGui::Begin("Inspector"))
		{
			// dont show anything if no object selected
			if (SelectedObjs.size() != 1)
			{
				ImGui::End();
				return;
			}

			// Get name of the Obj and allow changing it
			// Este const_cast tiene sentido y no rompe nada, que nadie se alarme
			char* label = const_cast<char*>(SelectedObjs[0]->GetName());
			ImGui::InputText("", label, 20);

			// show object tag
			ImGui::Text("tag:");
			ImGui::SameLine();
			ImGui::Text(tags[SelectedObjs[0]->mTag].c_str());

			// set object tag
			if (ImGui::CollapsingHeader("Set tag"))
				for (std::map<unsigned, std::string>::iterator it = tags.begin(); it != tags.end(); ++it)
					if (ImGui::Button(it->second.c_str()))
						SelectedObjs[0]->mTag = it->first;

			// Show the space of the object and allow changing space
			ImGui::Text("Space"); ImGui::SameLine();
			if (ImGui::TreeNode(SelectedObjs[0]->mOwnerSpace->GetName()))
			{
				for (auto& it : aexScene->GetAllSpaces())
				{
					if (strcmp(it->GetName(), "Editor Space") == 0)	// Edge case
						continue;

					if (ImGui::Button(it->GetName()))
					{
						Space* ObjSpace = SelectedObjs[0]->mOwnerSpace;
						ObjSpace->RemoveChild(SelectedObjs[0]); // Remove object from old space

						it->AddChild(SelectedObjs[0]);			// Add object to the new space
					}
				}
				ImGui::TreePop();
			}
			ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();	// More visually apealing


			// Show, Edit and Delete existing Components
			std::vector<IComp*> SComp = SelectedObjs[0]->GetAllComp();
			for (int it = 0; it < SComp.size(); it++)
			{
				bool deleted = false;
				std::string compName = SComp[it]->GetType().GetName();
				std::string CompDelete = "Delete " + compName;

				// The transform component should not be deletable
				if (compName == "TransformComp")
				{
					SComp[it]->Edit();
					continue;
				}

				// Delete component
				if (ImGui::Button(CompDelete.c_str()))
				{
					SelectedObjs[0]->RemoveComp(SComp[it]);
					deleted = true;
				}
				ImGui::SameLine();

				if (!deleted)
					SComp[it]->Edit();
			}

			if (ImGui::TreeNode("Add Component"))
			{
				//Show all components avaliable to add
				for (auto it = Rtti::GetAllTypes().begin(); it != Rtti::GetAllTypes().end(); ++it)
				{
					if (!it->second.IsDerivedFrom(IComp::TYPE()))
						continue;

					// avoid adding two components of the same type
					// Exception for the collider, an object can have multiple of this
					if (SelectedObjs[0]->GetComp(it->second.GetName().c_str()) != nullptr
						&& it->second.GetName() != "Collider")
						continue;

					// Edge cases: 
					// -Logic Comp is not a valid component 
					// -TransformComp comes by default
					if (it->second.GetName() == "LogicComp" || it->second.GetName() == "TransformComp")
						continue;

					// Create and add the component
					if (ImGui::Button(it->second.GetName().c_str()))
					{
						IComp* Comp = (IComp*)aexFactory->Create(it->second.GetName().c_str());
						SelectedObjs[0]->AddComp(Comp);
						Comp->AddToSystem(); Comp->OnCreate(); Comp->Initialize();
					}
				}
				ImGui::TreePop();
			}
		}
		ImGui::End();
	}

	void Editor::Assets()
	{
		if (ImGui::Begin("Assets"))
		{
			// Save prefab to the user's computer
			if (ImGui::CollapsingHeader("Create Prefab"))
			{
				if (SelectedObjs.empty())
					ImGui::Text("select an object");

				else for (auto& it : SelectedObjs)
				{
					ImGui::Text("set prefab named:");
					static char prefabName[100];
					if (ImGui::InputText("", prefabName, 100, ImGuiInputTextFlags_EnterReturnsTrue))
					{
						nlohmann::json j;
						it->StreamWrite(j);
						std::string filename = "data/Prefabs/";
						filename += prefabName;
						filename += ".arch";
						SaveJsonToFile(j, filename.c_str());
					}
				}
			}

			// Load prefab from the user's computer
			if (ImGui::CollapsingHeader("Instantiate Prefab"))
			{
				static TResource<Archetype>* selectedPrefab = nullptr;
				static unsigned prefabCounter = 0;
				if (aexEditor->getResource<Archetype>(&selectedPrefab))
				{
					// instantiate prefab when clicking on it
					GameObject* prefObj = aexScene->GetMainSpace()->NewObject("whatever");
					prefObj->StreamRead(selectedPrefab->GetRawResource()->prefab_json);

					if (prefabCounter > 0)
					{
						std::string prefName = prefObj->GetName();
						prefName += prefabCounter + 'a';
						prefObj->SetName(prefName.c_str());
					}
					++prefabCounter;

					prefObj->OnCreate();
					prefObj->Initialize();
					GameObjCounter++;
				}
			}

			// Update prefab to the user's computer
			if (ImGui::CollapsingHeader("Update Prefab"))
			{
				if (SelectedObjs.empty())
					ImGui::Text("select an object");
				else if (SelectedObjs.size() > 1)
					ImGui::Text("select only one object");

				else
				{
					TResource<Archetype>* selectedPrefab = nullptr;
					if (ImGui::CollapsingHeader("UPDATE") && aexEditor->getResource<Archetype>(&selectedPrefab))
					{
						nlohmann::json& j = selectedPrefab->GetRawResource()->prefab_json;
						printf("%s\n", selectedPrefab->GetFilePath().mFullPath.c_str());

						// overwrite object into prefab json file
						j.clear();
						SelectedObjs[0]->StreamWrite(j);
						SaveJsonToFile(j, selectedPrefab->GetFilePath().mFullPath.c_str());
					}
				}
			}
		}
		ImGui::End();
	}

	void Editor::InitEditorCamera()
	{
		Space* EditorSpace = aexScene->NewSpace("Editor Space");

		GameObject* cam = EditorSpace->NewObject("Editor Camera");

		auto tr = (TransformComp*)cam->AddComp(aexFactory->Create<TransformComp>());
		auto c = (Camera*)cam->AddComp(aexFactory->Create<Camera>());
		c->mViewRectangle = { 1280,720 };
		//c->mViewport = { 0,0, (f32)aexWindow->GetWidth(), (f32)aexWindow->GetHeight() };
		c->mViewport = { 0,0, 1, 1 };


		tr->SetWorldPosition(AEVec3(tr->GetWorldPosition().x, tr->GetWorldPosition().y, 500));

		cam->OnCreate(); cam->Initialize();
	}

	void Editor::UpdateEditorCamera()
	{
		Space* myScene = aexScene->GetSpace("Editor Space");
		GameObject* camObj = myScene->FindObjectByName("Editor Camera");
		Camera* cam = camObj->GetComp <Camera>();
		TransformComp* tr = camObj->GetComp<TransformComp>();

		auto input = aexInput;

		if (input->KeyPressed('W'))
			tr->SetWorldPosition(tr->GetWorldPosition() + AEVec3(0, 1, 0));

		if (input->KeyPressed('A'))
			tr->SetWorldPosition(tr->GetWorldPosition() + AEVec3(-1, 0, 0));

		if (input->KeyPressed('S'))
			tr->SetWorldPosition(tr->GetWorldPosition() + AEVec3(0, -1, 0));

		if (input->KeyPressed('D'))
			tr->SetWorldPosition(tr->GetWorldPosition() + AEVec3(1, 0, 0));

		if (input->GetMouseWheel() > 0)
			cam->mViewRectangle *= 0.99f;
		else if (input->GetMouseWheel() < 0)
			cam->mViewRectangle *= 1.01f;

		// Check click of the map
		if (!input->MousePressed(0))
			return;

		//SelectedObjs.clear();

		AEVec2 mousepos = input->GetMousePos();

		cam->ViewToWorld(mousepos);

		// Go through all objects
		for (auto& it : aexScene->GetAllSpaces())
		{
			if (strcmp(it->GetName(), "Editor Space") == 0) continue;	// cant select editor camera

			for (auto& it2 : it->GetChildren())
			{
				TransformComp* tr = it2->GetComp<TransformComp>();
				AEVec2 pos(mousepos.x, mousepos.y);
				AEVec2 trans(tr->GetWorldPosition().x, tr->GetWorldPosition().y);
				if (StaticPointToStaticRect(&pos, &trans, tr->GetWorldScale().x, tr->GetWorldScale().y))			// Check if mouse is inside object
				{
					SelectedObjs.clear();
					SelectedObjs.push_back(it2);
				}
			}
		}
	}


	void Editor::ImGuizmo()
	{
		// dont show anything if no object selected
		if (SelectedObjs.size() < 1)
			return;

		// Get things for the ImGuizmo
		Space* myScene = aexScene->GetSpace("Editor Space");
		GameObject* camera = myScene->FindObjectByName("Editor Camera");
		GameObject* obj = SelectedObjs[0];

		Camera* cam = camera->GetComp<Camera>();
		TransformComp* tr = obj->GetComp<TransformComp>();

		// change Editing mode
		if (aexInput->KeyTriggered('C'))
			sCurrentGizmoOperation = ImGuizmo::TRANSLATE;
		if (aexInput->KeyTriggered('V'))
			sCurrentGizmoOperation = ImGuizmo::ROTATE;
		if (aexInput->KeyTriggered('B'))
			sCurrentGizmoOperation = ImGuizmo::SCALE;

		// enable Imguizmo thingies
		ImGuizmo::Enable(true);
		ImGuizmo::BeginFrame();
		ImGuizmo::SetOrthographic(true); //set orthographic since we're in 2D. 
		ImGuizmo::SetRect(0, 0, (f32)aexWindow->GetWidth(), (f32)aexWindow->GetHeight());

		// get the necessary matrices for ImGuizmo to work:
		auto view = cam->GetViewMtx44(); view.TransposeThis();
		auto proj = cam->GetProjectionMtx44(); proj.TransposeThis();
		Transform world = tr->World();
		auto model = world.GetMatrix44(); model.TransposeThis();

		// manipulate the transform
		ImGuizmo::Manipulate(
			view.v,						// camera view transform
			proj.v,						// camera projection transform
			sCurrentGizmoOperation,		// current operation (SCALE/ROTATE/TRANSLATE)
			sCurrentGizmoMode,			// current mode (WORLD/LOCAL)
			model.v);					// object model transform

		AEVec3 pos, sca, rot;
		ImGuizmo::DecomposeMatrixToComponents(model.v, pos.v, rot.v, sca.v);

		// For moving multiple objects
		static AEVec3 prevPos, currentPos;
		if (aexInput->MouseTriggered(0))
		{
			prevPos = pos;
			currentPos = pos;
		}
		else if (aexInput->MousePressed(0))
		{
			prevPos = currentPos;
			currentPos = pos;
		}

		// Is the Guizmo in use or not
		if (ImGuizmo::IsUsing()) {
			for (auto& it : SelectedObjs)
			{
				tr = it->GetComp<TransformComp>();

				switch (sCurrentGizmoOperation)
				{
				case ImGuizmo::TRANSLATE:
					tr->Translate(currentPos - prevPos);
					break;
				case ImGuizmo::ROTATE:
					tr->Rotate(DegToRad(rot.z));
					break;
				case ImGuizmo::SCALE:
					tr->Scale({ sca.x, sca.y });
					break;
				default:
					break;
				}
			}
		}
	}

	void Editor::ShowObjects(const std::vector<GameObject*>& ObjList, bool indent)
	{
		// Move object to right depending on hierarchy level
		if (indent)
			ImGui::Indent();

		for (int it = 0; it < ObjList.size(); it++)
		{
			// determine if selected, so it will shown as selected or not
			bool selected = false;
			for (int selIt = 0; selIt < SelectedObjs.size(); ++selIt)
			{
				if (ObjList[it] == SelectedObjs[selIt])
				{
					selected = true;
					break;
				}
			}

			// Select and de-select objects
			if (ImGui::Selectable(ObjList[it]->GetName(), &selected))
			{
				if (!selected)
				{
					// remove from selectedObjec if de-selected
					for (int selIt = 0; selIt < SelectedObjs.size(); ++selIt)
					{
						if (ObjList[it] == SelectedObjs[selIt])
							SelectedObjs.erase(SelectedObjs.begin() + selIt);
					}
				}

				else    // Detects if we should only select one or allow multiple selection
				{
					if (aexInput->KeyPressed(Keys::KeyCodes::Control))
						SelectedObjs.push_back(ObjList[it]);
					else
					{
						SelectedObjs.clear();
						SelectedObjs.push_back(ObjList[it]);
					}
				}

			}

			// Use recursion to show the childrens of the object
			const std::vector<GameObject*>& ChildrenList = ObjList[it]->GetChildren();
			if (ChildrenList.size() != 0)
				ShowObjects(ObjList[it]->GetChildren(), true);
		}

		if (indent)
			ImGui::Unindent();
	}
}

//tr = it->GetComp<TransformComp>();
//
//AEVec3 pos, sca, rot;
//ImGuizmo::DecomposeMatrixToComponents(model.v, pos.v, rot.v, sca.v);
//
//static AEVec3 initialPos;
//static AEVec3 initialWorldPos;
//static bool firstPressed = true;
//
//if (firstPressed && aexInput->MousePressed(0))
//{
//	initialPos = pos;
//	initialWorldPos = tr->GetWorldPosition();
//	firstPressed = false;
//}
//if (aexInput->MouseReleased(0))
//{
//
//	firstPressed = true;
//}
//
//switch (sCurrentGizmoOperation)
//{
//case ImGuizmo::TRANSLATE:
//	tr->SetWorldPosition(initialWorldPos + pos - initialPos);
//	break;
//case ImGuizmo::ROTATE:
//	tr->SetWorldOrientation(DegToRad(rot.z));
//	break;
//case ImGuizmo::SCALE:
//	tr->SetWorldScale({ sca.x, sca.y });
//	break;
//default:
//	break;
//}