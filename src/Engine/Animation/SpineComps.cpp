// include spine
#include <spine/spine.h>
#include <spine/Extension.h>
#include <AEX.h>
#include <Platform/AEXSerialization.h>
using namespace AEX;

// include opengl
#include <Graphics/AEXGL.h>
#include "SpineComps.h"

#include "imgui/imgui.h"
/*
	The SpineExtension class is a mechanism for the engine to
	control how spine allocates memory, reads data from file.
	This is essentially spine-cpp's platform-independent layer.

	Fortunately, spine-cpp comes with a default extension that work
	with the default memory allocation functions of the C/C++ runtime
	(malloc, alloc, file I/O, etc...).

	All we have to do is to return the default extension.
*/
namespace spine
{
	SpineExtension* getDefaultExtension()
	{
		return new spine::DefaultSpineExtension();
	}
}

namespace AEX
{
	// ------------------------------------------------------------------------
	#pragma region// EventToAnimTimeMap
	void EventToAnimTimeMap::ProcessJson(const char* filename)
	{
		nlohmann::json j;
		ReadJsonFromFile(j, filename);
		auto& animationsJson = j["animations"];
		for (auto it = animationsJson.begin(); it != animationsJson.end(); ++it)
		{
			auto evit = it->find("events");
			if (evit != it->end())
			{
				for (auto& evJson : *evit) {
					if (evJson.find("time") != evJson.end())
						AnimEvents[it.key()][evJson["name"].get<std::string>()].push_back(evJson["time"]);
				}
			}
		}
	}

	void EventToAnimTimeMap::SaveToJson(const char* filename)
	{
		nlohmann::json j;
		for (auto& anim : AnimEvents) {
			auto &animJson = j[anim.first];
			for (auto& ev_array : anim.second) {
				auto& evJson = animJson[ev_array.first];
				for (auto val : ev_array.second)
					evJson.push_back(val);
			}
		}
		SaveJsonToFile(j, filename);
	}

	void EventToAnimTimeMap::LoadFromJson(const char* filename)
	{
		AnimEvents.clear();
		nlohmann::json j;
		ReadJsonFromFile(j, filename);
		for (auto it = j.begin(); it != j.end(); ++it) {
			auto& anim = AnimEvents[it.key()];
			for (auto evJson = it.value().begin(); evJson != it.value().end(); ++evJson)
			{
				auto& ev_array = anim[evJson.key()];
				for (auto& f : evJson.value())
					ev_array.push_back(f.get<float>());
			}
		}
	}

	#pragma endregion

	SpineData::SpineData() {}
	SpineData::~SpineData() {
		FreeData();
	}
	void SpineData::FreeData() {

		// Free Data if it's already loaded
		if (mSkeletonData)delete mSkeletonData;
		if (mAtlas) delete mAtlas;
		if (mAnimStateData) delete mAnimStateData;

		mSkeletonData = 0;
		mAtlas = 0;
		mAnimStateData = 0;
	}
	void SpineData::Load(const char* jsonPath, const char* atlasFile, ProcessPolicyFlag flag) {
		if (FilePath::Exists(jsonPath) && FilePath::Exists(atlasFile))
		{
			// free any existing data
			FreeData();

			// load atlas first
			mAtlas = new spine::Atlas(atlasFile, &spine::sTextureLoader);

			// load json again
			// json variable
			spine::SkeletonJson json(mAtlas);

			// set the scale at which the skeleton should be loaded
			json.setScale(mJsonScale);

			mSkeletonData = json.readSkeletonDataFile(jsonPath);

			// loading failed
			if (!mSkeletonData) {
				DebugPrint("ERROR! Loading skel data Json: %s\n", json.getError().buffer());
				FreeData();
				return;
			}

			// create a new animation state data
			mAnimStateData = new spine::AnimationStateData(mSkeletonData);

			// post processing? 
			if (flag)
			{
				if (flag & SpineData::eLoadAnimEvents) {
					// try to find the data path
					FilePath jpp(jsonPath);
					std::string fpath = jpp.mDirectory + jpp.mFilename + "_events.json";
					if (FilePath::Exists(fpath.c_str()))
						mEventTimes.LoadFromJson(fpath.c_str());
					else
						flag = (ProcessPolicyFlag) (flag | SpineData::eProcessAnimEvents);
				}
				if (flag & SpineData::eProcessAnimEvents) {
					mEventTimes.ProcessJson(jsonPath);
					FilePath jpp(jsonPath);
					std::string fpath = jpp.mDirectory + jpp.mFilename + "_events.json";
					mEventTimes.SaveToJson(fpath.c_str());
				}
			}

			// save files
			mJsonFile = jsonPath;
			mAtlasFile = atlasFile;
		}
	}

