#include <AEX.h>
#include <Extern/imgui/imgui.h>
#include <Extern/imgui/ImGuizmo.h>
#include <Scene/Scene.h>
#include "AEXEditor.h"
#include "Collisions/Collisions.h"
#include <Platform/AEXSerialization.h>	// save prefabs
#include <Platform/AEXInput.h>
#include <filesystem>
#include <algorithm>
#include <sstream>
#include <fstream>

namespace AEX
{
	static bool showImGuiDemoWindow = false;

	//ImGuizmo
	static ImGuizmo::OPERATION sCurrentGizmoOperation(ImGuizmo::OPERATION::TRANSLATE);
	static ImGuizmo::MODE sCurrentGizmoMode(ImGuizmo::MODE::WORLD);

	bool Editor::Initialize()
	{
		aexScene->LoadFile("data/Scenes/bubolScene.json", true);

		InitEditorCamera();	// Init editor camera in the new scene	

		timer.Start();
		return true;
	}
	void Editor::Update()
	{
		if (aexInput->KeyTriggered(Keys::F5))
			SetEnabled(!Enabled());


		Menu();
		GameObject* camera = mEditorCameras[currentSpace];
		if (Enabled())
		{
			for (auto& it : aexGraphics->mCameras)
			{
				if (it == camera->GetComp<Camera>())
					continue;

				it->SetEnabled(false);
				TransformComp* tr = it->GetOwner()->GetComp<TransformComp>();
				aexGraphics->DrawRect(tr->GetWorldPosition().x, tr->GetWorldPosition().y,it->mViewRectangle.x, it->mViewRectangle.y, Color(1, 1, 1, 1));
			}
			camera->SetEnabled(true);

			ImGuizmo();
			UpdateEditorCamera();

			ObjectManager();
			Inspector();
			//ShowFiles("data");
			//Assets("data");
		}
		else
		{
			for (auto& it : aexGraphics->mCameras)
				it->SetEnabled(true);

			camera->SetEnabled(false);
		}		
	}
	void Editor::Menu()
	{
		// Load and save scene
		ImGui::BeginMainMenuBar();

		//if (ImGui::BeginMenu("File"))
		//{
		//	if (ImGui::Button("New Scene"))
		//	{
		//		aexScene->SaveFile(SceneName.c_str());	// Save the scene we are working in
		//		aexScene->LoadFile("data/Scenes/Default Scene.json");	// Loads the default scene
		//		aexScene->SaveFile("data/Scenes/New Scene.json");		// Creates a new scene
		//		SceneName = "data/Scenes/New Scene.json";

		//		SelectedObjs.clear();	// clean inspector
		//		InitEditorCamera();	// Init editor camera in the new scene	
		//	}

		//	ImGui::EndMenu();
		//}

		//if (ImGui::BeginMenu("Load and Save"))
		//{
		//	ImGui::Checkbox("Show Demo Window", &showImGuiDemoWindow);

		//	static char label[] = "";
		//	ImGui::Text("Name of scene : "); ImGui::SameLine();
		//	ImGui::Text(SceneName.c_str());

		//	if (ImGui::InputText("Edit file name", label, 30))
		//		SceneName = AddScenePath(label);
		//	
		//	if (ImGui::Button("Save"))
		//		aexScene->SaveFile(SceneName.c_str());
		//	
		//	if (ImGui::Button("Load"))			// load file using OpenSaveFileDlg
		//	{
		//		OpenSaveFileDlg dlg;
		//		if (dlg.Open("Select Scene File"))
		//		{
		//			std::string filename = dlg.GetFiles()[0];
		//			SceneName = filename.c_str(); // Store the name

		//			/*aexGraphics->ClearCameras();
		//			aexGraphics->ClearRenderables();*/
		//			for (auto& it : mEditorCameras)	// delete prev camera
		//				it.second->Shutdown();

		//			aexScene->LoadFile(filename.c_str());
		//			SelectedObjs.clear();	// Clean inspector

		//			InitEditorCamera();	// Init editor camera in the new scene	
		//		}
		//	}
		//	ImGui::EndMenu();
		//}

		// Start, pause and continue
		if (ImGui::Button("Start"))
		{
			if (!editing)
				aexScene->SaveFile("data/Scenes/TemporalSave.json");

			aexScene->InitializeSpaces();
			aexCollision->clearContacts();	// PrevFrameContacts must be deleted when restarted

			editing = true;
			SetEnabled(false);
		}

		if (ImGui::Button("Pause"))
			SetEnabled(true);


		if (ImGui::Button("Stop") && editing)
		{
			//for (auto& it : mEditorCameras)	// delete prev camera
			//	it.second->Shutdown();

			aexScene->LoadFile("data/Scenes/TemporalSave.json", false);
			editing = false;
			SetEnabled(true);

			//aexScene->InitializeSpaces();
			InitEditorCamera();
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
			ImGui::Checkbox("Edit collider", &EditCollider);
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
			//if (ImGui::Button("Create Object"))
			//{
			//	Space* mainSp = aexScene->GetMainSpace();				// Get space where object will be added
			//
			//	std::string objStr("New Object ");
			//	objStr += std::to_string(GameObjCounter);				// Avoid object having the same name
			//	GameObjCounter++;
			//
			//	GameObject* Obj = mainSp->NewObject(objStr.c_str());	// create the object
			//
			//	// Every object must have a Transform and a Renderable by default
			//	TransformComp* tr = aexFactory->Create<TransformComp>();
			//	tr->mLocal.mScale = AEVec2(50, 50);
			//	Obj->AddComp(tr);	Obj->NewComp<Renderable>();
			//	Obj->OnCreate(); Obj->Initialize();
			//}
			//
			//ImGui::SameLine();
			//
			//// Delete selected objects
			//if (ImGui::Button("Delete Object") && SelectedObjs.size())
			//{
			//	for (int it = 0; it < SelectedObjs.size(); it++)
			//	{
			//		Space* objSpace = SelectedObjs[it]->mOwnerSpace;	// get space of object to be deleted
			//		objSpace->DeleteObject(SelectedObjs[it]);
			//	}
			//	SelectedObjs.clear();
			//}
			//
			//// Clone selected objects
			//if (ImGui::Button("Clone") && SelectedObjs.size())
			//{
			//	for (int it = 0; it < SelectedObjs.size(); it++)
			//	{
			//		GameObject* obj = SelectedObjs[it]->clone();
			//	}
			//}
			//ImGui::SameLine();
			//
			//// Add a child to selected objects
			//if (ImGui::Button("Create child") && SelectedObjs.size())
			//{
			//	for (int it = 0; it < SelectedObjs.size(); it++)
			//	{
			//		std::string objStr("Child");
			//		objStr += std::to_string(GameObjCounter);	// Avoid object having the same name
			//		GameObjCounter++;
			//
			//		Space* objSpace = SelectedObjs[it]->mOwnerSpace;			// get space from the parent
			//		GameObject* Obj = objSpace->NewObject(objStr.c_str());	// create child in that space
			//		SelectedObjs[it]->AddChild(Obj);
			//
			//		// Every object must have a Transform and a Renderable by default
			//		TransformComp* tr = aexFactory->Create<TransformComp>();
			//		tr->mLocal.mScale = AEVec2(1, 1);
			//		Obj->AddComp(tr);
			//		Obj->AddComp(aexFactory->Create<Renderable>());
			//		Obj->OnCreate(); Obj->Initialize();// don't initialize when in edit mode only when launching the game
			//	}
			//}

			// Show all exisiting objects + children in the scene
			for (auto& it : aexScene->GetAllSpaces())
			{
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

			// Show, Edit and Delete existing Components
			std::vector<IComp*> SComp = SelectedObjs[0]->GetAllComp();
			for (int it = 0; it < SComp.size(); it++)
			{
				bool deleted = false;
				std::string compName = SComp[it]->GetType().GetName();
				std::string CompDelete = "Delete " + compName;

				ImGui::PushID(SComp[it]);	// Make ImGui not bug when two or more colliders added
				// The transform component should not be deletable
				if (compName == "TransformComp")
				{
					if (ImGui::CollapsingHeader(SComp[it]->GetType().GetName().c_str()))
						SComp[it]->Edit();
					ImGui::PopID();
					continue;
				}

				//// Delete component
				//if (ImGui::Button(CompDelete.c_str()))
				//{
				//	SelectedObjs[0]->RemoveComp(SComp[it]);
				//	deleted = true;
				//}
				//ImGui::SameLine();

				if (!deleted)
				{
					if(ImGui::CollapsingHeader(SComp[it]->GetType().GetName().c_str()))
						SComp[it]->Edit();
				}

				ImGui::PopID();
			}

			//if (ImGui::TreeNode("Add Component"))
			//{
			//	//Show all components avaliable to add
			//	for (auto it = Rtti::GetAllTypes().begin(); it != Rtti::GetAllTypes().end(); ++it)
			//	{
			//		if (!it->second.IsDerivedFrom(IComp::TYPE()))
			//			continue;

			//		// avoid adding two components of the same type
			//		// Exception for the collider, an object can have multiple of this
			//		if (SelectedObjs[0]->GetComp(it->second.GetName().c_str()) != nullptr
			//			&& it->second.GetName() != "Collider")
			//			continue;

			//		// Edge cases: 
			//		// -Logic Comp is not a valid component 
			//		// -TransformComp comes by default
			//		if (it->second.GetName() == "LogicComp" || it->second.GetName() == "TransformComp")
			//			continue;

			//		// Create and add the component
			//		if (ImGui::Button(it->second.GetName().c_str()))
			//		{
			//			IComp* Comp = (IComp*)aexFactory->Create(it->second.GetName().c_str());
			//			SelectedObjs[0]->AddComp(Comp);
			//			Comp->AddToSystem(); Comp->OnCreate(); Comp->Initialize();
			//		}
			//	}
			//	ImGui::TreePop();
			//}
		}
		ImGui::End();
	}

	void Editor::Assets(const char* folderPath)
	{		
		if (ImGui::Begin("Assets"))
		{
			// Variables for the button
			int buttonCount = 0;
			auto sz = ImGui::GetContentRegionAvailWidth();
			ImVec2 btnSz = { 80,80 };
			auto pad = ImGui::GetStyle().ItemSpacing;
			u32 btnPerLine = floor(sz / (btnSz.x + pad.x * 2)) - 1;


			const std::filesystem::path data{ folderPath };
			int it = 0;
			for (auto const& dir_entry : std::filesystem::directory_iterator{ data })	// search for the directory selected
			{
				if (!dir_entry.is_directory())	// Only show directories
					continue;

				it++;
				if (it != static_cast<int>(selecFolder))	// Find the selected directory
					continue;

				std::string po = dir_entry.path().u8string();
				const std::filesystem::path SubData{ po.c_str() };
				void (Editor::*handle)(std::string);

				std::string ImageName = "Default.png";
				
				// Temporal ImageName = "Default.png"; until I actually put an image in the folders
				switch (selecFolder)
				{
				case AEX::SelectedFolder::Audio:
				{
					ImageName = "AudioIcon.png";
					handle = &Editor::HandleAudio;
					break;
				}
				case AEX::SelectedFolder::Images:	// Empty, image will be set later
				{
					handle = &Editor::HandleImage;
					break;
				}

				case AEX::SelectedFolder::Model:
				{
					ImageName = "ModelIcon.png";
					ImageName = "Default.png";
					handle = &Editor::HandleModel;
					break;
				}
				case AEX::SelectedFolder::Prefabs:
				{
					ShowPrefab();
					ImageName = "PrefabIcon.png";
					ImageName = "Default.png";
					handle = &Editor::HandlePrefab;
					break;
				}
				case AEX::SelectedFolder::Scene:
				{
					ImageName = "sceneIcon.png";
					handle = &Editor::HandleScene;
					break;
				}
				case AEX::SelectedFolder::Shaders:
				{
					ImageName = "ShaderIcon.png";
					ImageName = "Default.png";
					handle = &Editor::HandleShader;
					break;
				}
				}

				int ID = 0;

				for (auto const& dir_entry : std::filesystem::directory_iterator{ SubData })
				{
					// Set variables and stuff
					std::string name = dir_entry.path().filename().u8string();
					if (selecFolder == AEX::SelectedFolder::Images)
						ImageName = name;

					auto img = aexResources->GetResource<Texture>(ImageName.c_str());
					buttonCount++;

					// Group to show image and text
					ImGui::BeginGroup();
					ImGui::PushID(ID);	ID++;
					if (ImGui::ImageButton((ImTextureID)img->get()->GetGLHandle(), ImVec2(80, 80), { 0,0 }, { 1,-1 }))
						(*this.*handle)(name);
					ImGui::PopID();
					name.resize(11);
					name += "..";
					ImGui::Text(name.c_str());	
					ImGui::EndGroup();
					

					// For making new lines for the buttons
					if (buttonCount == btnPerLine)
						buttonCount = -1;
					else 
						ImGui::SameLine();		// currently does not check if it's last position
				}
				ImGui::Text("");	// Make the last SameLine() from the images to not affect the names	
				ImGui::SameLine();		// currently does not check if it's last position
			}
		}
		ImGui::End();
	}

	//ImGui window to show the files in data
	void Editor::ShowFiles(const char* folderPath)
	{
		auto resources = aexResources;
		ImGui::Begin("Resources");

		// Loads asset from the user's computer
		if (ImGui::Button("Add asset"))
		{
			OpenSaveFileDlg dlg;
			if (dlg.Open("Select File"))
			{
				std::string filename = dlg.GetFiles()[0];
				aexResources->LoadResource(filename.c_str(), false);
			}
		}

		ImVec2 end = ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowSize().x, ImGui::GetWindowPos().y + ImGui::GetWindowSize().y);

		resources->wPos = AEVec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
		resources->wScale = AEVec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);

