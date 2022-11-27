#pragma once
#include <Core/AEXCore.h>
#include <Composition/AEXGameObject.h>
namespace AEX
{
	class TransformComp;
	class Space : public GameObject
	{
		friend class Scene;
		AEX_RTTI_DECL(Space, GameObject);


	public:
		Space();
		//~Space();

		// IS THIS NEEDED????
		//#pragma region// state management
		//void OnCreate()override;
		//void Initialize()override;
		//void Shutdown()override;
		//#pragma endregion

		// update the space
		void Update();

		#pragma region// Object Management
		
		// direct object creation
		GameObject* NewObject(const char* name);
		void DeleteObject(GameObject* obj);

		// todo: finding objects
		GameObject* FindObjectByName(const char* name);
		GameObject* FindObjectByTag(unsigned tag);
		std::list<GameObject*> FindObjectsByTag(unsigned tag);

		#pragma endregion

		
		// Transform Management		
		void AddTransform(TransformComp* tr);
		void RemoveTransform(TransformComp* tr);
		void UpdateHierarchy();

		// Loading/Saving
		void StreamRead(const nlohmann::json& j) override;
		void StreamWrite(nlohmann::json& j) const override;

	protected: 

		// detele dead objects
		void DeleteDeadObjects();

		std::list<GameObject*> mDeletedObjects;
		std::list<TransformComp*> mRootTransforms;
	};

	class Scene : public IBase
	{
		AEX_RTTI_DECL(Scene, IBase);
		AEX_SINGLETON(Scene) {};

	public:
		

		// System state functions
		bool Initialize();
		void Update();
		void Shutdown();

		// Space management
		Space*	GetMainSpace();								// default Space in each Scene
		Space*	GetSpace(const char* spaceName);			// get a Space by name
		Space*	NewSpace(const char* spaceName);	// create a new Space by name
		void	DestroySpace(const char* spaceName);		// destroy Space by name
		void	Reset();									// destroy all and create main Space

		std::vector<Space*> GetAllSpaces();

		// Space's world transform update
		void UpdateAllSpaceHierarchies();
		void DeleteAllSpaceDeadObjects();

		// wrappers for object creation (always call the Space)
		GameObject* NewObject(const char* objName);
		GameObject* NewObject(const char * spaceName, const char* objName);

		//Search
		GameObject* FindObjectByNameInSpace(Space* space, const char* name);
		GameObject* FindObjectByNameInAnySpace(const char* name);

		// Loading/Saving
		void StreamRead(const nlohmann::json& j) override;
		void StreamWrite(nlohmann::json& j) const override;
		void LoadFile(const char* fileName, bool initSpaces = true);
		void SaveFile(const char* fileName);

		void InitializeSpaces();	// call Initialize on all Spaces
	private:
		void DestroyAllSpaces();
		void DestroySpace(Space* space);
		void CreateMainSpace();

		// container of Spaces -> by default this should always contain at least one Space
		std::vector<Space*> mSpaces;
	};
}
#define aexScene AEX::Singletons::Get<AEX::Scene>()