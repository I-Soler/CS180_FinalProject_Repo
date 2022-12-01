#pragma once
#include <Logic/AEXLogicSystem.h>
#include <Graphics/Components/AEXGfxComps.h> // Renderable
namespace spine
{
	class SkeletonData;
	class AnimationStateData;
	class Skeleton;
	class AnimationState;
	class Animation;
	class Atlas;
	class Bone;
}

namespace AEX
{
	

	// ------------------------------------------------------------------------
	#pragma region// EventTimeData
	struct EventToAnimTimeMap
	{
		bool mLoaded = false;
		std::map<std::string, std::map<std::string, std::vector<f32>>> AnimEvents;
		void ProcessJson(const char* filename);
		void SaveToJson(const char* filename);
		void LoadFromJson(const char* filename);
	};
	#pragma endregion

	
	// ------------------------------------------------------------------------
	#pragma region// Spine Data

	/*
	* Resource to hold the necessary data for generating a Spine Skeleton. 
	* It uses spine::Atlas, spine::SkeletonData and spine::AnimationStateData
	* which are generated in the Load() function. 
	* Note that like any resource, multiple instances can (and probably will)
	* share a same SpineData resource.
	*/
	class SpineData : public IBase
	{
		AEX_RTTI_DECL(SpineData, IBase);

	public:
		// constructor and destructor
		SpineData();
		~SpineData();

		// Load the spine data from a json and atlas file.
		enum ProcessPolicyFlag { eNo_Processing= 0, eLoadAnimEvents=1, eProcessAnimEvents = 2};
		void Load(const char* jsonPath, const char* atlasFile, ProcessPolicyFlag flag = eNo_Processing);

		// Free the spine data. 
		void FreeData();

		// Getters for data
		spine::Atlas* GetAtlas()						{ return mAtlas; }
		spine::SkeletonData* GetSkelData()				{ return mSkeletonData; }
		spine::AnimationStateData* GetAnimStateData()	{ return mAnimStateData; }
		const FilePath & GetJsonFilepath()				{return mJsonFile;}
		const FilePath & GetAtlasFilepath()				{return mAtlasFile;}

		// Json Scale
		float GetJsonScale()							{ return mJsonScale;}
		void  SetJsonScale(float jsonScale)				{ mJsonScale = jsonScale;}

		// advanced. 
		EventToAnimTimeMap & GetEventTimes()		{ return mEventTimes; }

	private:

		// Spine resources. 
		spine::Atlas* mAtlas = 0;
		spine::SkeletonData* mSkeletonData = 0;
		spine::AnimationStateData* mAnimStateData = 0;

		// Event time data
		EventToAnimTimeMap mEventTimes; 

		// scale to apply when loading the skeleton data
		float mJsonScale = 1.0f;

		// paths to the source json and atlas files.
		FilePath mJsonFile, mAtlasFile;
	};

	#pragma endregion

	// ------------------------------------------------------------------------
	#pragma region// Skeleton Animation Comp

	class PostAnimEvent : public Event //public IEvent
	{
		AEX_RTTI_DECL(PostAnimEvent, Event);	//MARTA: Uncomment lines when event system finished
	};

	class SkeletonAnimComp : public LogicComp
	{
		AEX_RTTI_DECL(SkeletonAnimComp, LogicComp);

	public:
		SkeletonAnimComp();
		virtual ~SkeletonAnimComp();

		// state functions
		void Update() override;
		void OnCreate() override;
		void Shutdown() override;

		bool Edit() override;

		// create the spine instances using the spine data
		void ReloadSpineInstances();
		void DeleteSpineInstances();

		// applies the current animation on the skeleton
		void ApplyAnimation(f32 dt = 0.0f);

		// Helpers and wrappers around Spine functionality
		std::string GetCurrentAnimationName();

		// Animation management

		// gets an animation for that spine skeleton by name. 
		spine::Animation* GetAnimationByName(const char* name) const;

		// sets the current animation if found. Uses the function above to find it. 
		// can specify if the animation should be looped. 
		void  SetCurrentAnimation(const char* animName, bool loop = false);

		// checks if the animation exists. 
		bool  AnimationExists(const char* animName)const;

		// resets the animation to the setup pose. 
		void  SetToSetupPose();

		// Get/Set the mix value between two animations. 
		f32  GetMixValue(const char* fromAnimName, const char* toAnimName);
		void SetMixValue(const char* fromAnimName, const char* toAnimName, f32 mixDuration);

		// Set current skin by name. 
		void SetSkin(const char* skinName);

		// debug draws the skeleton. 
		void DebugDraw();

		// Get and set anim time
		f32 GetAnimTime(u32 trackIdx = 0);
		void SetAnimTime(f32 time, u32 trackIdx = 0);

		// get animation duration
		f32 GetAnimDuration(u32 trackIdx = 0);

	public:

		// owner object's transform component 
		// set in OnCreate
		TransformComp* transform = nullptr;

		// Spine resources
		// TODO: Change this to a TResource<SpineData> *
		// Note: This should be fetched on serialize using the resource manager. 
		TResource<SpineData>* mSpineData = nullptr;

		// Spine Instances
		spine::Skeleton* mpSkeleton = nullptr;
		spine::AnimationState* mpAnimState = nullptr;

		// Animation time scale.
		float					mTimeScale = 1.0f;
		bool					mbPlaying = true;
	};
	#pragma endregion

	// ------------------------------------------------------------------------
	#pragma region// Skeleton Renderable
	/* SkeletonRenderable
	* Renderable component to draw the spine skeleton
	* Each frame, it will use the Spine API to generate the vertices
	* that make the mesh of the skeleton. 
	*/
	class SkeletonRenderable : public Renderable
	{
		AEX_RTTI_DECL(SkeletonRenderable, Renderable)

	public:

		// Allocates memory for the Skeleton Mesh
		void OnCreate() override;

		// Frees the memory for the Skeleton Mesh
		void Shutdown() override;

		// Overriden from Renderable. Takes care 
		// of generating the mesh vertices and drawing them
		void DrawMesh() override;

	protected:
		// components
		SkeletonAnimComp* mpSkelAnimComp = 0;

		// Intermediate step for drawing the skeleton mesh
		// when it uses more than one texture atlas to draw. 
		void DrawTempMesh(u32& vtx_counter);
	};
	#pragma endregion

	// ------------------------------------------------------------------------
	#pragma region// Helper functions

	AEVec2 GetBonePosInWorld(spine::Bone* bone, TransformComp* transform);
	void SetBonePosFromWorld(spine::Bone* bone, const AEVec2& posW, TransformComp* transform);

	#pragma endregion

}