	// ------------------------------------------------------------------------
	#pragma region// Skeleton Anim Comp
	SkeletonAnimComp::SkeletonAnimComp()
		: mSpineData(0)
		, mpSkeleton(0)
		, mpAnimState(0)
		, mTimeScale(1.0f)
		, transform(0)
	{
	}
	SkeletonAnimComp::~SkeletonAnimComp()
	{}

	void SkeletonAnimComp::Update()
	{
		// update the skeleton
		if (mbPlaying) {
			f32 dt = (f32)aexTime->GetFrameTime();
			ApplyAnimation(dt);
		}

		DebugDraw();
	}
	void SkeletonAnimComp::ApplyAnimation(f32 dt)
	{
		// update skeleton animation
		//mpSkeleton->update(dt * mTimeScale);

		// update anim
		mpAnimState->update(dt * mTimeScale);

		// apply anim
		mpAnimState->apply(*mpSkeleton);

		//aexMessagingSystem->trigger_event(PostAnimEvent(), this); //MARTA: Uncomment when event system finished

		// update transform
		mpSkeleton->updateWorldTransform();
	}

	void SkeletonAnimComp::ReloadSpineInstances() {

		// sanity check
		if (!mSpineData)
			return;

		DeleteSpineInstances();

		// create the skeleton
		mpSkeleton = new spine::Skeleton(mSpineData->get()->GetSkelData());

		// create the animation state
		mpAnimState = new spine::AnimationState(mSpineData->get()->GetAnimStateData());
	}

	void SkeletonAnimComp::DeleteSpineInstances()
	{
		if (mpSkeleton)delete mpSkeleton;
		if (mpAnimState) delete mpAnimState;

		mpSkeleton = 0;
		mpAnimState = 0;
	}

	void SkeletonAnimComp::OnCreate()
	{
		// get owner transform
		transform = GetOwner()->GetComp<TransformComp>();

		if (mSpineData)
		{
			// create the skeleton and animstate
			ReloadSpineInstances();

			// intitialize the instances
			mpSkeleton->setToSetupPose();
			mpSkeleton->setPosition(0, 0);

			// set default animation
			if (mSpineData->get()->GetSkelData()->getAnimations().size())
				mpAnimState->setAnimation(0, mSpineData->get()->GetSkelData()->getAnimations()[0], true);

			// set default skin
			mpSkeleton->setSkin(NULL);
			mpSkeleton->setSlotsToSetupPose();
			mpAnimState->apply(*mpSkeleton);
		}
	}
	void SkeletonAnimComp::Shutdown()
	{
		// delete skeleton and animstate
		DeleteSpineInstances();
	}

	bool SkeletonAnimComp::Edit()
	{
		return false;
	}

	spine::Animation* SkeletonAnimComp::GetAnimationByName(const char* name) const
	{
		return mSpineData->get()->GetSkelData()->findAnimation(name);
	}
	std::string SkeletonAnimComp::GetCurrentAnimationName()
	{
		AEX_STRING animName = mpAnimState->getCurrent(0)->getAnimation()->getName().buffer();
		return animName;
	}
	void  SkeletonAnimComp::SetCurrentAnimation(const char* animName, bool loop)
	{
		if (GetCurrentAnimationName() == animName)
			return;
		if (AnimationExists(animName))
			mpAnimState->setAnimation(0, GetAnimationByName(animName), loop);
	}
	bool  SkeletonAnimComp::AnimationExists(const char* animName) const
	{
		return GetAnimationByName(animName) != nullptr;
	}
	void  SkeletonAnimComp::SetToSetupPose()
	{
		mpSkeleton->setToSetupPose();
	}
	void  SkeletonAnimComp::SetMixValue(const char* fromAnimName, const char* toAnimName, f32 mixDuration)
	{
		mSpineData->get()->GetAnimStateData()->setMix(spine::String(fromAnimName), spine::String(toAnimName), mixDuration);
	}
	f32  SkeletonAnimComp::GetMixValue(const char* fromAnimName, const char* toAnimName)
	{
		// get the animation 
		spine::Animation* fromAnim = GetAnimationByName(fromAnimName);
		spine::Animation* toAnim = GetAnimationByName(toAnimName);
		if (fromAnim && toAnim)
			return mSpineData->get()->GetAnimStateData()->getMix(fromAnim, toAnim);
		return 0.0f;
	}

