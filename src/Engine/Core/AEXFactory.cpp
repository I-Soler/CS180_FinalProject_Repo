#include <Core\AEXContainers.h>
#include <Core\AEXFactory.h>

namespace AEX {

	// adds a new creator associatged to a typename. 
	void Factory::AddCreator(const char* class_type, ICreator* new_creator)
	{
		// first check if the creator exists already, if so, free it. 
		auto it = mCreators.find(class_type);
		if (it != mCreators.end())// exists, we overwrite
			delete it->second;

		// now register the creator
		mCreators[class_type] = new_creator;
	}

	// Creates a class deriving from IBase given a registered type name (from RTTI).
	IBase* Factory::Create(const char* class_type)
	{
		// first get the creator associated with the type
		auto it = mCreators.find(class_type);
		if (it != mCreators.end() && it->second != nullptr) // exists
			return it->second->Create();

		// creator doesn't exist, we don't know how to create the requested type. 
		return nullptr;
	}


}