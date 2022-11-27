
namespace AEX
{
    template<typename T>
    T* Singletons::Get()
    {
        // return value
        IBase* singleton_instance = nullptr;

        // use the rtti type as a key to find the singleton
        const AEX::Rtti* rtti = &T::TYPE();
        auto it = sSingletons.find(rtti);

        // the singleton doesn't exist
        if (it == sSingletons.end())// doesn't exits
        {
            // create and add
            singleton_instance = new T();
            sSingletons[rtti] = singleton_instance;
        }
        // the singleton exits
        else singleton_instance = it->second;

        return dynamic_cast<T*>(singleton_instance);
    }

    template<typename T>
    void Singletons::Delete()
    {
        // use the rtti type as a key to find the singleton
        const AEX::Rtti* rtti = &T::TYPE();
        auto it = sSingletons.find(rtti);

        // delete if exists
        if (it != sSingletons.end()) {
            delete it->second;
            sSingletons.erase(it);
        }
    }
}