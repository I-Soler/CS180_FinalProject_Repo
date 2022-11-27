#include <AEX.h>
#include "Demos.h"
#include <iostream>
#include "Composition/TransformComponent.h"
#include <Extern/imgui/imgui.h>
#include <Extern/imgui/ImGuizmo.h>

using namespace AEX;
static bool showImGuiDemoWindow = false;
static GameObject* transformedObject = nullptr;
static GameObject* cameraObject = nullptr;
static ImGuizmo::OPERATION sCurrentGizmoOperation(ImGuizmo::OPERATION::TRANSLATE);
static ImGuizmo::MODE sCurrentGizmoMode(ImGuizmo::MODE::WORLD);
static bool sGuizmoActive = true;

// ----------------------------------------------------------------------------
// DEMO FUNCTIONS
void ShowTransformGUI() {
	if (auto tr = transformedObject->GetComp<TransformComp>()) {

		ImGui::Text("Local");
		ImGui::PushID(1);
		ImGui::Indent();
		ImGui::DragFloat3("position", tr->mLocal.mTranslation.v);
		ImGui::DragFloat2("scale", tr->mLocal.mScale.v);
		ImGui::DragFloat("rotation", &tr->mLocal.mOrientation);
		ImGui::Unindent();
		ImGui::PopID();

		ImGui::Text("World");
		ImGui::PushID(2);
		ImGui::Indent();
		Transform world = tr->World();
		if (ImGui::DragFloat3("position", world.mTranslation.v))
			tr->SetWorldPosition(world.mTranslation);
		if (ImGui::DragFloat2("scale", world.mScale.v))
			tr->SetWorldScale(world.mScale);
		if (ImGui::DragFloat("rotation", &world.mOrientation))
			tr->SetWorldOrientation(world.mOrientation);
		ImGui::Unindent();
		ImGui::PopID();

		if (ImGui::CollapsingHeader("ImGuizmo Config", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Checkbox("Guizmo Active", &sGuizmoActive);
			if (ImGui::RadioButton("TRANSLATE", sCurrentGizmoOperation == ImGuizmo::TRANSLATE))
				sCurrentGizmoOperation = ImGuizmo::TRANSLATE;
			ImGui::SameLine();
			if (ImGui::RadioButton("SCALE", sCurrentGizmoOperation == ImGuizmo::SCALE))
				sCurrentGizmoOperation = ImGuizmo::SCALE;
			ImGui::SameLine();
			if (ImGui::RadioButton("ROTATE", sCurrentGizmoOperation == ImGuizmo::ROTATE))
				sCurrentGizmoOperation = ImGuizmo::ROTATE;


			if (ImGui::RadioButton("WORLD", sCurrentGizmoMode == ImGuizmo::WORLD))
				sCurrentGizmoMode = ImGuizmo::WORLD;
			ImGui::SameLine();
			if (ImGui::RadioButton("LOCAL", sCurrentGizmoMode == ImGuizmo::LOCAL))
				sCurrentGizmoMode = ImGuizmo::LOCAL;
		}
	}
}
void EditTransform() {

	auto objTransform = transformedObject->GetComp<TransformComp>();
	auto cam = cameraObject->GetComp<Camera>();

	if (aexInput->KeyTriggered('C'))
		sCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	if (aexInput->KeyTriggered('V'))
		sCurrentGizmoOperation = ImGuizmo::ROTATE;
	if (aexInput->KeyTriggered('B'))
		sCurrentGizmoOperation = ImGuizmo::SCALE;

	// enable Imguizmo - this can be set once outside the function.
	ImGuizmo::Enable(sGuizmoActive);
	ImGuizmo::BeginFrame();
	ImGuizmo::SetOrthographic(true); //set orthographic since we're in 2D. 
	ImGuizmo::SetRect(0, 0, (f32)aexWindow->GetWidth(), (f32)aexWindow->GetHeight());

	// get the necessary matrices for ImGuizmo to work:
	auto view = cam->GetViewMtx44(); view.TransposeThis();
	auto proj = cam->GetProjectionMtx44(); proj.TransposeThis();
	Transform world = objTransform->World();
	auto model = world.GetMatrix44(); model.TransposeThis();

	// manipulate the transform
	ImGuizmo::Manipulate(
		view.v,						// camera view transform
		proj.v,						// camera projection transform
		sCurrentGizmoOperation,		// current operation (SCALE/ROTATE/TRANSLATE)
		sCurrentGizmoMode,			// current mode (WORLD/LOCAL)
		model.v);					// object model transform

	// Is the Guizmo in use or not
	if (ImGuizmo::IsUsing()) {

		AEVec3 pos, sca, rot;
		ImGuizmo::DecomposeMatrixToComponents(model.v, pos.v, rot.v, sca.v);
		switch (sCurrentGizmoOperation)
		{
		case ImGuizmo::TRANSLATE:
			objTransform->SetWorldPosition(pos);
			break;
		case ImGuizmo::ROTATE:
			objTransform->SetWorldOrientation(DegToRad(rot.z));
			break;
		case ImGuizmo::SCALE:
			objTransform->SetWorldScale({ sca.x, sca.y });
			break;
		default:
			break;
		}
	}
}

// ----------------------------------------------------------------------------
// GAMESTATE FUNCTIONS
void GUIDemo::LoadResources()
{
	aexGraphics->LoadModel("quad.model");
	aexGraphics->LoadTexture("data/Images/Default.png");

	auto vtxShader = aexGraphics->LoadShader("data/Shaders/VertexTransform.vert");
	auto fragShader = aexGraphics->LoadShader("data/Shaders/TextureMap.frag");
	aexGraphics->LoadShaderProgram("TextureMap.shader", fragShader, vtxShader);
}
void GUIDemo::Initialize()
{
	showImGuiDemoWindow = false;

	auto scn = aexScene;
	// create demo object
	{
		transformedObject = scn->GetMainSpace()->NewObject("Object 1");
		auto tr = transformedObject->NewComp<TransformComp>();
		tr->SetScale({ 256,256 });
		auto ren = transformedObject->NewComp<Renderable>();
		ren->mpModel = aexGraphics->GetModel("quad.model");
		ren->mpShader = aexGraphics->GetShaderProgram("TextureMap.shader");
		ren->mpTexture = aexGraphics->GetTexture("Default.png");
		transformedObject->OnCreate();
	}

	// create camera
	{
		cameraObject = scn->GetMainSpace()->NewObject("Camera");
		auto tr = cameraObject->NewComp<TransformComp>();
		tr->SetPosition({ 0,0, 20 });
		auto cam = cameraObject->NewComp<Camera>();
		cam->mbClearViewport = true;
		cam->mViewportColor = Color(1, 0.5, 0);
		cam->mViewport = AEVec4(0, 0, aexWindow->GetWidth(), aexWindow->GetHeight());
		cam->mViewRectangle = AEVec2(aexWindow->GetWidth(), aexWindow->GetHeight());

		cameraObject->OnCreate();
	}

	// initialize space
	scn->GetMainSpace()->Initialize();
}
void GUIDemo::Shutdown()
{
	aexScene->GetMainSpace()->DeleteObject(transformedObject);
	aexScene->GetMainSpace()->DeleteObject(cameraObject);
}
void GUIDemo::Update()
{
	if (ImGui::Begin("My Window"))
	{
		static bool framerate = true;

		ImGui::Checkbox("Show Demo Window", &showImGuiDemoWindow);
		ImGui::Separator();
		if (ImGui::CollapsingHeader("Frame Rate")) {
			if (framerate)
				ImGui::Text("FPS: %.2f", aexTime->GetFrameRate());
			else
				ImGui::Text("Frame Time: %.3f", aexTime->GetFrameTime());

			if (ImGui::Button("Switch"))framerate = !framerate;
		}

		ImGui::Separator();
		ShowTransformGUI();
	}
	ImGui::End();

	EditTransform();

	// update the object's transform 
	transformedObject->GetComp<TransformComp>()->Update();

	if (showImGuiDemoWindow)
		ImGui::ShowDemoWindow();
}