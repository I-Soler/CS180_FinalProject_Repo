#include <AEX.h>
#include "Commons.h"
using namespace AEX;


void CamController::OnCreate()  {
	cam = GetOwner()->GetComp<Camera>();
	transform = GetOwner()->GetComp<TransformComp>();
}

void CamController::Update()  {
	f32 moveSpeed = 2.0f;
	f32 zoomSpeed = .30f;

	if (aexInput->MousePressed(0))
		transform->mLocal.mTranslation += aexInput->GetMouseMovement() * moveSpeed;

	if (auto mw = aexInput->GetMouseWheel())
		if (mw > 0) cam->mViewRectangle *= (1.0f + zoomSpeed);
		else cam->mViewRectangle *= (1.0f - zoomSpeed);
}