		resources->mouse = ImGui::IsMouseHoveringRect(ImGui::GetWindowPos(), end);
		if (!ImGui::IsAnyItemHovered())
		{
			resources->drag = false;
		}

		LoadFiles(folderPath);

		ImGui::End();
	}

	//loads the files and if a new one is dropped also loads it
	void Editor::LoadFiles(const char* folderPath)
	{
		// imGui things to select the wanted folder
		ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
		static int selection_mask = (1 << 2);
		int node_clicked = -1;
		int it = 0;

		auto resources = aexResources;

		const std::filesystem::path data{ folderPath };
		bool opened = false;

		for (auto const& dir_entry : std::filesystem::directory_iterator{ data })
		{
			if (dir_entry.is_directory())
			{
				it++;
				std::string po = dir_entry.path().u8string();

				// complex ImGui things
				// Determine with tree is selected
				{
				// Disable the default "open on single-click behavior" + set Selected flag according to our selection.
				ImGuiTreeNodeFlags node_flags = base_flags;
				const bool is_selected = (selection_mask & (1 << it)) != 0;
				if (is_selected)
					node_flags |= ImGuiTreeNodeFlags_Selected;

				// open with double click or arrow
				opened = ImGui::TreeNodeEx((void*)(intptr_t)it, node_flags, po.c_str(), it);
				if (ImGui::IsItemClicked())
				{
					node_clicked = it;
					selecFolder = static_cast<SelectedFolder>(it);
				}
				}
				std::string bar = "\\";

				if (ImGui::IsItemHovered() && resources->drag)
				{
					for (auto it : resources->dragdropfiles)
					{
						FilePath fp(it.c_str());
						std::fstream file;

						file.open(it, std::ios_base::in | std::ios::binary);
						auto ss = std::ostringstream{};

						if (file.is_open())
						{
							ss << file.rdbuf();
							file.close();
						}

						file.open(po.c_str() + bar + fp.mFilename + fp.mExtension, std::ios_base::out | std::ios::binary);

						file.write(ss.str().c_str(), ss.str().size());

						file.close();


						// load current file
						resources->LoadResource((po.c_str() + bar + fp.mFilename + fp.mExtension).c_str(), true);
						resources->drag = false;
					}
				}

				if (opened)
				{
					LoadFiles(po.c_str());
					ImGui::TreePop();
				}
			}
			else
			{
				std::string pe = dir_entry.path().filename().u8string();

				if (dir_entry.path().extension() == ".json")
				{
					continue;
				}

				opened = ImGui::TreeNodeEx(pe.c_str(), ImGuiTreeNodeFlags_Leaf);

				if (opened)
					ImGui::TreePop();
			}
		}

		if (node_clicked != -1)
		{
			// Update selection state
			// (process outside of tree loop to avoid visual inconsistencies during the clicking frame)
			if (ImGui::GetIO().KeyCtrl)
				selection_mask ^= (1 << node_clicked);          // CTRL+click to toggle
			else //if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, may want to preserve selection when clicking on item that is part of the selection
				selection_mask = (1 << node_clicked);           // Click to single-select
		}
	}


	void Editor::InitEditorCamera()
	{
		mEditorCameras.clear();
		currentSpace = aexScene->GetMainSpace();
		                                       
		auto scene = aexScene;
		auto allSpaces = scene->GetAllSpaces();

		// go through the existing spaces inside the scene
		// for each space, create an editor camera and store. 
		for (auto &sp : allSpaces)
		{
			auto camObj = aexFactory->Create<GameObject>();
			camObj->mOwnerSpace = sp;
			camObj->mParent = sp;		// because of the mParent and owner mess
			auto tr = camObj->NewComp<TransformComp>();
			tr->SetWorldPosition(AEVec3(tr->GetWorldPosition().x, tr->GetWorldPosition().y, 500));

			auto cam = camObj->NewComp<Camera>();
			cam->mViewRectangle = { 1280,720 };
			cam->mViewport = { 0,0, 1, 1 };
			camObj->OnCreate();

			//...
			// initiliaze the camera
			//...

			mEditorCameras[sp] = camObj;
		}
	}

	void Editor::UpdateEditorCamera()
	{
		GameObject* camObj = mEditorCameras[currentSpace];
		Camera* cam = camObj->GetComp <Camera>();
		TransformComp* tr = camObj->GetComp<TransformComp>();

		auto input = aexInput;

		// Check click of the map
		if (!input->MousePressed(0) || ImGuizmo::IsUsing())
			return;

		//SelectedObjs.clear();

		AEVec2 mousepos = input->GetMousePos();

		cam->ViewToWorld(mousepos);

		// Go through all objects
		for (auto& it : aexScene->GetAllSpaces())
		{
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
		GameObject* camObj = mEditorCameras[currentSpace];

		TransformComp* tr;
		GameObject* obj;
		Camera* cam = camObj->GetComp<Camera>();;

		if (EditCollider)
			tr = SelectedObjs[0]->GetComp<Collider>();
		
		if(!EditCollider || tr == nullptr)
		{
			EditCollider = false;

			obj = SelectedObjs[0];

			tr = obj->GetComp<TransformComp>();
		}


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
				if (EditCollider)
					tr = it->GetComp<Collider>();
				if (!EditCollider || tr == nullptr)
				{
					EditCollider = false;

					tr = it->GetComp<TransformComp>();
				}

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

			// drag drop
			{
				if (ImGui::BeginDragDropSource()) {

					ImGui::Text(ObjList[it]->GetName());
					ImGui::SetDragDropPayload("DND_OBJ_PTR", &ObjList[it], sizeof(GameObject*));
					ImGui::EndDragDropSource();
				}

				if (ImGui::BeginDragDropTarget())
				{
					if (ImGui::AcceptDragDropPayload("DND_OBJ_PTR"))
					{
						// set this object to parent
						GameObject* obj = *(GameObject**) ImGui::GetDragDropPayload()->Data;
						if (obj != ObjList[it])
						{
							// remove transform from the hierarchy
							auto tr = obj->GetComp<TransformComp>();
							if (tr)
							{
								// remove parent if we have it
								if (tr->Parent())
									tr->Parent()->RemoveChildTransform(tr);
							}

							ObjList[it]->AddChild(obj);

							// adjust transform hierarchy
							if (tr) {

								// our new parent has a transform
								if (auto newParentTr = ObjList[it]->GetComp <TransformComp>()) {
									newParentTr->AddChildTransform(tr);
								}
							}
						}
					}
					ImGui::EndDragDropTarget();
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

	void Editor::HandleAudio(std::string audio)
	{

	}
	void Editor::HandleImage(std::string image)
	{

	}
	void Editor::HandleModel(std::string model)
	{

	}
	void Editor::HandleScene(std::string scene)
	{
		std::string path = "data/Scenes/";
		path += scene;

		aexScene->SaveFile(SceneName.c_str());	// Save the scene we are working in
		aexScene->LoadFile(path.c_str());		// Loads the default scene
		SceneName = path.c_str();				// Override the current scene name

		SelectedObjs.clear(); // avoid inspector crashing
		InitEditorCamera();	// Init editor camera in the new scene	
	}
	void Editor::HandlePrefab(std::string prefab)
	{

	}
	void Editor::HandleShader(std::string shader)
	{

	}

	void Editor::ShowPrefab()
	{
		// Save prefab to the user's computer
			// Quitar de aqui cuando se decida donde ponerlo
		if (ImGui::Button("Create Prefab"))
		{
			if (!SelectedObjs.empty()) for (auto& it : SelectedObjs)
			{
				nlohmann::json j;
				it->StreamWrite(j);
				std::string filename = "data/Prefabs/";
				filename += it->GetName();
				filename += ".arch";
				SaveJsonToFile(j, filename.c_str());
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
				prefObj->StreamRead(selectedPrefab->get()->prefab_json);

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
					nlohmann::json& j = selectedPrefab->get()->prefab_json;
					printf("%s\n", selectedPrefab->GetFilePath().mFullPath.c_str());

					// overwrite object into prefab json file
					j.clear();
					SelectedObjs[0]->StreamWrite(j);
					SaveJsonToFile(j, selectedPrefab->GetFilePath().mFullPath.c_str());
				}
			}
		}
	}


	std::string AddScenePath(std::string file)
	{
		file.insert(0, "data/Scenes/");
		file += ".json";
		return file;
	}
}