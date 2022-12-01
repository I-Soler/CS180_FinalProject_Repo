#pragma once
#include <Core/AEXBase.h>
#include "Scene/Scene.h"

namespace AEX {
	class IComp : public IBase {
		AEX_RTTI_DECL(IComp, IBase);

		friend class GameObject;	// GameObject::AddComp, comp->mOwner = this;

		// default constructor (does nothing). 
		IComp() {}
		virtual ~IComp() {}  // destructor flags the class as pure virtual

		// basic state functions
		virtual void AddToSystem() {}
		virtual void RemoveFromSystem() {}
		virtual void OnCreate() {}		// happens when the object's component is just created. 
		virtual void Initialize() {}	// happens when the whole scene is created
		virtual void Shutdown() {}		// happens when the object is about to be destroyed
		virtual void Update(void) {}		// default update of the object (might not be used)
		virtual bool Edit() { return false; }			// happens when a component is changed with the editor

		// Get Parent
		GameObject* GetOwner() { return mOwner; }
		Space* GetOwnerSpace() { return mOwner->mOwnerSpace; }
		//Space* GetOwnerSpace();		// defined in Scene.cpp for Space definition

		// TODO: Get Owner Component

	protected:
		GameObject* mOwner = nullptr; // note: GameObject should always be set to a value
	};
}