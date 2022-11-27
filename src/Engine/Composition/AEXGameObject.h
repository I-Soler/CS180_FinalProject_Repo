#pragma once
#include <Core/AEXBase.h>
#include <Core/AEXContainers.h>
#include <Core/AEXFactory.h>

namespace AEX {
	class IComp;
	class GameObject : public IBase
	{
		friend class Space;

		AEX_RTTI_DECL(GameObject, IBase);

		#pragma region// State updates 

		// constructor/destructor
		GameObject();
		~GameObject();

		virtual void OnCreate();
		virtual void Initialize();
		virtual void Shutdown();

		#pragma endregion

		#pragma region// Component Management

		IComp* AddComp(IComp* comp);
		void   RemoveComp(IComp* comp);
		IComp* GetComp(const char* compTypeStr);
		GameObject* clone();

		// inherited from IBase
		bool SetEnabled(bool enabled) override;

		#pragma endregion
		
		#pragma region// Hierarchy management
		void AddChild(GameObject* obj);
		void RemoveChild(GameObject* obj);		// detach child from this
		const std::vector<GameObject*>& GetChildren() { return mChildren; }
		const std::vector<IComp*>& GetAllComp() { return mComponents; }
		void SetParent(GameObject* parent);
		#pragma endregion

		#pragma region// template interface 

		template <typename T> T* GetComp()
		{
			return dynamic_cast<T*>(GetComp(T::TYPE().GetName().c_str()));
		}
		template <typename T> T* NewComp() 
		{	
			return dynamic_cast<T*>(AddComp(aexFactory->Create<T>()));
		}
		template <typename T> void RemoveComp() {
			RemoveComp(GetComp<T>());
		}

		#pragma endregion

		// Loading/Saving
		void StreamRead(const nlohmann::json& j) override;
		void StreamWrite(nlohmann::json& j) const override;

		// TODO(Iñaki): This should be protected
		//Space* GetSpace() { return mOwnerSpace; }
		// public variables
		GameObject* mParent = nullptr;
		Space* mOwnerSpace = nullptr;
		unsigned mTag = DEFAULT_TAG;
		enum
		{
			DEFAULT_TAG,
			PLAYER_TAG,
			ENEMY_TAG,
			TAG_NUM
		};

	protected:
		// Component container
		std::vector<IComp*> mComponents;

		// Game Object hierarchy
		std::vector<GameObject*> mChildren;

		bool mbDestroyed = false;
	};
}