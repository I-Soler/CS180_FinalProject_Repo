namespace AEX {

	template<typename T> void Factory::Register() {
		std::string class_type = T::TYPE().GetName();
		AddCreator(class_type.c_str(), new TCreator<T>());
	}
	template<typename T> T* Factory::Create() {
		std::string class_type = T::TYPE().GetName();
		auto it = mCreators.find(class_type);
		if (it == mCreators.end()) // creator doesn't exist
			Register<T>();

		return dynamic_cast<T*>(mCreators[class_type]->Create());
	}
}