	void SkeletonAnimComp::SetSkin(const char* skinName)
	{
		mpSkeleton->setSkin(skinName);
		mpSkeleton->setSlotsToSetupPose();
		mpAnimState->apply(*mpSkeleton);
	}

	void SkeletonAnimComp::DebugDraw()
	{
		Transform world = transform->World();
		AEMtx44 modelMtx = world.GetMatrix44();


		// try to get the whole bone hierarchy
		for (u32 i = 0; i < mpSkeleton->getBones().size(); ++i)
		{
			spine::Bone* bone = mpSkeleton->getBones()[i];
			AEVec3 pos = { bone->getWorldX(), bone->getWorldY(), 0.0f };
			pos = modelMtx * pos;

			aexGraphics->DrawCircle(pos.x, pos.y, 5);

			// draw line to parent
			if (bone->getParent())
			{
				AEVec3 parentPos =
				{ bone->getParent()->getWorldX(), bone->getParent()->getWorldY(), 0.0f };
				parentPos = modelMtx * parentPos;
				aexGraphics->DrawLine(pos.x, pos.y, parentPos.x, parentPos.y);
			}
		}

		static f32 bboxVertices[128];
		for (u32 i = 0; i < mpSkeleton->getSlots().size(); ++i)
		{
			spine::Slot* slot = mpSkeleton->getDrawOrder()[i];
			if (slot->getAttachment() && slot->getAttachment()->getRTTI().isExactly(spine::BoundingBoxAttachment::rtti))
			{
				spine::BoundingBoxAttachment* bbox = (spine::BoundingBoxAttachment*)slot->getAttachment();
				size_t vtxCount = bbox->getWorldVerticesLength();
				bbox->computeWorldVertices(*slot, 0, vtxCount, bboxVertices, 0, 2);
				for (int v = 0; v < vtxCount - 1; v += 2)
				{
					// draw a line between each of them
					AEVec3 bboxPt;
					AEVec3 bboxPt2;

					if (v == vtxCount - 2)
					{
						bboxPt = AEVec3(bboxVertices[v], bboxVertices[v + 1], 0);
						bboxPt2 = AEVec3(bboxVertices[0], bboxVertices[1], 0);
					}
					else
					{
						bboxPt = AEVec3(bboxVertices[v], bboxVertices[v + 1], 0);
						bboxPt2 = AEVec3(bboxVertices[v + 2], bboxVertices[v + 3], 0);
					}
					bboxPt = modelMtx * bboxPt;
					bboxPt2 = modelMtx * bboxPt2;

					aexGraphics->DrawLine(bboxPt.x, bboxPt.y, bboxPt2.x, bboxPt2.y);
				}

			}
		}
	}

	f32 SkeletonAnimComp::GetAnimTime(u32 trackIdx)
	{
		if (mpAnimState && trackIdx < mpAnimState->getTracks().size())
			return mpAnimState->getTracks()[trackIdx]->getAnimationTime();
		return -1.0f;
	}

	void SkeletonAnimComp::SetAnimTime(f32 time, u32 trackIdx)
	{
		if (mpAnimState && trackIdx < mpAnimState->getTracks().size())
			mpAnimState->getTracks()[trackIdx]->setTrackTime(time);
	}

	f32 SkeletonAnimComp::GetAnimDuration(u32 trackIdx)
	{
		if (mpAnimState && trackIdx < mpAnimState->getTracks().size()) {
			//f32 e = mpAnimState->getTracks()[trackIdx]->getAnimationEnd();
			//f32 s = mpAnimState->getTracks()[trackIdx]->getAnimationStart();
			//return e - s;
			return mpAnimState->getTracks()[trackIdx]->getAnimation()->getDuration();
		}
		return -1.0f;
	}
	
	#pragma endregion

	// ------------------------------------------------------------------------
	#pragma region// Helper Functions

