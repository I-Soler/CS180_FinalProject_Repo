// AEXFactory.h
// Purpose of the factory is to centralized the memory allocation for classes in our engine (derives from IBase). 
// create components by type read from a string. 
#pragma once
#include <Core\AEXBase.h>
#include <Core\AEXSingleton.h>
namespace AEX
{
	struct ICreator {
		virtual IBase* Create() = 0;
	};
	template<typename T>
	struct TCreator : public ICreator {
		virtual IBase* Create()
		{
			return new T();
		}
	};
	class Factory : public IBase
	{
		AEX_RTTI_DECL(Factory, IBase);
		AEX_SINGLETON(Factory) {}

	private:
		std::map<std::string, ICreator*> mCreators;

	public:

		// Creator registration.
		void AddCreator(const char* class_type, ICreator* new_creator);
		template<typename T> void Register();

		// Object creation.
		IBase* Create(const char* class_type);
		template<typename T> T* Create();

		// static interface to register all types. 
		void RegisterAllTypes();
	};
}

#define aexFactory AEX::Singletons::Get<AEX::Factory>()
#include <Core/AEXFactory.inl>