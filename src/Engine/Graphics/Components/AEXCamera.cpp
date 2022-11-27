#include <Graphics/AEXGfxSystem.h>
#include <../Engine/Composition/TransformComponent.h>
#include <Composition/AEXComposition.h>
#include <Debug/AEXDebug.h>
#include "AEXGfxComps.h"
#include <Platform/AEXSerialization.h>
#include "imgui/imgui.h"
#include "Platform/AEXWindow.h"
namespace AEX{

	Camera::Camera(){}
	void Camera::AddToSystem(){
		aexGraphics->AddCamera(this);
	}
	void Camera::RemoveFromSystem() {
		aexGraphics->RemoveCamera(this);
	}
	void Camera::OnCreate(){
		// TODO: assert that we have a transform
		// TODO: get the transform component
		DebugAssert(this->transform = GetOwner()->GetComp<TransformComp>(), "Camera needs transform!");
	}
	void Camera::Shutdown() {
		RemoveFromSystem();
	}
	bool Camera::Edit(){

		if (ImGui::CollapsingHeader("Camera"))
		{
			// Set color
			float col1[3] = { mViewportColor.r, mViewportColor.g, mViewportColor.b };
			ImGui::ColorEdit3("Background", col1);
			mViewportColor.r = col1[0]; mViewportColor.g = col1[1]; mViewportColor.b = col1[2];
		}

		return true;
	}

	// get view matrix inverse (TRS) = s^r^t^
	AEMtx33 Camera::GetViewMtx33(){
		auto wtr = transform->mLocal;
		return wtr.GetInvMatrix();
	}
	AEMtx44 Camera::GetViewMtx44() {
		auto wtr = transform->mLocal;
		return wtr.GetInvMatrix44();
	}
	AEMtx33 Camera::GetInvViewMtx33() {
		auto wtr = transform->mLocal;
		return wtr.GetMatrix();
	}
	AEMtx44 Camera::GetInvViewMtx44() {
		auto wtr = transform->mLocal;
		return wtr.GetMatrix44();
	}

	// get projection matrix
	AEMtx44 Camera::GetProjectionMtx44() {
		return AEMtx44::OrthoProjGL(mViewRectangle.x, mViewRectangle.y, 0.01f, 1000.0f);
	}
	AEMtx44 Camera::GetInvProjectionMtx44() {
		return GetProjectionMtx44().Inverse();
	}

	

	// get model view projection matrix
	AEMtx44 Camera::GetViewProjMtx44() {
		return GetProjectionMtx44() * GetViewMtx44();
	}
	AEMtx44 Camera::GetInvViewProjMtx44(){
		return GetInvViewMtx44()* GetInvProjectionMtx44();
	}


	//--------------Made by Thomas---------------//
	  //Take one AEVec2 and apply changes to it//
	void Camera::ViewToWorld(AEVec2& MousePos)
	{
		AEVec2 winSz = { (float)aexWindow->GetWidth() , (float)aexWindow->GetHeight() };
		// 1. convert to frame buffer space, where the origin is the bottom-left
			// 0 < x < WW and 0 < y < WH
		MousePos.x += winSz.x / 2.0f;
		MousePos.y += winSz.y / 2.0f;


		// 2. check whether or not the mouse position is located inside the camera viewport
		auto vp = mViewport;
		vp.x *= winSz.x; vp.z *= winSz.x;
		vp.y *= winSz.y; vp.w *= winSz.y;
		if (!(vp.x <= MousePos.x && MousePos.x <= vp.z && vp.y <= MousePos.y && MousePos.y <= vp.w))
			return;


		// 3. convert from the camera's viewport space to the NDC space.
		MousePos -= AEVec2(vp.x, vp.y);            // undo translation to vp position
		MousePos.x = MousePos.x / (vp.z - vp.x);    // undo scale by vp size
		MousePos.y = MousePos.y / (vp.w - vp.y);
		MousePos -= AEVec2(0.5f, 0.5f);            // undo offset translation


	   // 4. transform the mouse position from NDC to camera space
		MousePos.x *= mViewRectangle.x;    // Scale to camera view rectangle
		MousePos.y *= mViewRectangle.y;
		MousePos = AEMtx33::RotRad(transform->GetOrientation()) * MousePos;    // rotate

		AEVec2 pos(transform->GetPosition().x, transform->GetPosition().y);
		MousePos += pos;
	}

	void Camera::StreamRead(const nlohmann::json& j)
	{
		j["viewRectangle"] >> mViewRectangle;
		j["viewport"] >> mViewport;
		j["viewportColor"] >> mViewportColor;
		mbClearViewport = j["viewportClear"];
		DrawOrder = j["drawOrder"];
	}
	void Camera::StreamWrite(nlohmann::json& j) const
	{
		j["viewRectangle"] << mViewRectangle;
		j["viewport"] << mViewport;
		j["viewportColor"] << mViewportColor;
		j["viewportClear"] = mbClearViewport;
		j["drawOrder"] = DrawOrder;
	}
}