	AEVec2 GetBonePosInWorld(spine::Bone* bone, TransformComp* transform) {
		auto bPos = AEVec2(bone->getWorldX(), bone->getWorldY());
		auto world = transform->World();
		return world.GetMatrix() * bPos;
	}
	void SetBonePosFromWorld(spine::Bone* bone, const AEVec2& posW, TransformComp* transform) {
		auto world = transform->World();
		auto bPosLocal = world.GetInvMatrix() * posW;
		if (bone->getParent())
		{
			auto parent = bone->getParent();
			parent->worldToLocal(bPosLocal.x, bPosLocal.y, bPosLocal.x, bPosLocal.y);
			bone->setX(bPosLocal.x);
			bone->setY(bPosLocal.y);
		}
		else
		{
			bone->setX(bPosLocal.x);
			bone->setY(bPosLocal.y);
		}

	}

	#pragma endregion

	// ------------------------------------------------------------------------
	#pragma region// Skeleton Renderable

	void SkeletonRenderable::OnCreate()
	{
		Renderable::OnCreate();
		mpSkelAnimComp = GetOwner()->GetComp<SkeletonAnimComp>();

		// manage our own model
		mpModel = new TResource<Model>();
		mpModel->SetRawResource(new Model, false);
		for (u32 i = 0; i < 4096; ++i)
			mpModel->get()->AddVertex(Vertex());
		mpModel->get()->UploadToGPU();
	}

	void SkeletonRenderable::Shutdown()
	{
		if (mpModel)delete mpModel;
		mpModel = 0;
	}

	void SkeletonRenderable::DrawMesh()
	{
		if (auto skeleton = mpSkelAnimComp->mpSkeleton)
		{
			// get ptr to model current resource
			auto model = mpModel->get();

			DebugAssert(model != nullptr, "spine render: model is null");

			auto gfx = aexGraphics;
			auto depthEnabled = gfx->GetDepthTestEnabled(); 
			//gfx->SetDepthTestEnabled(false);

			// vertex array used below to hold the attachment vertices when copying to the model.
			std::vector<Vertex> tmpVtx(100);
			Color skelColor = Color(skeleton->getColor().r, skeleton->getColor().g, skeleton->getColor().b, skeleton->getColor().a);
			u32 vtx_counter = 0;
			Texture* previous_tex = nullptr;

			// prepare the clipping attachment helpers
			// http://esotericsoftware.com/spine-clipping
			spine::SkeletonClipping clipping;
			spine::ClippingAttachment* clipAttachment;

			// draw the skeleton 
			for (u32 i = 0; i < skeleton->getSlots().size(); ++i)
			{
				spine::Slot* slot = skeleton->getDrawOrder()[i];
				if (!slot)continue;

				spine::Attachment* attachment = slot->getAttachment();
				if (!attachment)continue;

				// compute the attachment color
				Color color;
				Color slotColor = Color(slot->getColor().r, slot->getColor().g, slot->getColor().b, slot->getColor().a);
				
				if (attachment->getRTTI().isExactly(spine::RegionAttachment::rtti))
				{
					// cast in region attachment
					spine::RegionAttachment* regionAttachment = (spine::RegionAttachment*)attachment;

					// compute color 
					Color attColor = Color(regionAttachment->getColor().r, regionAttachment->getColor().g, regionAttachment->getColor().b, regionAttachment->getColor().a);
					color = slotColor * attColor;

					// retrieve texture
					TResource<Texture>* pTexRes = (TResource<Texture>*)((spine::AtlasRegion*)regionAttachment->getRendererObject())->page->getRendererObject();
					Texture* pTex = pTexRes->get();
					
					// bind the texture
					pTex->Bind();

					// set the vertex positions
					{
						int hola = model->GetVertexCount();
						//ensure that we have enough vertices left 
						DebugAssert(vtx_counter + 6 < model->GetVertexCount(), "not enough vertex data");
						if (vtx_counter + 6 < model->GetVertexCount())
						{
							// compute vertices of the region in world (i.e. the skeleton root space)
							// all the vertex data is indexed, so here we are going to receive only 
							// 4 vertex, when 6 are needed to form a quad. 
							{
								// obtain the vertex position from the region attachment
								regionAttachment->computeWorldVertices(
									*slot,			// pointer to the bone the region is attached to
									(f32*)tmpVtx.data(),		// pointer to the first element in the temp vertex array
									0,							// offset of the vertex to start copying from (always zero in our case).
									sizeof(Vertex) / sizeof(f32) // stride from one vertex to another
								);

								// copy uvs and colors a is
								// note that because the UVs are stored as arrays of floats, 
								// we must increment the index by 2 everytime. 
								for (u32 k = 0, l = 0; k < 4; ++k, l += 2) {
									tmpVtx[k].mTexCoord.x = regionAttachment->getUVs()[l];
									tmpVtx[k].mTexCoord.y = 1.0f - regionAttachment->getUVs()[l + 1];
									tmpVtx[k].mColor = color;
								}
							}

							// clip if necessary
							if (clipping.isClipping())
							{
								unsigned short triangles[] = { 0,1,2,0,2,3 };

								clipping.clipTriangles(
									(f32*)&tmpVtx[0],						// pointer to the first vertex world position as computed above. 
									triangles,								// index buffer
									6,										// count of indices
									(f32*)&tmpVtx[0].mTexCoord,				// pointer to the first vertex UV. 
									sizeof(Vertex) / sizeof(float));		// stride from one vertex to the next


								// copy data from the clip attachment
								size_t numIndices = clipping.getClippedTriangles().size();
								float* clippedPos = clipping.getClippedVertices().buffer();
								float* clippedUV = clipping.getClippedUVs().buffer();
								for (u32 k = 0; k < numIndices; ++k) {

									// Get the index of the vertex in the clipped data arrays (pos + uv). 
									// Note(Thomas): we multiply by 2 the index because the clipped data arrays 
									// are given as float arrays (instead of vec2). 
									int idx = clipping.getClippedTriangles()[k] * 2;

									// get the vertex position, uv and color
									auto pos = AEVec2(clippedPos[idx], clippedPos[idx + 1]);
									auto uv = AEVec2(clippedUV[idx], clippedUV[idx + 1]);

									// set model data
									model->SetVertexPos(vtx_counter + k, pos);
									model->SetVertexTex(vtx_counter + k, uv);
									model->SetVertexColor(vtx_counter + k, color);
								}

								// advance counter by the number of added vertices (in this case, the same as indices)
								vtx_counter += (u32)numIndices;
							}

							// copy the data to the vertex buffer
							else 
							{
								// simple idx buffer to copy easily the quad data. 
								u16 idxBuff[6] = { 0,1,2,0,2,3 };

								// set data and increment couinter
								for (u32 k = 0; k < 6; ++k) {
									model->SetVertexPos(vtx_counter + k, tmpVtx[idxBuff[k]].mPosition);
									model->SetVertexTex(vtx_counter + k, tmpVtx[idxBuff[k]].mTexCoord);
									model->SetVertexColor(vtx_counter + k, tmpVtx[idxBuff[k]].mColor);
								}

								// advance counter by the number of added vertices (in this case, 6 vertices)
								vtx_counter += 6;
							}
						}
					}
				}
				else if (attachment->getRTTI().isExactly(spine::MeshAttachment::rtti))
				{
					spine::MeshAttachment* meshAttachment = (spine::MeshAttachment*)attachment;

					// compute color 
					Color attColor = Color(meshAttachment->getColor().r, meshAttachment->getColor().g, meshAttachment->getColor().b, meshAttachment->getColor().a);
					color = slotColor * attColor;

					// retrieve texture
					TResource<Texture>* pTexRes = (TResource<Texture>*)((spine::AtlasRegion*)meshAttachment->getRendererObject())->page->getRendererObject();
					Texture* pTex = pTexRes->get();
					
					// bind the texture
					pTex->Bind();

					// set the vertex positions
					{
						// get the number of indices in the mesh (each corresponds to a vertex index). 
						size_t numIndices = meshAttachment->getTriangles().size();

						//ensure that we have enough vertices left 
						DebugAssert(vtx_counter + numIndices < model->GetVertexCount(), "not enough vertex data");
						if (vtx_counter + numIndices < model->GetVertexCount())
						{
							// Obtain vertex data from mesh attachment and store into tmpVtx
							{
								// get the number of vertices in the attachment
								size_t numVertices = meshAttachment->getWorldVerticesLength() / 2;

								// resize if the attachment has more vertices
								if (tmpVtx.size() < numVertices)tmpVtx.resize(numVertices);

								// compute world position of vertices and copy ito tmpVtx
								meshAttachment->computeWorldVertices(
									*slot,											// slot with the mesh attachment
									0,												// offset from the slot vertices to start transforming
									numVertices * 2,								// number of vertices to copy (float count, 2 per position xy)
									(f32*)tmpVtx.data(),							// pointer to the start of the vertex 
									0,												// offset to the first available vertex
									sizeof(Vertex) / sizeof(float));				// stride is the size of an entire vertex

								// copy the vertex UV in the mesh data
								f32* meshUVs = meshAttachment->getUVs().buffer();
								for (u32 k = 0; k < numVertices; ++k)
									// copy from the uv buffer, flip the y value
									tmpVtx[k].mTexCoord = { meshUVs[(k * 2)], 1.0f - meshUVs[(k * 2) + 1] };
							}

							// apply clipping if necessary
							if (clipping.isClipping())
							{
								// perform clipping
								clipping.clipTriangles(
									(f32*)&tmpVtx[0],							// pointer to the first vertex world position as computed above. 
									meshAttachment->getTriangles().buffer(),	// index buffer
									meshAttachment->getTriangles().size(),		// count of indices
									(f32*)&tmpVtx[0].mTexCoord,					// pointer to the first vertex UV. 
									sizeof(Vertex) / sizeof(float));			// stride from one vertex to the next

								// copy data from the clip attachment
								numIndices = clipping.getClippedTriangles().size();
								float* clippedPos = clipping.getClippedVertices().buffer();
								float* clippedUV = clipping.getClippedUVs().buffer();
								for (u32 k = 0; k < numIndices; ++k) {

									// Get the index of the vertex in the clipped data arrays (pos + uv). 
									// Note(Thomas): we multiply by 2 the index because the clipped data arrays 
									// are given as float arrays (instead of vec2). 
									int idx = clipping.getClippedTriangles()[k] * 2;

									// get the vertex position, uv and color
									auto pos = AEVec2(clippedPos[idx], clippedPos[idx+1]);
									auto uv =  AEVec2(clippedUV[idx], clippedUV[idx+1]);

									// set model data
									model->SetVertexPos(vtx_counter + k, pos);
									model->SetVertexTex(vtx_counter + k, uv);
									model->SetVertexColor(vtx_counter + k, color);
								}
							}

							// otherwise copy as is
							else
							{
								// At this point, the vertex positions in world have been copied to the temporary vertex data
								// we're going to read each index in the index buffer (meshAttachment->getTriangles()) and copy 
								// the corresponding vertex position in the tmpVtx data. 
								// the color is the same for all vertices and is computed at the start of the loop
								// IMPORTANT: Our *naive* implementation doesn't actually use an index buffer and so we are 
								// potentially adding duplicated vertices. 
								for (u32 k = 0; k < numIndices; ++k) 
								{
									// get the index of the vertex to copy
									int idx = meshAttachment->getTriangles()[k];

									// get the vertex position, uv and color
									model->SetVertexPos(vtx_counter + k, tmpVtx[idx].mPosition);
									model->SetVertexTex(vtx_counter + k, tmpVtx[idx].mTexCoord); 
									model->SetVertexColor(vtx_counter + k, color);
								}
							}

							// advance counter by the number of added vertices (in this case, the same as indices)
							vtx_counter += (u32)numIndices;
						}
					}
				}
				else if (attachment->getRTTI().isExactly(spine::ClippingAttachment::rtti))
				{
					// initialize clipping procedure. 
					clipAttachment = (spine::ClippingAttachment*)attachment;
					clipping.clipStart(*slot, clipAttachment);
				}

				// end clipping if necessary. Note: spine does check for 
				// argument slot being the end slot marker that should end clipping
				// otherwise, the clipping will not end. 
				clipping.clipEnd(*slot);
				DrawTempMesh(vtx_counter);
			}

			// draw the remainder of the accumulated mesh.
			//DrawTempMesh(vtx_counter);

			// reset render pipeline state
			gfx->SetDepthTestEnabled(depthEnabled);
		}
	}

	void SkeletonRenderable::DrawTempMesh(u32& vtx_counter)
	{
		// draw and flush
		mpModel->get()->ReloadToGPU(0, vtx_counter - 1);
		mpModel->get()->Draw(0, vtx_counter - 1);

		// flush
		vtx_counter = 0;
	}

	#pragma endregion
}